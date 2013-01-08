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

/*
 * cxOpenIGTLinkConnection.cpp
 *
 *  \date Jan 25, 2011
 *      \author christiana
 */

#include "cxVideoConnection.h"
#include <QStringList>

#include "vtkRenderWindow.h"

#include "sscLabeledComboBoxWidget.h"
#include "sscVideoRep.h"
#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "cxSettings.h"
#include "cxDataLocations.h"
#include "cxImageSenderFactory.h"
#include "cxProcessWrapper.h"

namespace cx
{

VideoConnection::VideoConnection()
{
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
//	mServer = new QProcess(this);
	connect(mProcess->getProcess(), SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(serverProcessStateChanged(QProcess::ProcessState)));
//	connect(mServer, SIGNAL(error(QProcess::ProcessError)), this, SLOT(serverProcessError(QProcess::ProcessError)));
//
//	connect(mServer, SIGNAL(readyRead()), this, SLOT(serverProcessReadyRead()));
//	mServer->setProcessChannelMode(QProcess::MergedChannels);
//	mServer->setReadChannel(QProcess::StandardOutput);

	mRTSource.reset(new OpenIGTLinkRTSource());
	connect(getVideoSource().get(), SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));
	connect(mRTSource.get(), SIGNAL(fps(int)), this, SIGNAL(fps(int))); // thread-bridging connection
}

VideoConnection::~VideoConnection()
{
	mRTSource->disconnectServer();
//	// avoid getting crash reports: disable signal
//	disconnect(mServer, SIGNAL(error(QProcess::ProcessError)), this, SLOT(serverProcessError(QProcess::ProcessError)));
//	mServer->close();
}

void VideoConnection::setLocalServerExecutable(QString commandline)
{
	settings()->setValue("IGTLink/localServer", commandline);
}

QString VideoConnection::getLocalServerExecutable()
{
	QString cmd = settings()->value("IGTLink/localServer").toString();
	return cmd;
}

void VideoConnection::setPort(int port)
{
	settings()->setValue("IGTLink/port", port);
}

int VideoConnection::getPort()
{
	QVariant var = settings()->value("IGTLink/port");
	if (var.canConvert<int> ())
		return var.toInt();
	return 18333;
}

void VideoConnection::setInitScript(QString filename)
{
	settings()->setValue("IGTLink/initScript", filename);
}

QString VideoConnection::getInitScript()
{
	QString cmd = settings()->value("IGTLink/initScript").toString();
	return cmd;
}

QProcess* VideoConnection::getProcess()
{
	return mProcess->getProcess();
}

//void VideoConnection::setUseLocalServer(bool use)
//{
//	settings()->setValue("IGTLink/useLocalServer", use);
//}
//
//bool VideoConnection::getUseLocalServer()
//{
//	QVariant var = settings()->value("IGTLink/useLocalServer");
//	if (var.canConvert<bool> ())
//		return var.toBool();
//	return true;
//}
//
//void VideoConnection::setUseDirectLink(bool use)
//{
//	settings()->setValue("IGTLink/useDirectLink", use);
//}
//
//bool VideoConnection::getUseDirectLink()
//{
//	QVariant var = settings()->value("IGTLink/useDirectLink");
//	if (var.canConvert<bool> ())
//		return var.toBool();
//	return true;
//}

bool VideoConnection::getUseLocalServer2()
{
	return mConnectionMethod->getValue() == "Local Server";
}

bool VideoConnection::getUseDirectLink2()
{
	return mConnectionMethod->getValue() == "Direct Link";
}

/**Get list of recent hosts. The first is the current.
 *
 */
QStringList VideoConnection::getHostHistory()
{
	QStringList hostHistory = settings()->value("IGTLink/hostHistory").toStringList();
	if (hostHistory.isEmpty())
		hostHistory << "Localhost";
	return hostHistory;
}

QString VideoConnection::getHost()
{
	return this->getHostHistory().front(); // history will always contain elements.
}

void VideoConnection::setHost(QString host)
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

QStringList VideoConnection::getDirectLinkArgumentHistory()
{
	QStringList history = settings()->value("IGTLink/directLinkArgumentHistory").toStringList();
	if (history.isEmpty())
		history << "";
	return history;
}

void VideoConnection::setLocalServerArguments(QString commandline)
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

