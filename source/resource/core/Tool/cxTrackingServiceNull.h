/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTRACKINGSERVICENULL_H
#define CXTRACKINGSERVICENULL_H

#include "cxResourceExport.h"

#include "cxTrackingService.h"

namespace cx
{

/**
 *
 *  \ingroup cx_resource_core_tool
 */
class cxResource_EXPORT TrackingServiceNull : public TrackingService
{
public:
	TrackingServiceNull();

	virtual Tool::State getState() const;
	virtual void setState(const Tool::State val);

	virtual ToolPtr getTool(const QString& uid);
	virtual ToolPtr getActiveTool();
	virtual void setActiveTool(const QString& uid);
	virtual void clearActiveTool();
	virtual ToolPtr getFirstProbe();

	virtual ToolPtr getReferenceTool() const;
	virtual ToolPtr getManualTool();

	virtual ToolMap getTools(); ///< get configured and initialized tools

	virtual bool isPlaybackMode() const;
	virtual void setPlaybackMode(PlaybackTimePtr controller);

	virtual SessionToolHistoryMap getSessionHistory(double startTime, double stopTime);
	virtual void runDummyTool(DummyToolPtr tool);
	virtual std::vector<TrackerConfigurationPtr> getConfigurations();
	//virtual TrackerConfigurationPtr getConfiguration(QString trackingSystemImplementation);
	virtual TrackerConfigurationPtr getConfiguration();
	virtual void setCurrentTrackingSystemImplementation(QString trackingSystemImplementation);
	virtual QString getCurrentTrackingSystemImplementation();

	virtual void installTrackingSystem(TrackingSystemServicePtr system);
	virtual void unInstallTrackingSystem(TrackingSystemServicePtr system);
	virtual std::vector<TrackingSystemServicePtr> getTrackingSystems();
	virtual void resetTimeSynchronization();

	virtual bool isNull();
private:
	void printWarning() const;
};
} //cx
#endif // CXTRACKINGSERVICENULL_H
