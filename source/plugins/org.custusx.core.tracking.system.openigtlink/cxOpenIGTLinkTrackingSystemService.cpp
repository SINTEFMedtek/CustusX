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

#include "cxOpenIGTLinkTrackingSystemService.h"

#include "ctkPluginContext.h"
#include "cxLogger.h"
#include "cxOpenIGTLinkClient.h"
#include "cxOpenIGTLinkSessionManager.h"
#include "cxOpenIGTLinkTool.h"

namespace cx
{

std::vector<ToolPtr> toVector(std::map<QString, OpenIGTLinkToolPtr> map)
{
    std::vector<ToolPtr> retval;
    std::map<QString, OpenIGTLinkToolPtr>::iterator it = map.begin();
    for(; it!= map.end(); ++it)
    {
        retval.push_back(it->second);
    }
    return retval;
}

OpenIGTLinkTrackingSystemService::OpenIGTLinkTrackingSystemService() :
    mState(Tool::tsNONE),
    mIp("10.218.140.127"),
    mPort(18944)
{
    // CLIENT START
    OpenIGTLinkClient *client = new OpenIGTLinkClient;
    client->moveToThread(&mOpenIGTLinkThread);
    connect(&mOpenIGTLinkThread, &QThread::finished, client, &QObject::deleteLater);
    connect(this, &OpenIGTLinkTrackingSystemService::connectToServer, client, &OpenIGTLinkClient::requestConnect);
    connect(this, &OpenIGTLinkTrackingSystemService::disconnectFromServer, client, &OpenIGTLinkClient::requestDisconnect);
    connect(this, &OpenIGTLinkTrackingSystemService::startListenToServer, client, &OpenIGTLinkClient::requestStartProcessingMessages);
    connect(this, &OpenIGTLinkTrackingSystemService::stopListenToServer, client, &OpenIGTLinkClient::requestStopProcessingMessages);
    connect(client, &OpenIGTLinkClient::connected, this, &OpenIGTLinkTrackingSystemService::serverIsConnected);
    connect(client, &OpenIGTLinkClient::disconnected, this, &OpenIGTLinkTrackingSystemService::serverIsDisconnected);
    connect(client, &OpenIGTLinkClient::startedProcessingMessages, this, &OpenIGTLinkTrackingSystemService::serverStartedProcessingMessages);
    connect(client, &OpenIGTLinkClient::stoppedProcessingMessages, this, &OpenIGTLinkTrackingSystemService::serverStoppedProcessingMessages);
    connect(client, &OpenIGTLinkClient::transform, this, &OpenIGTLinkTrackingSystemService::receiveTransform);
    //CLIENT END

    //SESSIONMANAGER START
    //I think the session manager might be buggy, not changed since 2012... try client instead
    /*
    OpenIGTLinkSessionManager *session = new OpenIGTLinkSessionManager;

    session->moveToThread(&mOpenIGTLinkThread);
    connect(&mOpenIGTLinkThread, &QThread::finished, session, &QObject::deleteLater);
    connect(this, &OpenIGTLinkTrackingSystemService::connectToServer, session, &OpenIGTLinkSessionManager::requestConnect);
    connect(this, &OpenIGTLinkTrackingSystemService::disconnectFromServer, session, &OpenIGTLinkSessionManager::requestDisconnect);
    connect(this, &OpenIGTLinkTrackingSystemService::startListenToServer, session, &OpenIGTLinkSessionManager::requestStartProcessingMessages);
    connect(this, &OpenIGTLinkTrackingSystemService::stopListenToServer, session, &OpenIGTLinkSessionManager::requestStopProcessingMessages);
    connect(session, &OpenIGTLinkSessionManager::connected, this, &OpenIGTLinkTrackingSystemService::serverIsConnected);
    connect(session, &OpenIGTLinkSessionManager::disconnected, this, &OpenIGTLinkTrackingSystemService::serverIsDisconnected);
    connect(session, &OpenIGTLinkSessionManager::startedProcessingMessages, this, &OpenIGTLinkTrackingSystemService::serverStartedProcessingMessages);
    connect(session, &OpenIGTLinkSessionManager::stoppedProcessingMessages, this, &OpenIGTLinkTrackingSystemService::serverStoppedProcessingMessages);
    */
    //SESSIONMANAGER END
}

OpenIGTLinkTrackingSystemService::~OpenIGTLinkTrackingSystemService()
{
    this->deconfigure();
    CX_LOG_CHANNEL_DEBUG("janne beate") << "shutting down opentigtlink thread and destructing OpenIGTLinkTrackingSystemService";
}

QString OpenIGTLinkTrackingSystemService::getUid() const
{
    return "org.custusx.core.tracking.system.openigtlink";
}

Tool::State OpenIGTLinkTrackingSystemService::getState() const
{
    return mState;
}

void OpenIGTLinkTrackingSystemService::setState(const Tool::State val)
{
    if (mState==val)
        return;

    CX_LOG_CHANNEL_DEBUG("janne beate ") << "setState " << val;

    if (val > mState) // up
    {
        if (val == Tool::tsTRACKING)
            this->startTracking();
        else if (val == Tool::tsINITIALIZED)
            this->initialize();
        else if (val == Tool::tsCONFIGURED)
            this->configure();
    }
    else // down
    {
        if (val == Tool::tsINITIALIZED)
            this->stopTracking();
        else if (val == Tool::tsCONFIGURED)
            this->uninitialize();
        else if (val == Tool::tsNONE)
            this->deconfigure();
    }
}

std::vector<ToolPtr> OpenIGTLinkTrackingSystemService::getTools()
{
    return toVector(mTools);
}

TrackerConfigurationPtr OpenIGTLinkTrackingSystemService::getConfiguration()
{
    TrackerConfigurationPtr retval;
    return retval;
}

ToolPtr OpenIGTLinkTrackingSystemService::getReference()
{
    return mReference;
}

void OpenIGTLinkTrackingSystemService::setLoggingFolder(QString loggingFolder)
{}

void OpenIGTLinkTrackingSystemService::getPackage()
{
    CX_LOG_CHANNEL_DEBUG("janne beate ") << "Package arrived in CustusX.";
}

void OpenIGTLinkTrackingSystemService::configure()
{
    CX_LOG_CHANNEL_DEBUG("janne beate ") << "configure";
    mOpenIGTLinkThread.start();
    this->serverIsConfigured();
}

void OpenIGTLinkTrackingSystemService::deconfigure()
{
    CX_LOG_CHANNEL_DEBUG("janne beate ") << "deconfigure";
    mOpenIGTLinkThread.quit();
    mOpenIGTLinkThread.wait();
    mTools.clear();
    mReference.reset();
    this->serverIsDeconfigured();
}

void OpenIGTLinkTrackingSystemService::initialize()
{
    CX_LOG_CHANNEL_DEBUG("janne beate ") << "initialize";
    emit connectToServer(mIp, mPort);
}

void OpenIGTLinkTrackingSystemService::uninitialize()
{
    CX_LOG_CHANNEL_DEBUG("janne beate ") << "uninitialize";
    emit disconnectFromServer();
}

void OpenIGTLinkTrackingSystemService::startTracking()
{
    CX_LOG_CHANNEL_DEBUG("janne beate ") << "startTracking";
    emit startListenToServer();
}

void OpenIGTLinkTrackingSystemService::stopTracking()
{
    CX_LOG_CHANNEL_DEBUG("janne beate ") << "stopTracking";
    emit stopListenToServer();
}

void OpenIGTLinkTrackingSystemService::serverIsConfigured()
{
    this->internalSetState(Tool::tsCONFIGURED);
}

void OpenIGTLinkTrackingSystemService::serverIsDeconfigured()
{
    this->internalSetState(Tool::tsNONE);
}

void OpenIGTLinkTrackingSystemService::serverIsConnected()
{
    this->internalSetState(Tool::tsINITIALIZED);
}

void OpenIGTLinkTrackingSystemService::serverIsDisconnected()
{
    this->internalSetState(Tool::tsCONFIGURED);
}

void OpenIGTLinkTrackingSystemService::serverStartedProcessingMessages()
{
    this->internalSetState(Tool::tsTRACKING);
}

void OpenIGTLinkTrackingSystemService::serverStoppedProcessingMessages()
{
    this->internalSetState(Tool::tsINITIALIZED);
}

void OpenIGTLinkTrackingSystemService::receiveTransform(QString devicename, Transform3D transform, double timestamp)
{
    CX_LOG_CHANNEL_DEBUG("janne beate ") << "*******";
    CX_LOG_CHANNEL_DEBUG("janne beate ") << "DeviceName " << devicename;
    CX_LOG_CHANNEL_DEBUG("janne beate ") << "Transform " << transform;
    CX_LOG_CHANNEL_DEBUG("janne beate ") << "Timestamp " << timestamp;
    OpenIGTLinkToolPtr tool = this->getTool(devicename);
    tool->toolTransformAndTimestampSlot(transform, timestamp);

}

void OpenIGTLinkTrackingSystemService::internalSetState(Tool::State state)
{
    mState = state;
    emit stateChanged();
}

OpenIGTLinkToolPtr OpenIGTLinkTrackingSystemService::getTool(QString devicename)
{
    OpenIGTLinkToolPtr retval;
    std::map<QString, OpenIGTLinkToolPtr>::iterator it = mTools.find(devicename);
    if(it == mTools.end())
    {
        retval = OpenIGTLinkToolPtr(new OpenIGTLinkTool(devicename));
        mTools[devicename] = retval;
        //todo: will this work?
        emit stateChanged();
    }
    else
    {
        retval = it->second;
    }

    return retval;
}


} /* namespace cx */
