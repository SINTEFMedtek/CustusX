/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTRACKINGSERVICEPROXY_H
#define CXTRACKINGSERVICEPROXY_H

#include "cxResourceExport.h"

#include "cxTrackingService.h"
#include "cxServiceTrackerListener.h"
class ctkPluginContext;

namespace cx
{

/**
 *  \ingroup cx_resource_core_tool
 */
class cxResource_EXPORT TrackingServiceProxy : public TrackingService
{
public:
	static TrackingServicePtr create(ctkPluginContext *pluginContext);
	TrackingServiceProxy(ctkPluginContext *pluginContext);

	virtual Tool::State getState() const;
	virtual void setState(const Tool::State val);

	virtual ToolPtr getTool(const QString& uid);
	virtual ToolPtr getActiveTool();
	virtual void setActiveTool(const QString& uid);
	virtual ToolPtr getFirstProbe();

	virtual ToolPtr getReferenceTool() const;
	virtual ToolPtr getManualTool();

	virtual ToolMap getTools(); ///< get configured and initialized tools

	virtual bool isPlaybackMode() const;
	virtual void setPlaybackMode(PlaybackTimePtr controller);

	virtual SessionToolHistoryMap getSessionHistory(double startTime, double stopTime);
	virtual void runDummyTool(DummyToolPtr tool);
	virtual TrackerConfigurationPtr getConfiguration();

	virtual void installTrackingSystem(TrackingSystemServicePtr system);
	virtual void unInstallTrackingSystem(TrackingSystemServicePtr system);
	virtual std::vector<TrackingSystemServicePtr> getTrackingSystems();

	bool isNull();

private:
	void initServiceListener();
	void onServiceAdded(TrackingService* service);
	void onServiceRemoved(TrackingService *service);

	ctkPluginContext *mPluginContext;
	TrackingServicePtr mTrackingService;
	boost::shared_ptr<ServiceTrackerListener<TrackingService> > mServiceListener;
};
} //cx
#endif // CXTRACKINGSERVICEPROXY_H
