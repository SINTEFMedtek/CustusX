// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXTOOLMANAGER_H_
#define CXTOOLMANAGER_H_

#include "vtkSmartPointer.h"

//#include "sscDummyTool.h"
#include "sscToolManager.h"
#include "sscDefinitions.h"
#include "sscManualTool.h"
#include "sscLandmark.h"

#include "cxTool.h"
#include "vtkForwardDeclarations.h"
#include "cxManualToolAdapter.h"

class QDomNode;
class QDomDocument;
class QDomNodeList;
class QTimer;
class QFileInfo;

namespace cx
{
/**
 * \file
 * \addtogroup cxServiceTracking
 * @{
 */

typedef boost::shared_ptr<class IgstkTrackerThread> IgstkTrackerThreadPtr;
typedef boost::shared_ptr<class PlaybackTime> PlaybackTimePtr;

/**
 * \brief Interface towards the navigation system.
 * \ingroup cxServiceTracking
 *
 * \image html tracking_simple.png "Tracking Service ideal design."
 *
 * Links the navigation to the physical world. Devices with a physical position
 * are realized by tracking hardware.
 *
 * The tracking module is an implementation of the ssc::Tool module.
 * ToolManager and Tool are implemented using IGSTK, and communicates with
 * the Polaris and Aurora tracking systems. All interaction with the module
 * should go through the SSC interfaces. The ssc::ToolManager should be
 * considered equal to the \ref cxServiceTracking.
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
class ToolManager: public ssc::ToolManager
{
Q_OBJECT

public:
	static void initializeObject();
	static ToolManager* getInstance();

	QStringList getSupportedTrackingSystems();

	virtual bool isConfigured() const; ///< checks if the system is configured
	virtual bool isInitialized() const; ///< checks if the hardware is initialized
	virtual bool isTracking() const; ///< checks if the system is tracking
	bool         isPlaybackMode() const { return mPlayBackMode; }

	virtual ssc::ToolManager::ToolMapPtr getConfiguredTools(); ///< get all configured, but not initialized tools
	virtual ssc::ToolManager::ToolMapPtr getInitializedTools(); ///< get all initialized tools
	virtual ssc::ToolManager::ToolMapPtr getTools(); ///< get all configured and initialized tools
	virtual ssc::ToolPtr getTool(const QString& uid); ///< get a specific tool
	virtual void setTooltipOffset(double offset); ///< set the tool tip offset
	virtual double getTooltipOffset() const; ///< get the tool tip offset

	virtual ssc::ToolPtr getDominantTool(); ///< get the dominant tool
	virtual void setDominantTool(const QString& uid); ///< can be set to either a connected or configured tool

	void setClinicalApplication(ssc::CLINICAL_APPLICATION application);

	virtual std::map<QString, QString> getToolUidsAndNames() const; ///< both from configured and connected tools
	virtual std::vector<QString> getToolNames() const; ///< both from configured and connected tools
	virtual std::vector<QString> getToolUids() const; ///< both from configured and connected tools

	virtual ssc::Transform3DPtr get_rMpr() const; ///< get the patient registration transform
	virtual void set_rMpr(const ssc::Transform3DPtr& val); ///<  set the transform from patient to reference space

	virtual ssc::RegistrationHistoryPtr get_rMpr_History();

	virtual ssc::ToolPtr getReferenceTool() const; ///< get the tool that is used as a reference, if any

	virtual void savePositionHistory();
	virtual void loadPositionHistory();

	void setLoggingFolder(QString loggingFolder); ///<\param loggingFolder path to the folder where logs should be saved

	void addXml(QDomNode& parentNode); ///< write internal state to node
	void parseXml(QDomNode& dataNode); ///< read internal state from node
	virtual void clear(); ///< clear everything loaded from xml

	ssc::ManualToolPtr getManualTool(); ///< a mouse-controllable virtual tool that is available even when not tracking.
	virtual ssc::LandmarkMap getLandmarks();
	virtual void setLandmark(ssc::Landmark landmark);
	virtual void removeLandmark(QString uid);
	virtual void removeLandmarks();

	virtual ssc::SessionToolHistoryMap getSessionHistory(double startTime, double stopTime);

	void runDummyTool(ssc::DummyToolPtr tool);
	ssc::ToolPtr findFirstProbe();

	void setPlaybackMode(PlaybackTimePtr controller);

public slots:
	void configure(); ///< sets up the software like the xml file suggests
	void deconfigure(); ///< deconfigures the software
	void initialize(); ///< connects to the hardware
	void uninitialize(); ///< disconnects from the hardware
	void startTracking(); ///< starts tracking
	void stopTracking(); ///< stops tracking
	void saveToolsSlot(); ///< saves transforms and timestamps
	void dominantCheckSlot(); ///< checks if the visible tool is going to be set as dominant tool

private slots:
	void trackerConfiguredSlot(bool on);
	void initializedSlot(bool);
	void trackerTrackingSlot(bool);

	void startTrackingAfterInitSlot();
	void initializeAfterConfigSlot();
	void uninitializeAfterTrackingStoppedSlot();
	void deconfigureAfterUninitializedSlot();
	void configureAfterDeconfigureSlot();
	void globalConfigurationFileChangedSlot(QString key);

private:
	ToolManager(); ///< use getInstance instead
	virtual ~ToolManager(); ///< destructor

	void closePlayBackMode();
	void initializeManualTool();
	void setConfigurationFile(QString configurationFile); ///< Sets the configuration file to use, must be located in the resourcefolder \param configurationFile path to the configuration file to use

	QString mConfigurationFilePath; ///< path to the configuration file
	QString mLoggingFolder; ///< path to where logging should be saved

	ssc::CLINICAL_APPLICATION mApplication; ///< Current clinical application
	ssc::ToolManager::ToolMap mTools; ///< all tools

	ssc::ToolPtr mDominantTool; ///< the tool with highest priority
	ssc::ToolPtr mReferenceTool; ///< the tool which is used as patient reference tool
	ManualToolAdapterPtr mManualTool; ///< a mouse-controllable virtual tool that is available even when not tracking.

	ssc::RegistrationHistoryPtr m_rMpr_History; ///< transform from the patient reference to the reference, along with historical data.

	bool mConfigured; ///< whether or not the system is configured
	bool mInitialized; ///< whether or not the system is initialized
	bool mTracking; ///< whether or not the system is tracking
	bool mPlayBackMode; ///< special mode: all tools are displaying historic positions.

	ssc::LandmarkMap mLandmarks; ///< in space patient reference.
	double mLastLoadPositionHistory;

	IgstkTrackerThreadPtr mTrackerThread;

	double mToolTipOffset; ///< Common tool tip offset for all tools

private:
	ToolManager(ToolManager const&);
	ToolManager& operator=(ToolManager const&);

#ifndef WIN32
	bool createSymlink();
	QFileInfo getSymlink() const;
	void cleanupSymlink();
#endif //WIN32
};

bool toolTypeSort(const ssc::ToolPtr tool1, const ssc::ToolPtr tool2); ///< function for sorting tools by type

/**
 * @}
 */
} //namespace cx
#endif /* CXTOOLMANAGER_H_ */
