// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxVideoConnectionManager.h"
#include <QStringList>

#include "vtkRenderWindow.h"
#include <QTimer>

#include "sscLabeledComboBoxWidget.h"
#include "sscVideoRep.h"
#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "cxSettings.h"
#include "cxDataLocations.h"
#include "cxImageSenderFactory.h"
#include "cxProcessWrapper.h"
#include "cxVideoConnection.h"
#include "sscStringDataAdapterXml.h"

namespace cx
{

VideoConnectionManager::VideoConnectionManager()
{
	mReconnectInterval = 400;
	mOptions = ssc::XmlOptionFile(DataLocations::getXmlSettingsFile(), "CustusX").descend("video");

	QStringList connectionOptions;
	QString defaultConnection = "Direct Link";
#ifdef __APPLE__
	defaultConnection = "Local Server";	// grabber server is the preferred method on Mac.
#endif
	connectionOptions << "Local Server" << "Direct Link" << "Remote Server";
	mConnectionMethod = ssc::StringDataAdapterXml::initialize("Connection", "",
			"Method for connecting to Video Server",
			defaultConnection,
			connectionOptions,
			mOptions.getElement());
	connect(mConnectionMethod.get(), SIGNAL(changed()), this, SIGNAL(settingsChanged()));

	mConnectWhenLocalServerRunning = 0;

	mIniScript.reset(new ProcessWrapper("Init Script"));
	mProcess.reset(new ProcessWrapper("Local Video Server"));
	connect(mProcess->getProcess(), SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(serverProcessStateChanged(QProcess::ProcessState)));

	mRTSource.reset(new VideoConnection());
	connect(mRTSource.get(), SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));
	connect(mRTSource.get(), SIGNAL(fps(int)), this, SIGNAL(fps(int)));
	connect(mRTSource.get(), SIGNAL(videoSourcesChanged()), this, SIGNAL(videoSourcesChanged()));
}

VideoConnectionManager::~VideoConnectionManager()
{
	mRTSource->disconnectServer();
}

void VideoConnectionManager::setLocalServerExecutable(QString commandline)
{
	settings()->setValue("IGTLink/localServer", commandline);
}

QString VideoConnectionManager::getLocalServerExecutable()
{
	QString cmd = settings()->value("IGTLink/localServer").toString();
	return cmd;
}

void VideoConnectionManager::setPort(int port)
{
	settings()->setValue("IGTLink/port", port);
}

int VideoConnectionManager::getPort()
{
	QVariant var = settings()->value("IGTLink/port");
	if (var.canConvert<int> ())
		return var.toInt();
	return 18333;
}

void VideoConnectionManager::setInitScript(QString filename)
{
	settings()->setValue("IGTLink/initScript", filename);
}

QString VideoConnectionManager::getInitScript()
{
	QString cmd = settings()->value("IGTLink/initScript").toString();
	return cmd;
}

QProcess* VideoConnectionManager::getProcess()
{
	return mProcess->getProcess();
}

bool VideoConnectionManager::getUseLocalServer2()
{
	return mConnectionMethod->getValue() == "Local Server";
}

bool VideoConnectionManager::getUseDirectLink2()
{
	return mConnectionMethod->getValue() == "Direct Link";
}

/**Get list of recent hosts. The first is the current.
 *
 */
QStringList VideoConnectionManager::getHostHistory()
{
	QStringList hostHistory = settings()->value("IGTLink/hostHistory").toStringList();
	if (hostHistory.isEmpty())
		hostHistory << "Localhost";
	return hostHistory;
}

QString VideoConnectionManager::getHost()
{
	return this->getHostHistory().front(); // history will always contain elements.
}

void VideoConnectionManager::setHost(QString host)
{
	QStringList history = this->getHostHistory();
	history.prepend(host);
	for (int i = 1; i < history.size(); ++i)
		if (history[i] == host)
			history.removeAt(i);
	while (history.size() > 5)
		history.removeLast();

	settings()->setValue("IGTLink/hostHistory", history);
}

QStringList VideoConnectionManager::getDirectLinkArgumentHistory()
{
	QStringList history = settings()->value("IGTLink/directLinkArgumentHistory").toStringList();
	if (history.isEmpty())
		history << "";
	return history;
}

void VideoConnectionManager::setLocalServerArguments(QString commandline)
{
	QStringList history = this->getDirectLinkArgumentHistory();
	history.prepend(commandline);
	for (int i = 1; i < history.size(); ++i)
		if (history[i] == commandline)
			history.removeAt(i);
	while (history.size() > 5)
		history.removeLast();

	settings()->setValue("IGTLink/directLinkArgumentHistory", history);
}

QString VideoConnectionManager::getLocalServerArguments()
{
	return this->getDirectLinkArgumentHistory().front(); // history will always contain elements.
}

void VideoConnectionManager::launchServer()
{
	if (!this->getUseLocalServer2())
	{
		ssc::messageManager()->sendError("Ignoring Launch local server: Must select local server");
		return;
	}

	QString program = this->getLocalServerExecutable();
	QStringList arguments = this->getLocalServerArguments().split(" ");

//	std::cout << "program: " << program << std::endl;
//	std::cout << "arguments: " << arguments.join("--") << std::endl;
	mProcess->launch(program, arguments);
}

void VideoConnectionManager::connectServer()
{
	if (!mRTSource->isConnected())
	{
		if (this->getUseLocalServer2())
			mRTSource->connectServer("LocalHost", this->getPort());
		else
			mRTSource->connectServer(this->getHost(), this->getPort());
	}

	this->delayedAutoConnectServer();
}

/** Attempt to connect to server at a later time if mConnectWhenLocalServerRunning >0,
 * and the server is unconnected.
 *
 */
void VideoConnectionManager::delayedAutoConnectServer()
{
	if (mRTSource->isConnected())
		mConnectWhenLocalServerRunning = 0;

	if (mConnectWhenLocalServerRunning)
	{
		--mConnectWhenLocalServerRunning;
		QTimer::singleShot(mReconnectInterval, this, SLOT(connectServer())); // the process need some time to get its tcp server up and listening. GrabberServer seems to need more than 500ms
	}
}


void VideoConnectionManager::launchAndConnectServer()
{
	if (mRTSource->isConnected())
		return;

	mIniScript->launch(this->getInitScript());

	if (this->getUseDirectLink2())
	{
		QString commandline = this->getLocalServerArguments();
		StringMap args = extractCommandlineOptions(commandline.split(" "));
		mRTSource->directLink(args);
		return;
	}

	if (this->getUseLocalServer2())
	{
		this->launchServer();

		if (this->getProcess()->state() != QProcess::Running)
		{
			mConnectWhenLocalServerRunning = 5; // attempt N connects
		}
		else
		{
			this->connectServer();
		}
	}
	else
	{
		this->connectServer();
	}
}

void VideoConnectionManager::serverProcessStateChanged(QProcess::ProcessState newState)
{
	if (newState == QProcess::Running)
	{
		this->delayedAutoConnectServer();
	}
}

std::vector<ssc::VideoSourcePtr> VideoConnectionManager::getVideoSources()
{
	return mRTSource->getVideoSources();
}

void VideoConnectionManager::disconnectServer()
{
	mRTSource->disconnectServer();
}

bool VideoConnectionManager::isConnected() const
{
	return mRTSource->isConnected();
}

}//end namespace cx
