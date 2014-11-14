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

#ifndef CXTRACKINGIMPLSERVICE_H_
#define CXTRACKINGIMPLSERVICE_H_

#include "org_custusx_core_tracking_Export.h"
#include "cxTrackingService.h"
#include "vtkForwardDeclarations.h"

class QDomNode;
class QDomDocument;
class QDomNodeList;
class QTimer;
class QFileInfo;

class ctkPluginContext;

namespace cx
{
/**
 * \file
 * \addtogroup cx_service_tracking
 * @{
 */

typedef boost::shared_ptr<class TrackingImplService> TrackingImplServicePtr;

typedef boost::shared_ptr<class ManualToolAdapter> ManualToolAdapterPtr;
typedef boost::shared_ptr<class IgstkTrackerThread> IgstkTrackerThreadPtr;
typedef boost::shared_ptr<class PlaybackTime> PlaybackTimePtr;

typedef boost::shared_ptr<class TrackingSystemService> TrackingSystemServicePtr;
typedef boost::shared_ptr<class TrackingSystemPlaybackService> TrackingSystemPlaybackServicePtr;

/**
 * \brief Interface towards the navigation system.
 * \ingroup cx_service_tracking
 *
 * \image html tracking_simple.png "Tracking Service ideal design."
 *
 * Links the navigation to the physical world. Devices with a physical position
 * are realized by tracking hardware.
 *
 * The tracking module is an implementation of the Tool module.
 * ToolManager and Tool are implemented using IGSTK, and communicates with
 * the Polaris and Aurora tracking systems. All interaction with the module
 * should go through the SSC interfaces. The ToolManager should be
 * considered equal to the \ref cx_service_tracking.
 *
 * \image html cxArchitecture_tracking.png "Tracking Service Implementation."
 *
 * Examples of Polaris tracking devices:
 *
 * \image html us_probe.png "Ultrasound Probe with Polaris tracking sensors attached."
 * \image html PolarisTool.jpg "Polaris Navigation Pointer from NDI"
 *
 *
 * \date Nov 6, 2008
 * \date Jan 19, 2012
 *
 * \author Janne Beate Bakeng, SINTEF
 * \author Christian Askeland, SINTEF
 */
class org_custusx_core_tracking_EXPORT TrackingImplService: public TrackingService
{
Q_OBJECT
Q_INTERFACES(cx::TrackingService)

public:
	TrackingImplService(ctkPluginContext* context);
	virtual ~TrackingImplService();

	virtual Tool::State getState() const;
	virtual void setState(const Tool::State val);

	virtual bool isPlaybackMode() const;

	virtual ToolMap getTools(); ///< get all configured and initialized tools
	virtual ToolPtr getTool(const QString& uid); ///< get a specific tool

	virtual ToolPtr getActiveTool(); ///< get the dominant tool
	virtual void setActiveTool(const QString& uid); ///< can be set to either a connected or configured tool

	virtual ToolPtr getReferenceTool() const; ///< get the tool that is used as a reference, if any

	virtual void savePositionHistory();
	virtual void loadPositionHistory();

	virtual void setLoggingFolder(QString loggingFolder); ///<\param loggingFolder path to the folder where logs should be saved

	void addXml(QDomNode& parentNode); ///< write internal state to node
	void parseXml(QDomNode& dataNode); ///< read internal state from node
	virtual void clear(); ///< clear everything loaded from xml

	virtual ToolPtr getManualTool(); ///< a mouse-controllable virtual tool that is available even when not tracking.

	virtual SessionToolHistoryMap getSessionHistory(double startTime, double stopTime);

	virtual void runDummyTool(DummyToolPtr tool);
	virtual ToolPtr getFirstProbe();

	virtual void setPlaybackMode(PlaybackTimePtr controller);
	virtual TrackerConfigurationPtr getConfiguration();

	virtual void installTrackingSystem(TrackingSystemServicePtr system);
	virtual void unInstallTrackingSystem(TrackingSystemServicePtr system);
	virtual std::vector<TrackingSystemServicePtr> getTrackingSystems();

	bool isNull();

private slots:
	void globalConfigurationFileChangedSlot(QString key);
	void onSystemStateChanged();
	void dominantCheckSlot(); ///< checks if the visible tool is going to be set as dominant tool
	void onTooltipOffset(double val);

private:
//	ToolManagerUsingIGSTK();
	void rebuildCachedTools();
	void initializeManualTool();
	void setConfigurationFile(QString configurationFile); ///< Sets the configuration file to use, must be located in the resourcefolder \param configurationFile path to the configuration file to use
	void resetTrackingPositionFilters();
	void waitForState(Tool::State state, int timeout);
	void imbueManualToolWithRealProperties();
	void addToolsFrom(TrackingSystemServicePtr system);
	bool manualToolHasMostRecentTimestamp();
	std::vector<ToolPtr> getVisibleTools();

	QString mLoggingFolder; ///< path to where logging should be saved

	ToolMap mTools; ///< all tools

	ToolPtr mDominantTool; ///< the tool with highest priority
	ToolPtr mReferenceTool; ///< the tool which is used as patient reference tool
	ManualToolAdapterPtr mManualTool; ///< a mouse-controllable virtual tool that is available even when not tracking.

	double mLastLoadPositionHistory;

	std::vector<TrackingSystemServicePtr> mTrackingSystems;
	TrackingSystemPlaybackServicePtr mPlaybackSystem;
	ctkPluginContext *mContext;

	double mToolTipOffset; ///< Common tool tip offset for all tools

//private:
//	ToolManagerUsingIGSTK(ToolManagerUsingIGSTK const&);
//	ToolManagerUsingIGSTK& operator=(ToolManagerUsingIGSTK const&);
};

bool toolTypeSort(const ToolPtr tool1, const ToolPtr tool2); ///< function for sorting tools by type

/**
 * @}
 */
} //namespace cx


#endif /* CXTRACKINGIMPLSERVICE_H_ */

