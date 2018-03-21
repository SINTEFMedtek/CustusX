/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTRACKINGIMPLSERVICE_H_
#define CXTRACKINGIMPLSERVICE_H_

#include "org_custusx_core_tracking_Export.h"
#include "cxTrackingService.h"
#include "vtkForwardDeclarations.h"
#include "cxServiceTrackerListener.h"

class QDomNode;
class QDomElement;
class QDomDocument;
class QDomNodeList;
class QTimer;
class QFileInfo;

class ctkPluginContext;

namespace cx
{
typedef boost::shared_ptr<class TrackingImplService> TrackingImplServicePtr;

typedef boost::shared_ptr<class ManualToolAdapter> ManualToolAdapterPtr;
typedef boost::shared_ptr<class PlaybackTime> PlaybackTimePtr;

typedef boost::shared_ptr<class TrackingSystemService> TrackingSystemServicePtr;
typedef boost::shared_ptr<class TrackingSystemPlaybackService> TrackingSystemPlaybackServicePtr;
typedef boost::shared_ptr<class SessionStorageService> SessionStorageServicePtr;

/**
 * \brief Interface towards the navigation system.
 * \ingroup org_custusx_core_tracking
 *
 * \image html tracking_simple.png "Tracking Service ideal design."
 *
 * Links the navigation to the physical world. Devices with a physical position
 * are realized by tracking hardware.
 *
 * The tracking module is an implementation of the Tool module.
 * ToolManager and Tool are implemented using IGSTK, and communicates with
 * the Polaris and Aurora tracking systems.
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

	virtual ToolPtr getActiveTool();
	virtual void setActiveTool(const QString& uid); ///< can be set to either a connected or configured tool

	virtual ToolPtr getReferenceTool() const; ///< get the tool that is used as a reference, if any

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
	void activeCheckSlot(); ///< checks if the visible tool is going to be set as active tool
	void onTooltipOffset(double val);

	void onSessionChanged();
	void onSessionCleared();
	void onSessionLoad(QDomElement& node);
	void onSessionSave(QDomElement& node);

private:
    void listenForTrackingSystemServices(ctkPluginContext *context);
    void onTrackingSystemAdded(TrackingSystemService* service);
    void onTrackingSystemRemoved(TrackingSystemService* service);
    void onTrackingSystemModified(TrackingSystemService* service);


	void rebuildCachedTools();
	void initializeManualTool();
	void setConfigurationFile(QString configurationFile); ///< Sets the configuration file to use, must be located in the resourcefolder \param configurationFile path to the configuration file to use
	void resetTrackingPositionFilters();
	void waitForState(Tool::State state, int timeout);
	void imbueManualToolWithRealProperties();
	void addToolsFrom(TrackingSystemServicePtr system);
	bool manualToolHasMostRecentTimestamp();
	std::vector<ToolPtr> getVisibleTools();

	void addXml(QDomNode& parentNode); ///< write internal state to node
	void parseXml(QDomNode& dataNode); ///< read internal state from node
	virtual void savePositionHistory();
	virtual void loadPositionHistory();

	QString getLoggingFolder();

	ToolMap mTools; ///< all tools
	ToolPtr mActiveTool; ///< the tool with highest priority
	ToolPtr mReferenceTool; ///< the tool which is used as patient reference tool
	ManualToolAdapterPtr mManualTool; ///< a mouse-controllable virtual tool that is available even when not tracking.

	double mLastLoadPositionHistory;

	std::vector<TrackingSystemServicePtr> mTrackingSystems;
	TrackingSystemPlaybackServicePtr mPlaybackSystem;
	ctkPluginContext *mContext;
	SessionStorageServicePtr mSession;

	double mToolTipOffset; ///< Common tool tip offset for all tools

    boost::shared_ptr<ServiceTrackerListener<TrackingSystemService> > mServiceListener;
};

bool toolTypeSort(const ToolPtr tool1, const ToolPtr tool2); ///< function for sorting tools by type

} //namespace cx


#endif /* CXTRACKINGIMPLSERVICE_H_ */

