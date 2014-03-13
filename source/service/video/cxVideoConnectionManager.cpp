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

//#include "cxLabeledComboBoxWidget.h"
#include "cxTypeConversions.h"
#include "cxMessageManager.h"
#include "cxSettings.h"
#include "cxDataLocations.h"
#include "cxImageSenderFactory.h"
#include "cxProcessWrapper.h"
#include "cxVideoConnection.h"
#include "cxStringDataAdapterXml.h"
#include "cxLogger.h"

namespace cx
{

VideoConnectionManager::VideoConnectionManager(VideoServiceBackendPtr backend)
{
	mBackend = backend;
	mReconnectInterval = 400;
	mOptions = XmlOptionFile(DataLocations::getXmlSettingsFile(), "CustusX").descend("video");

	QStringList connectionOptions;
	QString defaultConnection = "Direct Link";
	connectionOptions << "Local Server" << "Direct Link" << "Remote Server" << "Simulation Server";
	mConnectionMethod = StringDataAdapterXml::initialize("Connection", "", "Method for connecting to Video Server", defaultConnection, connectionOptions, mOptions.getElement());
	connect(mConnectionMethod.get(), SIGNAL(changed()), this, SIGNAL(connectionMethodChanged()));
	mConnectWhenLocalServerRunning = 0;
	mIniScriptProcess.reset(new ProcessWrapper("Init Script"));
	mLocalVideoServerProcess.reset(new ProcessWrapper("Local Video Server"));
	connect(mLocalVideoServerProcess->getProcess(), SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(serverProcessStateChanged(QProcess::ProcessState)));
	mVideoConnection.reset(new VideoConnection(mBackend));
	connect(mVideoConnection.get(), SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));
	connect(mVideoConnection.get(), SIGNAL(fps(QString, int)), this, SIGNAL(fps(QString, int)));
	connect(mVideoConnection.get(), SIGNAL(videoSourcesChanged()), this, SIGNAL(videoSourcesChanged()));
}

VideoConnectionManager::~VideoConnectionManager()
{
	mVideoConnection->disconnectServer();
}

StringDataAdapterXmlPtr VideoConnectionManager::getConnectionMethod()
{
	return mConnectionMethod;
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
	if (var.canConvert<int>())
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

QProcess* VideoConnectionManager::getLocalVideoServerProcess()
{
	return mLocalVideoServerProcess->getProcess();
}

bool VideoConnectionManager::useLocalServer()
{
	return mConnectionMethod->getValue() == "Local Server";
}

bool VideoConnectionManager::useDirectLink()
{
	return mConnectionMethod->getValue() == "Direct Link";
}

bool VideoConnectionManager::useRemoteServer()
{
	return mConnectionMethod->getValue() == "Remote Server";
}

bool VideoConnectionManager::useSimulatedServer()
{
	return mConnectionMethod->getValue() == "Simulation Server";
}

QStringList VideoConnectionManager::getHostHistory()
{
	QStringList hostHistory = settings()->value("IGTLink/hostHistory").toStringList();
	if (hostHistory.isEmpty())
		hostHistory << "Localhost";

	return hostHistory;
}
QString VideoConnectionManager::getHost()
{
	return this->getHostHistory().front();
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
	return this->getDirectLinkArgumentHistory().front();
}

void VideoConnectionManager::launchServer()
{
	if (!this->useLocalServer())
	{
		messageManager()->sendError("Ignoring Launch local server: Must select local server");
		return;
	}
	QString program = this->getLocalServerExecutable();
	QStringList arguments = this->getLocalServerArguments().split(" ");
	mLocalVideoServerProcess->launchWithRelativePath(program, arguments);
}

void VideoConnectionManager::connectServer()
{
	if (!mVideoConnection->isConnected())
	{
		if (this->useLocalServer())
			mVideoConnection->runIGTLinkedClient("LocalHost", this->getPort());
		else
			mVideoConnection->runIGTLinkedClient(this->getHost(), this->getPort());
	}
	this->delayedAutoConnectServer();
}

void VideoConnectionManager::delayedAutoConnectServer()
{
	if (mVideoConnection->isConnected())
		mConnectWhenLocalServerRunning = 0;

	if (mConnectWhenLocalServerRunning)
	{
		--mConnectWhenLocalServerRunning;
		QTimer::singleShot(mReconnectInterval, this, SLOT(connectServer())); // the process need some time to get its tcp server up and listening. GrabberServer seems to need more than 500ms
	}
}

void VideoConnectionManager::launchAndConnectServer()
{
	if (mVideoConnection->isConnected())
		return;

	this->runScript();

	if (useDirectLink() || useSimulatedServer())
		this->setupAndRunDirectLinkClient();
	else if (useLocalServer())
		this->launchAndConnectUsingLocalServer();
	else if (useRemoteServer())
		this->connectServer();
	else
		messageManager()->sendError("Could not determine which server to launch.");
}

void VideoConnectionManager::serverProcessStateChanged(QProcess::ProcessState newState)
{
	if (newState == QProcess::Running)
		this->delayedAutoConnectServer();
}

std::vector<VideoSourcePtr> VideoConnectionManager::getVideoSources()
{
	return mVideoConnection->getVideoSources();
}

VideoConnectionPtr VideoConnectionManager::getVideoConnection()
{
	return mVideoConnection;
}

void VideoConnectionManager::setReconnectInterval(int interval)
{
	mReconnectInterval = interval;
}

void VideoConnectionManager::disconnectServer()
{
	mVideoConnection->disconnectServer();
}

bool VideoConnectionManager::isConnected() const
{
	return mVideoConnection->isConnected();
}

void VideoConnectionManager::runScript()
{
	if (this->getInitScript().isEmpty())
		return;
	mIniScriptProcess->launchWithRelativePath(this->getInitScript());
}

bool VideoConnectionManager::localVideoServerIsRunning()
{
	return this->getLocalVideoServerProcess()->state() != QProcess::Running;
}

void VideoConnectionManager::setupAndRunDirectLinkClient()
{
	QString commandline = this->getLocalServerArguments();
	StringMap args = extractCommandlineOptions(commandline.split(" "));
	mVideoConnection->runDirectLinkClient(args);
}

void VideoConnectionManager::launchAndConnectUsingLocalServer()
{
	this->launchServer();
	if (this->localVideoServerIsRunning())
		mConnectWhenLocalServerRunning = 5;
	else
		this->connectServer();
}

}//end namespace cx
