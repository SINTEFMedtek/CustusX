/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTRACKINGSYSTEMNDISERVICE_H_
#define CXTRACKINGSYSTEMNDISERVICE_H_

#include "org_custusx_core_tracking_system_ndi_Export.h"
#include <memory>

#include "cxTrackingSystemService.h"
#include "cxNdiDevice.h"

class QDomNode;
class QDomDocument;
class QDomNodeList;
class QFileInfo;

namespace cx
{
/**
 * \file
 * \addtogroup org_custusx_core_tracking_system_ndi
 * @{
 */

typedef std::shared_ptr<class NdiTrackerThread> NdiTrackerThreadPtr;

/**
 * \brief Tracking system service using the NDI CAPI (Combined API).
 * \ingroup org_custusx_core_tracking_system_ndi
 *
 * Supports NDI Polaris (optical) and Aurora (electromagnetic) trackers via
 * direct Ethernet (TCP) or serial (USB/COM) connection using the NDI Combined API.
 *
 * The device hostname/serial port can be configured via the CustusX settings key
 * "ndiTrackingHostname". If not set, the service will attempt to auto-detect a
 * serial device (e.g. /dev/ttyUSB0 on Linux).
 *
 * Tool definitions are loaded from the standard CustusX XML tool configuration files.
 * Each tool that requires a passive tool definition needs an SROM (.rom) file set
 * in the tool XML.
 */
class org_custusx_core_tracking_system_ndi_EXPORT TrackingSystemNDIService : public TrackingSystemService
{
	Q_OBJECT

public:
	/** NDI handles ndi configurations, and igstk configurations unless the IGSTK plugin is built too. */
	static bool handlesImplementation(QString implementation, bool igstkPluginBuilt);
	static bool isIgstkPluginBuilt();

	TrackingSystemNDIService();
	virtual ~TrackingSystemNDIService();

	virtual QString getUid() const { return "org.custusx.core.tracking.system.ndi"; }
	virtual std::vector<ToolPtr> getTools();
	virtual ToolPtr getReference() { return mReference; }

	virtual void setState(const Tool::State val);
	virtual void setConfigurationFile(QString configurationFile);
	virtual void setLoggingFolder(QString loggingFolder);
	virtual TrackerConfigurationPtr getConfiguration();

signals:
	// Internal state-change signals
	void configured();
	void deconfigured();
	void initialized();
	void uninitialized();
	void trackingStarted();
	void trackingStopped();

private slots:
	virtual void configure();
	virtual void deconfigure();
	virtual void initialize();
	virtual void uninitialize();
	virtual void startTracking();
	virtual void stopTracking();

	void trackerConfiguredSlot(bool on);
	void initializedSlot(bool on);
	void trackerTrackingSlot(bool on);

	void trackerErrorSlot();
	void startTrackingAfterInitSlot();
	void initializeAfterConfigSlot();
	void uninitializeAfterTrackingStoppedSlot();
	void configureAfterDeconfigureSlot();
	void globalConfigurationFileChangedSlot(QString key);

protected:
	NdiConnectionSettings readConnectionSettings() const;

private:
	void destroyTrackerThread();

	std::vector<ToolPtr> mTools;
	ToolPtr mReference;

	NdiTrackerThreadPtr mTrackerThread;
};

/**
 * @}
 */
} // namespace cx

#endif /* CXTRACKINGSYSTEMNDISERVICE_H_ */