QString VideoConnection::getLocalServerArguments()
{
	return this->getDirectLinkArgumentHistory().front(); // history will always contain elements.
}

void VideoConnection::launchServer()
{
//	//  QString program = "/Users/christiana/christiana/workspace/CustusX3/build_RelWithDebInfo/modules/OpenIGTLinkServer/cxOpenIGTLinkServer";
//	//  QStringList arguments;
//	//  arguments << "18333" <<  "/Users/christiana/Patients/20101126T114627_Lab_66.cx3/US_Acq/USAcq_20100909T111205_5.mhd";
//
//	QString commandline = this->getLocalServerExecutable() + " " + this->getLocalServerArguments();
//
//	if (this->getLocalServerExecutable().isEmpty())
//		return;
//	if (mServer->state() != QProcess::NotRunning)
//		return;
////	if (this->getHost().toUpper() != "LOCALHOST")
//	if (!this->getUseLocalServer2())
//	{
//		ssc::messageManager()->sendError("Ignoring Launch local server: Must select local server");
//		return;
//	}
//
////	QStringList text = commandline.split(" ");
//	QString program = this->getLocalServerExecutable();
//	QStringList arguments = this->getLocalServerArguments().split(" ");
////	arguments.pop_front();
//
//	if (!QFileInfo(program).isAbsolute())
//		program = DataLocations::getBundlePath() + "/" + program;
//
//	if (!QFileInfo(program).exists())
//	{
//		ssc::messageManager()->sendError("Cannot find IGTLink server " + program);
//		return;
//	}
//
//	ssc::messageManager()->sendInfo("Launching local IGTLink server " + program + " with arguments " + arguments.join(
//		", "));
//
//	if (mServer->state() == QProcess::NotRunning)
//		mServer->start(program, arguments);

	if (!this->getUseLocalServer2())
	{
		ssc::messageManager()->sendError("Ignoring Launch local server: Must select local server");
		return;
	}

	QString program = this->getLocalServerExecutable();
	QStringList arguments = this->getLocalServerArguments().split(" ");

	mProcess->launch(program, arguments);
}

void VideoConnection::connectServer()
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
void VideoConnection::delayedAutoConnectServer()
{
	if (mRTSource->isConnected())
		mConnectWhenLocalServerRunning = 0;

	if (mConnectWhenLocalServerRunning)
	{
		--mConnectWhenLocalServerRunning;
		QTimer::singleShot(400, this, SLOT(connectServer())); // the process need some time to get its tcp server up and listening. GrabberServer seems to need more than 500ms
	}
}


void VideoConnection::launchAndConnectServer()
{
	if (this->getVideoSource()->isConnected())
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

void VideoConnection::serverProcessStateChanged(QProcess::ProcessState newState)
{
	if (newState == QProcess::Running)
	{
		this->delayedAutoConnectServer();
	}
}

//void VideoConnection::serverProcessError(QProcess::ProcessError error)
//{
//	QString msg;
//	msg += "Video server reported an error: ";
//
//	switch (error)
//	{
//	case QProcess::FailedToStart:
//		msg += "Failed to start";
//		break;
//	case QProcess::Crashed:
//		msg += "Crashed";
//		break;
//	case QProcess::Timedout:
//		msg += "Timed out";
//		break;
//	case QProcess::WriteError:
//		msg += "Write Error";
//		break;
//	case QProcess::ReadError:
//		msg += "Read Error";
//		break;
//	case QProcess::UnknownError:
//		msg += "Unknown Error";
//		break;
//	default:
//		msg += "Invalid error";
//	}
//
//	ssc::messageManager()->sendError(msg);
//}
//
//void VideoConnection::serverProcessStateChanged(QProcess::ProcessState newState)
//{
//	if (newState == QProcess::Running)
//	{
//		ssc::messageManager()->sendInfo("Video Source Server running.");
//		this->delayedAutoConnectServer();
//	}
//	if (newState == QProcess::NotRunning)
//	{
//		ssc::messageManager()->sendInfo("Video Source Server not running.");
//	}
//	if (newState == QProcess::Starting)
//	{
//		ssc::messageManager()->sendInfo("Video Source Server starting.");
//	}
//}
//
//void VideoConnection::serverProcessReadyRead()
//{
//	ssc::messageManager()->sendInfo(QString(mServer->readAllStandardOutput()));
//}

}//end namespace cx
