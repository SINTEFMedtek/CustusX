/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#include "cxVideoConnectionManager.h"

#include <boost/bind.hpp>
#include <QStringList>
#include <QTimer>

#include "vtkRenderWindow.h"

#include "cxTypeConversions.h"
#include "cxReporter.h"
#include "cxSettings.h"
#include "cxDataLocations.h"
#include "cxProcessWrapper.h"
#include "cxVideoConnection.h"
#include "cxStringDataAdapterXml.h"
#include "cxLogger.h"
#include "cxStreamer.h"
#include "cxCommandlineImageStreamerFactory.h"
#include "cxVideoServiceBackend.h"

//#include "cxSimulatedImageStreamerService.h"

namespace cx
{

VideoConnectionManager::VideoConnectionManager(VideoServiceBackendPtr backend)
{
	mBackend = backend;
	mReconnectInterval = 400;

	mConnectionMethod = "Direct Link";
#ifdef __APPLE__
	mConnectionMethod = "Local Server";
#endif

	mConnectWhenLocalServerRunning = 0;
	mIniScriptProcess.reset(new ProcessWrapper("Init Script"));
	mLocalVideoServerProcess.reset(new ProcessWrapper("Local Video Server"));
	connect(mLocalVideoServerProcess->getProcess(), SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(serverProcessStateChanged(QProcess::ProcessState)));
	mVideoConnection.reset(new VideoConnection(mBackend));
	connect(mVideoConnection.get(), SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));
	connect(mVideoConnection.get(), SIGNAL(fps(QString, int)), this, SIGNAL(fps(QString, int)));
	connect(mVideoConnection.get(), SIGNAL(videoSourcesChanged()), this, SIGNAL(videoSourcesChanged()));

	mServiceListener.reset(new ServiceTrackerListener<StreamerService>(
													 mBackend->getPluginContext(),
													 boost::bind(&VideoConnectionManager::onServiceAdded, this, _1),
													 boost::function<void (StreamerService*)>(),
													 boost::bind(&VideoConnectionManager::onServiceRemoved, this, _1),
													 backend->getPluginFramework()
													 ));
	mServiceListener->open();
}

VideoConnectionManager::~VideoConnectionManager()
{
//	std::cout << "VideoConnectionManager destructor" << std::endl;
//	mServiceListener.reset();//needed?
	mVideoConnection->disconnectServer();
}

void VideoConnectionManager::onServiceAdded(StreamerService* service)
{
//	std::cout << "VideoConnectionManager:: Service added!!! " << service->getName() << std::endl;
}

void VideoConnectionManager::onServiceRemoved(StreamerService *service)
{
//	std::cout << "VideoConnectionManager::Service removed!!! " << service->getName() << std::endl;
	this->disconnectServer();//Disconnect to be safe. Can be improved by only disconneting if the removed service is running
}

QString VideoConnectionManager::getConnectionMethod()
{
	return mConnectionMethod;
}

void VideoConnectionManager::setConnectionMethod(QString connectionMethod)
{
	if(!connectionMethod.isEmpty())
		mConnectionMethod = connectionMethod;
	else
		reporter()->sendWarning("Trying to set connection method to empty string");
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
	return mConnectionMethod == "Local Server";
}

bool VideoConnectionManager::useDirectLink()
{
	return mConnectionMethod == "Direct Link";
}

bool VideoConnectionManager::useRemoteServer()
{
	return mConnectionMethod == "Remote Server";
}

//bool VideoConnectionManager::useSimulatedServer()
//{
//	return mConnectionMethod->getValue() == "Simulation Server";
//}

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
		reportError("Ignoring Launch local server: Must select local server");
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

void VideoConnectionManager::launchAndConnectServer(QString connectionMethod)
{
	mConnectionMethod = connectionMethod;
	this->launchAndConnectServer();
}

void VideoConnectionManager::launchAndConnectServer()
{
	if (mVideoConnection->isConnected())
		return;

	this->runScript();

	if (useDirectLink())
		this->setupAndRunDirectLinkClient();
	else if (useLocalServer())
		this->launchAndConnectUsingLocalServer();
	else if (useRemoteServer())
		this->connectServer();
	else if(!this->connectToService())
		reportError("Could not determine which server to launch.");
}

bool VideoConnectionManager::connectToService()
{
	StreamerService* service = mServiceListener->getService(mConnectionMethod);
	if (!service)
		return false;

	mVideoConnection->runDirectLinkClient(service);
	return true;
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
