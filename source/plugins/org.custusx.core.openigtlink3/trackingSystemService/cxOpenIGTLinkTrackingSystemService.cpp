/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxOpenIGTLinkTrackingSystemService.h"

#include "cxLogger.h"
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

OpenIGTLinkTrackingSystemService::OpenIGTLinkTrackingSystemService(NetworkHandlerPtr networkHandler) :
	mState(Tool::tsNONE),
	mNetworkHandler(networkHandler)

{
	if(mNetworkHandler == NULL)
		return;

	connect(mNetworkHandler.get(), &NetworkHandler::connected,this, &OpenIGTLinkTrackingSystemService::serverIsConnected);
	connect(mNetworkHandler.get(), &NetworkHandler::disconnected, this, &OpenIGTLinkTrackingSystemService::serverIsDisconnected);
	connect(mNetworkHandler.get(), &NetworkHandler::transform, this, &OpenIGTLinkTrackingSystemService::receiveTransform);
	//connect(mNetworkHandler.get(), &NetworkHandler::calibration, this, &OpenIGTLinkTrackingSystemService::receiveCalibration);
	connect(mNetworkHandler.get(), &NetworkHandler::probedefinition, this, &OpenIGTLinkTrackingSystemService::receiveProbedefinition);
}

OpenIGTLinkTrackingSystemService::~OpenIGTLinkTrackingSystemService()
{
	this->deconfigure();
}

QString OpenIGTLinkTrackingSystemService::getUid() const
{
	return "org.custusx.core.openigtlink3";
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
	return retval;
}

ToolPtr OpenIGTLinkTrackingSystemService::getReference()
{
	return mReference;
}

void OpenIGTLinkTrackingSystemService::setLoggingFolder(QString loggingFolder)
{}

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

void OpenIGTLinkTrackingSystemService::receiveTransform(QString devicename, igtlio::BaseConverter::EQUIPMENT_TYPE equipmentType, Transform3D transform, double timestamp)
{
//	CX_LOG_DEBUG() << "receiveTransform for: " << devicename;
	OpenIGTLinkToolPtr tool = this->getTool(devicename, equipmentType);
	tool->toolTransformAndTimestampSlot(transform, timestamp);
}

void OpenIGTLinkTrackingSystemService::receiveCalibration(QString devicename, igtlio::BaseConverter::EQUIPMENT_TYPE equipmentType, Transform3D calibration)
{
	CX_LOG_DEBUG() << "receiveCalibration for: " << devicename;
	OpenIGTLinkToolPtr tool = this->getTool(devicename, equipmentType);
	tool->setCalibration_sMt(calibration);
}

void OpenIGTLinkTrackingSystemService::receiveProbedefinition(QString devicename, igtlio::BaseConverter::EQUIPMENT_TYPE equipmentType, ProbeDefinitionPtr definition)
{
//	CX_LOG_DEBUG() << "receiveProbedefinition for: " << devicename << " equipmentType: " << equipmentType;
	OpenIGTLinkToolPtr tool = this->getTool(devicename, equipmentType);
	if(tool)
	{
		ProbePtr probe = tool->getProbe();
		if(probe)
		{
			CX_LOG_DEBUG() << "receiveProbedefinition. Tool is probe: " << devicename;
			ProbeDefinition old_def = probe->getProbeDefinition();
			definition->setUid(old_def.getUid());
			definition->applySoundSpeedCompensationFactor(old_def.getSoundSpeedCompensationFactor());

			probe->setProbeDefinition(*(definition.get()));
			emit stateChanged();
		}
		else
		{
			CX_LOG_DEBUG() << "receiveProbedefinition. Tool is not probe: " << devicename;
		}
	}
}

void OpenIGTLinkTrackingSystemService::internalSetState(Tool::State state)
{
	mState = state;
	emit stateChanged();
}

OpenIGTLinkToolPtr OpenIGTLinkTrackingSystemService::getTool(QString devicename, igtlio::BaseConverter::EQUIPMENT_TYPE equipmentType)
{
//	CX_LOG_DEBUG() << "OpenIGTLinkTrackingSystemService::getTool: " << devicename;
	OpenIGTLinkToolPtr retval;
	std::map<QString, OpenIGTLinkToolPtr>::iterator it = mTools.find(devicename);
	if(it == mTools.end())
	{
//		CX_LOG_DEBUG() << "OpenIGTLinkTrackingSystemService::getTool. Create new tool: " << devicename;
		retval = OpenIGTLinkToolPtr(new OpenIGTLinkTool(devicename, equipmentType));
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
