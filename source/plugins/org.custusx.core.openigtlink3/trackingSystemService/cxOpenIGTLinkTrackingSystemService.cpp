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
#include "cxOpenIGTLinkTool.h"
#include "cxProfile.h"

#include "cxToolConfigurationParser.h"
#include "cxTrackerConfigurationImpl.h"

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

OpenIGTLinkTrackingSystemService::OpenIGTLinkTrackingSystemService(NetworkHandlerPtr networkHandler) :
	mNetworkHandler(networkHandler)
{
	if(mNetworkHandler == NULL)
		return;

	connect(mNetworkHandler.get(), &NetworkHandler::connected,this, &OpenIGTLinkTrackingSystemService::serverIsConnected);
	connect(mNetworkHandler.get(), &NetworkHandler::disconnected, this, &OpenIGTLinkTrackingSystemService::serverIsDisconnected);
	connect(mNetworkHandler.get(), &NetworkHandler::transform, this, &OpenIGTLinkTrackingSystemService::receiveTransform);
	//connect(mNetworkHandler.get(), &NetworkHandler::calibration, this, &OpenIGTLinkTrackingSystemService::receiveCalibration);
	connect(mNetworkHandler.get(), &NetworkHandler::probedefinition, this, &OpenIGTLinkTrackingSystemService::receiveProbedefinition);

	connect(this, &OpenIGTLinkTrackingSystemService::setInternalState, this, &OpenIGTLinkTrackingSystemService::internalSetState);

	this->setConfigurationFile(profile()->getToolConfigFilePath());
}

OpenIGTLinkTrackingSystemService::~OpenIGTLinkTrackingSystemService()
{
	this->deconfigure();
}

QString OpenIGTLinkTrackingSystemService::getUid() const
{
	return "org.custusx.core.openigtlink3";
}

void OpenIGTLinkTrackingSystemService::setState(const Tool::State val)
{
	emit setInternalState(val);
}

void OpenIGTLinkTrackingSystemService::internalSetState(Tool::State val)
{
	TrackingSystemService::internalSetState(val);
	mState = val;
	emit stateChanged();
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

void OpenIGTLinkTrackingSystemService::configure()
{
	//parse
	ConfigurationFileParser configParser(mConfigurationFilePath, mLoggingFolder);

	if(!configParser.getTrackingSystemImplementation().contains(TRACKING_SYSTEM_IMPLEMENTATION_IGTLINK, Qt::CaseInsensitive))
	{
		CX_LOG_DEBUG() << "OpenIGTLinkTrackingSystemService::configure(): Not using OpenIGTLink tracking.";
		this->setState(Tool::tsNONE);
		return;
	}

	CX_LOG_DEBUG() << "OpenIGTLinkTrackingSystemService::configure(): Using OpenIGTLink3 tracking";

	std::vector<ConfigurationFileParser::ToolStructure> toolList = configParser.getToolListWithMetaInformation();

	//Create tools
	for(std::vector<ConfigurationFileParser::ToolStructure>::iterator it = toolList.begin(); it != toolList.end(); ++it)
	{
		ToolFileParser toolParser((*it).mAbsoluteToolFilePath, mLoggingFolder);
		ToolFileParser::ToolInternalStructurePtr internalTool = toolParser.getTool();

		QString devicename = internalTool->mUid;
		OpenIGTLinkToolPtr newTool = OpenIGTLinkToolPtr(new OpenIGTLinkTool((*it), internalTool));
		mTools[devicename] = newTool;
		if(newTool->isReference())
			mReference = newTool;
	}
	if(!mReference)
		CX_LOG_WARNING() << "OpenIGTLinkTrackingSystemService::configure() Got no reference tool";

	mState = Tool::tsCONFIGURED;//Setting state directly. Cannot get setState() to work with test
}

void OpenIGTLinkTrackingSystemService::deconfigure()
{
	if (!this->isConfigured())
		return;

	mState = Tool::tsNONE;//Setting state directly. Cannot get setState() to work with test

	mTools.clear();
	mReference.reset();
}

void OpenIGTLinkTrackingSystemService::serverIsConnected()
{
	this->setState(Tool::tsINITIALIZED);
	this->setState(Tool::tsTRACKING);
}

void OpenIGTLinkTrackingSystemService::serverIsDisconnected()
{
	this->setState(Tool::tsCONFIGURED);
	this->setState(Tool::tsINITIALIZED);
}

void OpenIGTLinkTrackingSystemService::receiveTransform(QString devicename, Transform3D transform, double timestamp)
{
//	CX_LOG_DEBUG() << "receiveTransform for: " << devicename;
	OpenIGTLinkToolPtr tool = this->getTool(devicename);
	if(tool)
	{
		tool->toolTransformAndTimestampSlot(transform, timestamp);
	}
}

void OpenIGTLinkTrackingSystemService::receiveCalibration(QString devicename, Transform3D calibration)
{
	CX_LOG_DEBUG() << "receiveCalibration for: " << devicename;
	OpenIGTLinkToolPtr tool = this->getTool(devicename);
	if(tool)
		tool->setCalibration_sMt(calibration);
}

void OpenIGTLinkTrackingSystemService::receiveProbedefinition(QString devicename, ProbeDefinitionPtr definition)
{
	OpenIGTLinkToolPtr tool = this->getTool(devicename);
	if(tool)
	{
		ProbePtr probe = tool->getProbe();
		if(probe)
		{
			ProbeDefinition old_def = probe->getProbeDefinition();
			definition->setUid(old_def.getUid());
			definition->applySoundSpeedCompensationFactor(old_def.getSoundSpeedCompensationFactor());

			probe->setProbeDefinition(*(definition.get()));
			emit stateChanged();
		}
	}
}

OpenIGTLinkToolPtr OpenIGTLinkTrackingSystemService::getTool(QString devicename)
{
//	CX_LOG_DEBUG() << "OpenIGTLinkTrackingSystemService::getTool: " << devicename;

	std::map<QString, OpenIGTLinkToolPtr>::iterator it;
	for (it = mTools.begin(); it != mTools.end(); ++it)
	{
		OpenIGTLinkToolPtr tool = it->second;
		if (tool->isThisTool(devicename))
		{
			return tool;
		}
	}
	return OpenIGTLinkToolPtr();
}


} /* namespace cx */
