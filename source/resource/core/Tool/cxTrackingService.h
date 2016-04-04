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

#ifndef CXTRACKINGSERVICE_H
#define CXTRACKINGSERVICE_H

#include "cxResourceExport.h"

#include <QObject>
#include <boost/shared_ptr.hpp>
#include "cxTransform3D.h"
#include "cxTool.h"

#define TrackingService_iid "cx::TrackingService"

class QDomNode;

namespace cx
{

typedef std::map<double, Transform3D> TimedTransformMap;
typedef boost::shared_ptr<class Tool> ToolPtr;
typedef std::map<ToolPtr, TimedTransformMap> SessionToolHistoryMap;
typedef boost::shared_ptr<class Landmarks> LandmarksPtr;
typedef boost::shared_ptr<class PlaybackTime> PlaybackTimePtr;
typedef boost::shared_ptr<class TrackerConfiguration> TrackerConfigurationPtr;

typedef boost::shared_ptr<class TrackingSystemService> TrackingSystemServicePtr;
typedef boost::shared_ptr<class TrackingService> TrackingServicePtr;
typedef boost::shared_ptr<class DummyTool> DummyToolPtr;
typedef boost::shared_ptr<class ManualTool> ManualToolPtr;

/**
 * \brief Service providing tools and tracking systems.
 *
 * Links the navigation system to the physical world.
 * Devices with a physical position are realized by tracking hardware.
 *
 *  \ingroup cx_resource_core_tool
 */
class cxResource_EXPORT TrackingService : public QObject
{
	Q_OBJECT

public:
	typedef std::map<QString, ToolPtr> ToolMap;

	virtual ~TrackingService() {}

	virtual Tool::State getState() const = 0;
	virtual void setState(const Tool::State val) = 0;

	virtual ToolPtr getTool(const QString& uid) = 0; ///< get a tool
	virtual ToolPtr getActiveTool() = 0; ///< get the tool that has higest priority when tracking
	virtual void setActiveTool(const QString& uid) = 0; ///< set a tool to be the active tool
	virtual ToolPtr getFirstProbe() = 0; ///< get the active probe or any if none active

	virtual ToolPtr getReferenceTool() const = 0; ///< tool used as patient reference
	virtual ToolPtr getManualTool() = 0; ///< a mouse-controllable virtual tool that is available even when not tracking.

	virtual ToolMap getTools() = 0; ///< get configured and initialized tools

	virtual bool isPlaybackMode() const = 0;
	virtual void setPlaybackMode(PlaybackTimePtr controller) = 0;

	virtual SessionToolHistoryMap getSessionHistory(double startTime, double stopTime) = 0;
	virtual void runDummyTool(DummyToolPtr tool) = 0;
	virtual TrackerConfigurationPtr getConfiguration() = 0;

	virtual void installTrackingSystem(TrackingSystemServicePtr system) = 0;
	virtual void unInstallTrackingSystem(TrackingSystemServicePtr system) = 0;
	virtual std::vector<TrackingSystemServicePtr> getTrackingSystems() = 0;

	virtual bool isNull() = 0;
	static TrackingServicePtr getNullObject();

signals:
	void stateChanged();
	void activeToolChanged(const QString& uId);

};

} //cx
Q_DECLARE_INTERFACE(cx::TrackingService, TrackingService_iid)

#endif // CXTRACKINGSERVICE_H
