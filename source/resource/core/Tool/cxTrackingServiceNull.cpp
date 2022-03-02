/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxTrackingServiceNull.h"

#include "cxTrackerConfiguration.h"


namespace cx
{
TrackingServiceNull::TrackingServiceNull()
{
}
bool TrackingServiceNull::isNull()
{
	return true;
}

void TrackingServiceNull::printWarning() const
{
//	reportWarning("Trying to use TrackingServiceNull. Is TrackingService (org.custusx.core.tracking) disabled?");
}

Tool::State TrackingServiceNull::getState() const
{
	return Tool::tsNONE;
}

void TrackingServiceNull::setState(const Tool::State val)
{

}

ToolPtr TrackingServiceNull::getTool(const QString& uid)
{
	this->printWarning();
	return ToolPtr();
}

ToolPtr TrackingServiceNull::getActiveTool()
{
	this->printWarning();
	return ToolPtr();
}

void TrackingServiceNull::setActiveTool(const QString& uid)
{
	this->printWarning();
}

void TrackingServiceNull::clearActiveTool()
{
	this->printWarning();
}

ToolPtr TrackingServiceNull::getFirstProbe()
{
	this->printWarning();
	return ToolPtr();
}

ToolPtr TrackingServiceNull::getReferenceTool() const
{
	this->printWarning();
	return ToolPtr();
}

ToolPtr TrackingServiceNull::getManualTool()
{
	this->printWarning();
	return ToolPtr();
}

TrackingService::ToolMap TrackingServiceNull::getTools()
{
	return ToolMap();
}

bool TrackingServiceNull::isPlaybackMode() const
{
	return false;
}

void TrackingServiceNull::setPlaybackMode(PlaybackTimePtr controller)
{

}

SessionToolHistoryMap TrackingServiceNull::getSessionHistory(double startTime, double stopTime)
{
	return SessionToolHistoryMap();
}

void TrackingServiceNull::runDummyTool(DummyToolPtr tool)
{
}

std::vector<TrackerConfigurationPtr> TrackingServiceNull::getConfigurations()
{
	std::vector<TrackerConfigurationPtr> retval;
	retval.push_back(TrackerConfiguration::getNullObject());
	return retval;
}

/*TrackerConfigurationPtr TrackingServiceNull::getConfiguration(QString trackingSystemImplementation)
{
	return TrackerConfiguration::getNullObject();
}*/

TrackerConfigurationPtr TrackingServiceNull::getConfiguration()
{
	return TrackerConfiguration::getNullObject();
}

void TrackingServiceNull::setCurrentTrackingSystemImplementation(QString trackingSystemImplementation)
{
}

QString TrackingServiceNull::getCurrentTrackingSystemImplementation()
{
	return QString();
}

void TrackingServiceNull::installTrackingSystem(TrackingSystemServicePtr system)
{

}

void TrackingServiceNull::unInstallTrackingSystem(TrackingSystemServicePtr system)
{

}

std::vector<TrackingSystemServicePtr> TrackingServiceNull::getTrackingSystems()
{
	return std::vector<TrackingSystemServicePtr> ();
}

void TrackingServiceNull::resetTimeSynchronization()
{

}

} //cx
