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
#include "cxProfile.h"
#include "cxToolConfigurationParser.h"
#include "cxTrackerConfigurationImpl.h"
#include "cxNetworkHandler.h"
#include "cxOpenIGTLinkStreamerService.h"
#include "cxSettings.h"
#include "cxXmlOptionItem.h"

namespace cx
{

std::vector<ToolPtr> toVector(std::map<QString, OpenIGTLinkToolPtr> map, ToolPtr externalTool)
{
	std::vector<ToolPtr> retval;
	std::map<QString, OpenIGTLinkToolPtr>::iterator it = map.begin();
	for(; it!= map.end(); ++it)
	{
		retval.push_back(it->second);
	}
	if(externalTool)
		retval.push_back(externalTool);
	return retval;
}

OpenIGTLinkTrackingSystemService::OpenIGTLinkTrackingSystemService(NetworkHandlerPtr networkHandler, TrackingServicePtr trackingService) :
	mNetworkHandler(networkHandler),
	mTrackingService(trackingService),
	mLastTrackingState(Tool::tsNONE),
	mConfigured(false),
	mRequestedState(Tool::tsNONE),
	mConnectionAcquired(false),
	mFallback(false)
{
	if(mNetworkHandler == NULL)
		return;

	connect(mNetworkHandler.get(), &NetworkHandler::connected,this, &OpenIGTLinkTrackingSystemService::serverIsConnected);
	connect(mNetworkHandler.get(), &NetworkHandler::disconnected, this, &OpenIGTLinkTrackingSystemService::serverIsDisconnected);
	connect(mNetworkHandler.get(), &NetworkHandler::transform, this, &OpenIGTLinkTrackingSystemService::receiveTransform);
	//connect(mNetworkHandler.get(), &NetworkHandler::calibration, this, &OpenIGTLinkTrackingSystemService::receiveCalibration);
	connect(mNetworkHandler.get(), &NetworkHandler::probedefinition, this, &OpenIGTLinkTrackingSystemService::receiveProbedefinition);

	connect(this, &OpenIGTLinkTrackingSystemService::setInternalState, this, &OpenIGTLinkTrackingSystemService::internalSetState);

	if (mTrackingService)
		connect(mTrackingService.get(), &TrackingService::stateChanged,
				this, &OpenIGTLinkTrackingSystemService::applyPendingProbeDefinitions,
				Qt::QueuedConnection);

	connect(settings(), &Settings::valueChangedFor, this, &OpenIGTLinkTrackingSystemService::globalConfigurationFileChangedSlot);
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
	// A new start ends the fallback: the other system tries its device again, and asks again if it is still in use
	if (val == Tool::tsNONE || (mFallback && val >= Tool::tsTRACKING))
	{
		this->deconfigure();
	}
	else
	{
		if (!mConfigured)
		{
			this->configure();
		}
		if (mConfigured)
		{
			mRequestedState = val;
			if (val >= Tool::tsTRACKING)
			{
				this->connectToServer();
			}
			else
			{
				this->disconnectFromServer();
			}
		}
	}
	this->updateState();
}

void OpenIGTLinkTrackingSystemService::updateState()
{
	Tool::State state = Tool::tsNONE;
	if (mConfigured)
	{
		bool connected = this->isServerConnected();
		state = (connected && mRequestedState >= Tool::tsINITIALIZED) ? mRequestedState : Tool::tsCONFIGURED;
	}
	if (state != mState)
	{
		mState = state;
		emit stateChanged();
	}
}

bool OpenIGTLinkTrackingSystemService::isServerConnected() const
{
	// Without a network handler (in tests), act as if connected
	return !mNetworkHandler || mNetworkHandler->isConnected();
}

void OpenIGTLinkTrackingSystemService::connectToServer()
{
	if (mNetworkHandler)
	{
		// Also when already acquired: connects again if the last attempt failed
		QDomElement options = profile()->getXmlSettings().descend("video").getElement("video");
		QString host = XmlOptionItem(OPENIGTLINK3_STREAMER_IP, options).readNonEmptyValue("127.0.0.1");
		int port = XmlOptionItem(OPENIGTLINK3_STREAMER_PORT, options).readNonEmptyValue("18944").toInt();
		mConnectionAcquired = true;
		mNetworkHandler->acquireConnection("tracking", host, port);
	}
}

void OpenIGTLinkTrackingSystemService::disconnectFromServer()
{
	if (mNetworkHandler && mConnectionAcquired)
	{
		mConnectionAcquired = false;
		mNetworkHandler->releaseConnection("tracking");
	}
}

void OpenIGTLinkTrackingSystemService::startFallbackTracking(QString configurationFile)
{
	if (mNetworkHandler)
	{
		this->deconfigure();
		mFallback = true;
		mFallbackConfigurationFile = configurationFile;
		this->configure();
		if (mConfigured)
		{
			report("OpenIGTLink: Tracking the tools of " + configurationFile + " from the OpenIGTLink server, since their device is used by another application.");
			mRequestedState = Tool::tsTRACKING;
			this->connectToServer();
		}
		else
		{
			mFallback = false;
		}
		this->updateState();
	}
}

void OpenIGTLinkTrackingSystemService::globalConfigurationFileChangedSlot(QString key)
{
	if (key == "toolConfigFile")
	{
		this->setConfigurationFile(profile()->getToolConfigFilePath());
	}
}

std::vector<ToolPtr> OpenIGTLinkTrackingSystemService::getTools()
{
	return toVector(mTools, mExternalTool);
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
	ConfigurationFileParser configParser(mFallback ? mFallbackConfigurationFile : mConfigurationFilePath, mLoggingFolder);

	if(!mFallback && !configParser.getTrackingSystemImplementation().contains(TRACKING_SYSTEM_IMPLEMENTATION_IGTLINK, Qt::CaseInsensitive))
	{
		CX_LOG_DEBUG() << "OpenIGTLinkTrackingSystemService::configure(): Not using OpenIGTLink tracking.";
		return;
	}
	emit updateTrackingSystemImplementation(configParser.getTrackingSystemImplementation());

	CX_LOG_DEBUG() << "OpenIGTLinkTrackingSystemService::configure(): Using OpenIGTLink3 tracking";

	std::vector<ConfigurationFileParser::ToolStructure> toolList = configParser.getToolListWithMetaInformation();
	if (mFallback)
	{
		// The server replaces the device: positions in tracker space, as from the device itself
		bool applyReference = settings()->value("openIGTLinkFallbackApplyReference", true).toBool();
		for (ConfigurationFileParser::ToolStructure& tool : toolList)
		{
			tool.mApplyRefToTool = applyReference;
		}
	}

	//Create tools
	for(std::vector<ConfigurationFileParser::ToolStructure>::iterator it = toolList.begin(); it != toolList.end(); ++it)
	{
		ToolFileParser toolParser((*it).mAbsoluteToolFilePath, mLoggingFolder);
		ToolFileParser::ToolInternalStructurePtr internalTool = toolParser.getTool();
		if(!internalTool)
			continue; // not a valid tool file, reported by the parser

		QString devicename = internalTool->mUid;
		OpenIGTLinkToolPtr newTool = OpenIGTLinkToolPtr(new OpenIGTLinkTool((*it), internalTool));
		if(mTools.count(devicename))
			CX_LOG_WARNING() << "Tool configuration already contain tool: " << devicename << ". Existing tool will be overwritten";
		mTools[devicename] = newTool;
		if(newTool->isReference())
			mReference = newTool;
	}
	if(!mReference)
		CX_LOG_WARNING() << "OpenIGTLinkTrackingSystemService::configure() Got no reference tool";

	mConfigured = true;
	this->updateState();
}

void OpenIGTLinkTrackingSystemService::deconfigure()
{
	mTransformIdWarningPrinted.clear();
	this->disconnectFromServer();
	mRequestedState = Tool::tsNONE;
	mConfigured = false;
	mFallback = false;
	mTools.clear();
	mReference.reset();
	this->updateState();
}

void OpenIGTLinkTrackingSystemService::serverIsConnected()
{
	// A connection opened for video also starts tracking with an OpenIGTLink tool configuration
	if (!mConfigured)
	{
		this->configure();
	}
	if (mConfigured)
	{
		mRequestedState = Tool::tsTRACKING;
	}
	this->updateState();
}

void OpenIGTLinkTrackingSystemService::serverIsDisconnected()
{
	this->updateState();
}

void OpenIGTLinkTrackingSystemService::receiveTransform(QString devicename, Transform3D transform, double timestampMS)
{
//	CX_LOG_DEBUG() << "receiveTransform for: " << devicename;
	OpenIGTLinkToolPtr tool = this->getTool(devicename);
	if(!this->isTracking())
	{
		// Tracking is stopped, but the connection is still used by video
	}
	else if(tool)
	{
		bool waitingForReference = mReference && !tool->isReference() && mReference->getTimestamp() <= 0;
		if(tool->applyReference() && waitingForReference)
		{
			// No reference position yet: the tool position would be in tracker space
		}
		else if(tool->applyReference())
		{
			Transform3D tsMs = transform;
			Transform3D prMs = tsMs;//If no reference tool: ts==pr (ts = tracking system )
			if(mReference && !tool->isReference())
			{
				Transform3D tsMpr = mReference->get_prMt();
				prMs = tsMpr.inv() * tsMs;
			}
			//Apply ref sensor pos to all tool positions
			tool->toolTransformAndTimestampSlot(prMs, timestampMS);
		}
		else
			tool->toolTransformAndTimestampSlot(transform, timestampMS);
	}
	else
		this->printTransformIdWarning(devicename);
}

bool OpenIGTLinkTrackingSystemService::printTransformIdWarning(QString devicename)
{
	if(!mTransformIdWarningPrinted.contains(devicename))
	{
		CX_LOG_WARNING() << "No tool with openigtlinktransformid: " << devicename;
		CX_LOG_WARNING() << "Add the id to an openigtlinktransformid tag in one of the used tool files to fix this";
		mTransformIdWarningPrinted << devicename;
		return true;
	}
	return false;
}

void OpenIGTLinkTrackingSystemService::receiveCalibration(QString devicename, Transform3D calibration)
{
	CX_LOG_DEBUG() << "receiveCalibration for: " << devicename;
	OpenIGTLinkToolPtr tool = this->getTool(devicename);
	if(tool)
		tool->setCalibration_sMt(calibration);
}

void OpenIGTLinkTrackingSystemService::applyProbeDefinition(ToolPtr tool, ProbeDefinitionPtr definition)
{
	ProbePtr probe = tool->getProbe();
	if (probe)
	{
		ProbeDefinition old_def = probe->getProbeDefinition();
		definition->setUid(old_def.getUid());
		definition->applySoundSpeedCompensationFactor(old_def.getSoundSpeedCompensationFactor());
		probe->setProbeDefinition(*(definition.get()));
		emit stateChanged();
	}
}

//Used by probe tool. See also OpenIGTLinkStreamer::receiveProbedefinition() for RT view
void OpenIGTLinkTrackingSystemService::receiveProbedefinition(QString devicename, ProbeDefinitionPtr definition)
{
	OpenIGTLinkToolPtr ownTool = this->getTool(devicename);
	if (ownTool)
	{
		applyProbeDefinition(ownTool, definition);
	}
	else if (mTrackingService)
	{
		bool applied = false;
		std::map<QString, ToolPtr> allTools = mTrackingService->getTools();
		for (std::pair<const QString, ToolPtr>& kv : allTools)
		{
			if (kv.second->getOpenIGTLinkImageId().compare(devicename, Qt::CaseInsensitive) == 0)
			{
				applyProbeDefinition(kv.second, definition);
				applied = true;
				break;
			}
		}
		if (!applied)
			mPendingProbeDefinitions[devicename] = definition;
	}
}

void OpenIGTLinkTrackingSystemService::applyPendingProbeDefinitions()
{
	if (!mTrackingService)
		return;

	Tool::State currentState = mTrackingService->getState();
	bool trackingStarted = (currentState == Tool::tsTRACKING && mLastTrackingState != Tool::tsTRACKING);
	mLastTrackingState = currentState;

	if (trackingStarted && mNetworkHandler)
		mNetworkHandler->resendProbedefinition();

	if (mPendingProbeDefinitions.empty())
		return;

	std::map<QString, ToolPtr> allTools = mTrackingService->getTools();
	std::map<QString, ProbeDefinitionPtr>::iterator it = mPendingProbeDefinitions.begin();
	while (it != mPendingProbeDefinitions.end())
	{
		bool applied = false;
		for (std::pair<const QString, ToolPtr>& kv : allTools)
		{
			if (kv.second->getOpenIGTLinkImageId().compare(it->first, Qt::CaseInsensitive) == 0)
			{
				ProbeDefinitionPtr definition = it->second;
				ToolPtr tool = kv.second;
				it = mPendingProbeDefinitions.erase(it);
				applyProbeDefinition(tool, definition);
				applied = true;
				break;
			}
		}
		if (!applied)
			++it;
	}
}

OpenIGTLinkToolPtr OpenIGTLinkTrackingSystemService::getTool(QString devicename)
{
//	CX_LOG_DEBUG() << "OpenIGTLinkTrackingSystemService::getTool: " << devicename;

	std::map<QString, OpenIGTLinkToolPtr>::iterator it;
	for (it = mTools.begin(); it != mTools.end(); ++it)
	{
		OpenIGTLinkToolPtr tool = it->second;
		if (tool->doIdCorrespondToTool(devicename))
		{
			return tool;
		}
	}
	return OpenIGTLinkToolPtr();
}


void OpenIGTLinkTrackingSystemService::resetTimeSynchronization()
{
	mNetworkHandler->clearTimestampSynchronization();
}

} /* namespace cx */
