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

#ifndef CXTOOLMANAGERUSINGIGSTK_H_
#define CXTOOLMANAGERUSINGIGSTK_H_

#include "cxTrackingServiceExport.h"

#include "cxToolManager.h"
#include "vtkForwardDeclarations.h"

class QDomNode;
class QDomDocument;
class QDomNodeList;
class QTimer;
class QFileInfo;

namespace cx
{
/**
 * \file
 * \addtogroup cx_service_tracking
 * @{
 */

typedef boost::shared_ptr<class ManualToolAdapter> ManualToolAdapterPtr;
typedef boost::shared_ptr<class IgstkTrackerThread> IgstkTrackerThreadPtr;
typedef boost::shared_ptr<class PlaybackTime> PlaybackTimePtr;

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
class cxTrackingService_EXPORT ToolManagerUsingIGSTK: public ToolManager
{
Q_OBJECT

public:
	typedef boost::shared_ptr<ToolManagerUsingIGSTK> ToolManagerUsingIGSTKPtr;

	static ToolManagerUsingIGSTKPtr create();
	virtual ~ToolManagerUsingIGSTK();

//	static void initializeObject();
//	static cxTrackingServiceOldPtr getInstance();

	virtual QStringList getSupportedTrackingSystems();

	virtual Tool::State getState() const;
	virtual void setState(const Tool::State val);

//	virtual bool isConfigured() const; ///< checks if the system is configured
//	virtual bool isInitialized() const; ///< checks if the hardware is initialized
//	virtual bool isTracking() const; ///< checks if the system is tracking
	virtual bool isPlaybackMode() const { return mPlayBackMode; }

	virtual ToolManager::ToolMap getTools(); ///< get all configured and initialized tools
	virtual ToolPtr getTool(const QString& uid); ///< get a specific tool
	virtual void setTooltipOffset(double offset); ///< set the tool tip offset
	virtual double getTooltipOffset() const; ///< get the tool tip offset

	virtual ToolPtr getDominantTool(); ///< get the dominant tool
	virtual void setDominantTool(const QString& uid); ///< can be set to either a connected or configured tool

	virtual ToolPtr getReferenceTool() const; ///< get the tool that is used as a reference, if any

	virtual void savePositionHistory();
	virtual void loadPositionHistory();

	virtual void setLoggingFolder(QString loggingFolder); ///<\param loggingFolder path to the folder where logs should be saved

	void addXml(QDomNode& parentNode); ///< write internal state to node
	void parseXml(QDomNode& dataNode); ///< read internal state from node
	virtual void clear(); ///< clear everything loaded from xml

	virtual ManualToolPtr getManualTool(); ///< a mouse-controllable virtual tool that is available even when not tracking.

	virtual SessionToolHistoryMap getSessionHistory(double startTime, double stopTime);

	virtual void runDummyTool(DummyToolPtr tool);
	virtual ToolPtr findFirstProbe();

	virtual void setPlaybackMode(PlaybackTimePtr controller);
	virtual TrackerConfigurationPtr getConfiguration();

signals:
	void probeAvailable(); ///< Emitted when a probe is configured

	// internal use only
	void configured(); ///< system is configured
	void deconfigured(); ///<
	void initialized(); ///< system is initialized
	void uninitialized(); ///< system is uninitialized
	void trackingStarted(); ///< system starts tracking
	void trackingStopped(); ///< system stops tracking

public slots:
//	void configure(); ///< sets up the software like the xml file suggests
//	virtual void deconfigure(); ///< deconfigures the software
//	void initialize(); ///< connects to the hardware
//	void uninitialize(); ///< disconnects from the hardware
//	void startTracking(); ///< starts tracking
//	void stopTracking(); ///< stops tracking
	virtual void saveToolsSlot(); ///< saves transforms and timestamps
	virtual void dominantCheckSlot(); ///< checks if the visible tool is going to be set as dominant tool

private slots:
	void configure(); ///< sets up the software like the xml file suggests
	virtual void deconfigure(); ///< deconfigures the software
	void initialize(); ///< connects to the hardware
	void uninitialize(); ///< disconnects from the hardware
	void startTracking(); ///< starts tracking
	void stopTracking(); ///< stops tracking

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
	ToolManagerUsingIGSTK();
	TrackingServiceWeakPtr mSelf;

	bool isConfigured() const; ///< checks if the system is configured
	bool isInitialized() const; ///< checks if the hardware is initialized
	bool isTracking() const; ///< checks if the system is tracking
	void closePlayBackMode();
	void initializeManualTool();
	void setConfigurationFile(QString configurationFile); ///< Sets the configuration file to use, must be located in the resourcefolder \param configurationFile path to the configuration file to use
	void resetTrackingPositionFilters();

	QString mConfigurationFilePath; ///< path to the configuration file
	QString mLoggingFolder; ///< path to where logging should be saved

	ToolManager::ToolMap mTools; ///< all tools

	ToolPtr mDominantTool; ///< the tool with highest priority
	ToolPtr mReferenceTool; ///< the tool which is used as patient reference tool
	ManualToolAdapterPtr mManualTool; ///< a mouse-controllable virtual tool that is available even when not tracking.

	Tool::State mState;
//	bool mConfigured; ///< whether or not the system is configured
//	bool mInitialized; ///< whether or not the system is initialized
//	bool mTracking; ///< whether or not the system is tracking
	bool mPlayBackMode; ///< special mode: all tools are displaying historic positions.

	double mLastLoadPositionHistory;

	IgstkTrackerThreadPtr mTrackerThread;

	double mToolTipOffset; ///< Common tool tip offset for all tools

private:
	ToolManagerUsingIGSTK(ToolManagerUsingIGSTK const&);
	ToolManagerUsingIGSTK& operator=(ToolManagerUsingIGSTK const&);

#ifndef WIN32
	bool createSymlink();
	QFileInfo getSymlink() const;
	void cleanupSymlink();
#endif //WIN32
};

bool toolTypeSort(const ToolPtr tool1, const ToolPtr tool2); ///< function for sorting tools by type

/**
 * @}
 */
} //namespace cx
#endif /* CXTOOLMANAGERUSINGIGSTK_H_ */
