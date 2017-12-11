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
//	virtual QStringList getSupportedTrackingSystems();

	virtual Tool::State getState() const;
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
