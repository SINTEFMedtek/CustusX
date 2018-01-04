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

#include "cxLogger.h"
#include "cxNetworkConnection.h"
#include "cxNetworkConnectionHandle.h"
#include "cxOpenIGTLinkTool.h"

#include "cxTrackerConfigurationImpl.h"
#include "cxToolConfigurationParser.h"

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

OpenIGTLinkTrackingSystemService::OpenIGTLinkTrackingSystemService(NetworkConnectionHandlePtr connection) :
	mState(Tool::tsNONE),
	mConnection(connection)

{
//	qRegisterMetaType<Tool::State>("Tool::State");//Needed?

	if(mConnection == NULL)
        return;

	NetworkConnection* client = mConnection->getNetworkConnection();

	connect(this, &OpenIGTLinkTrackingSystemService::connectToServer, client, &NetworkConnection::requestConnect);
	connect(this, &OpenIGTLinkTrackingSystemService::disconnectFromServer, client, &NetworkConnection::requestDisconnect);
	connect(client, &NetworkConnection::connected, this, &OpenIGTLinkTrackingSystemService::serverIsConnected);
	connect(client, &NetworkConnection::disconnected, this, &OpenIGTLinkTrackingSystemService::serverIsDisconnected);
	connect(client, &NetworkConnection::transform, this, &OpenIGTLinkTrackingSystemService::receiveTransform);
	connect(client, &NetworkConnection::calibration, this, &OpenIGTLinkTrackingSystemService::receiveCalibration);
	connect(client, &NetworkConnection::probedefinition, this, &OpenIGTLinkTrackingSystemService::receiveProbedefinition);
}

OpenIGTLinkTrackingSystemService::~OpenIGTLinkTrackingSystemService()
{
    this->deconfigure();
}

QString OpenIGTLinkTrackingSystemService::getUid() const
{
    return "org.custusx.core.openigtlink";
}

Tool::State OpenIGTLinkTrackingSystemService::getState() const
{
    return mState;
}

void OpenIGTLinkTrackingSystemService::setState(const Tool::State val)
{
    if (mState==val)
        return;

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
		retval.reset(new TrackerConfigurationImpl());
		retval->setTrackingSystemImplementation(TRACKING_SYSTEM_IMPLEMENTATION_IGTLINK);
    return retval;
}

ToolPtr OpenIGTLinkTrackingSystemService::getReference()
{
    return mReference;
}

void OpenIGTLinkTrackingSystemService::setLoggingFolder(QString loggingFolder)
{
	Q_UNUSED(loggingFolder);
}

void OpenIGTLinkTrackingSystemService::configure()
{
    this->serverIsConfigured();
}

void OpenIGTLinkTrackingSystemService::deconfigure()
{
    mTools.clear();
    mReference.reset();
    this->serverIsDeconfigured();
}

void OpenIGTLinkTrackingSystemService::initialize()
{
    //emit connectToServer();
}

void OpenIGTLinkTrackingSystemService::uninitialize()
{
    emit disconnectFromServer();
}

void OpenIGTLinkTrackingSystemService::startTracking()
{
    //emit startListenToServer();
    //emit connectToServer();
}

void OpenIGTLinkTrackingSystemService::stopTracking()
{
    //emit stopListenToServer();
    emit disconnectFromServer();
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
    this->internalSetState(Tool::tsTRACKING);
}

void OpenIGTLinkTrackingSystemService::serverIsDisconnected()
{
    this->internalSetState(Tool::tsCONFIGURED);
    this->internalSetState(Tool::tsINITIALIZED);
}

void OpenIGTLinkTrackingSystemService::receiveTransform(QString devicename, Transform3D transform, double timestamp)
{
    CX_LOG_DEBUG() << "transform";
    OpenIGTLinkToolPtr tool = this->getTool(devicename);
    tool->toolTransformAndTimestampSlot(transform, timestamp);
}

void OpenIGTLinkTrackingSystemService::receiveCalibration(QString devicename, Transform3D calibration)
{
    OpenIGTLinkToolPtr tool = this->getTool(devicename);
    tool->setCalibration_sMt(calibration);
}

void OpenIGTLinkTrackingSystemService::receiveProbedefinition(QString devicename, ProbeDefinitionPtr definition)
{
    OpenIGTLinkToolPtr tool = this->getTool(devicename);
    ProbePtr probe = tool->getProbe();
    ProbeDefinition old_def = probe->getProbeDefinition();
    definition->setUid(old_def.getUid());
    definition->applySoundSpeedCompensationFactor(old_def.getSoundSpeedCompensationFactor());

    probe->setProbeDefinition(*(definition.get()));
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
