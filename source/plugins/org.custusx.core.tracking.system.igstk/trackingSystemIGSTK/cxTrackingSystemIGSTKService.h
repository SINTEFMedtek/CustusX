/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTRACKINGSYSTEMIGSTKSERVICE_H
#define CXTRACKINGSYSTEMIGSTKSERVICE_H

#include "org_custusx_core_tracking_system_igstk_Export.h"

#include "cxTrackingSystemService.h"
#include "vtkForwardDeclarations.h"

class QDomNode;
class QDomDocument;
class QDomNodeList;
class QFileInfo;

namespace cx
{
/**
 * \file
 * \addtogroup org_custusx_core_tracking_igstk
 * @{
 */

typedef boost::shared_ptr<class ManualToolAdapter> ManualToolAdapterPtr;
typedef boost::shared_ptr<class IgstkTrackerThread> IgstkTrackerThreadPtr;
typedef boost::shared_ptr<class PlaybackTime> PlaybackTimePtr;

/**
 * \brief Interface towards one tracking system.
 * \ingroup org_custusx_core_tracking
 *
 * \image html tracking_simple.png "Tracking Service ideal design."
 *
 * Links the navigation to the physical world. Devices with a physical position
 * are realized by tracking hardware.
 *
 * ToolManager and Tool are implemented using IGSTK, and communicates with
 * the Polaris and Aurora tracking systems.
 *
 * Examples of Polaris tracking devices:
 *
 * \image html us_probe.png "Ultrasound Probe with Polaris tracking sensors attached."
 * \image html PolarisTool.jpg "Polaris Navigation Pointer from NDI"
 *
 * \date Nov 6, 2008
 * \date Jan 19, 2012
 * \date 2014-11-01
 *
 * \author Janne Beate Bakeng, SINTEF
 * \author Christian Askeland, SINTEF
 */
class org_custusx_core_tracking_system_igstk_EXPORT TrackingSystemIGSTKService : public TrackingSystemService
{
Q_OBJECT

public:
	TrackingSystemIGSTKService();
	virtual ~TrackingSystemIGSTKService();

	virtual QString getUid() const { return "org.custusx.core.tracking.system.igstk"; }
	virtual std::vector<ToolPtr> getTools();
	virtual ToolPtr getReference() { return mReference; }

	virtual void setState(const Tool::State val);

	virtual void setLoggingFolder(QString loggingFolder); ///<\param loggingFolder path to the folder where logs should be saved

	virtual TrackerConfigurationPtr getConfiguration();

signals:

	// internal use only
	void configured(); ///< system is configured
	void deconfigured(); ///<
	void initialized(); ///< system is initialized
	void uninitialized(); ///< system is uninitialized
	void trackingStarted(); ///< system starts tracking
	void trackingStopped(); ///< system stops tracking

private slots:
	virtual void configure(); ///< sets up the software like the xml file suggests
	virtual void deconfigure(); ///< deconfigures the software
	virtual void initialize(); ///< connects to the hardware
	virtual void uninitialize(); ///< disconnects from the hardware
	virtual void startTracking(); ///< starts tracking
	virtual void stopTracking(); ///< stops tracking

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
	void setConfigurationFile(QString configurationFile); ///< Sets the configuration file to use, must be located in the resourcefolder \param configurationFile path to the configuration file to use
	void destroyTrackerThread();

	std::vector<ToolPtr> mTools; ///< all tools
	ToolPtr mReference; ///< the one tool selected as reference by IGSTK - defines pr space.

	IgstkTrackerThreadPtr mTrackerThread;

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

} // namespace cx

#endif // CXTRACKINGSYSTEMIGSTKSERVICE_H
