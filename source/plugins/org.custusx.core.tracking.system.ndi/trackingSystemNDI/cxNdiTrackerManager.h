/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXNDITRACKERMANAGER_H_
#define CXNDITRACKERMANAGER_H_

#include "org_custusx_core_tracking_system_ndi_Export.h"

#include <map>
#include <vector>
#include <QObject>
#include <QMutex>

#include "cxNdiDevice.h"
#include "cxNdiFrameRateEstimator.h"
#include "cxNdiTool.h"
#include "cxNdiToolSetup.h"
#include "cxToolFileParser.h"

class QTimer;

namespace cx
{

/** \brief Connects to an NDI device, sets up the tools and distributes their positions.
 *
 * Lives in the tracker thread (NdiTrackerThread); all device calls are made there.
 *
 * \ingroup org_custusx_core_tracking_system_ndi
 */
class org_custusx_core_tracking_system_ndi_EXPORT NdiTrackerManager : public QObject
{
	Q_OBJECT
public:
	/** device: the device to use, or empty to use ndicapi (NdiCapiDevice). */
	NdiTrackerManager(ToolFileParser::TrackerInternalStructure trackerStructure,
					  std::vector<ToolFileParser::ToolInternalStructurePtr> toolStructures,
					  ToolFileParser::ToolInternalStructurePtr referenceToolStructure,
					  NdiDevicePtr device = NdiDevicePtr());
	virtual ~NdiTrackerManager();

	std::map<QString, NdiToolPtr> getTools();   ///< Thread-safe
	NdiToolPtr getReferenceTool();              ///< Thread-safe
	bool isInitialized() const;
	bool isTracking() const;
	void setConnectionSettings(const NdiConnectionSettings& connection); ///< Thread-safe. Used by the next connect.

	static NdiToolConfig createToolConfig(ToolFileParser::ToolInternalStructurePtr structure, TRACKING_SYSTEM trackerType);
	static const int kMaxConsecutiveReadFailures = 3;

signals:
	void initialized(bool on);
	void tracking(bool on);
	void error();

public slots:
	void initializeSlot(bool on); ///< Connect/disconnect from hardware
	void trackSlot(bool on);      ///< Start/stop tracking mode
	void poll();                  ///< Read one frame from the device. Called by the timer while tracking.

private:
	bool connectToDevice();
	void disconnectFromDevice();
	bool setupTools();
	void distributeFrame(const NdiFrame& frame, double timestamp);
	void setAllToolsInvisible();
	void startFastPoll();
	void scheduleNextPoll(double now_ms);

	NdiDevicePtr mDevice;
	QMutex mConnectionMutex;
	NdiConnectionSettings mConnection;
	QMutex mToolMutex;
	std::map<QString, NdiToolPtr> mTools;       ///< All configured tools by uid
	std::map<int, NdiToolPtr> mPortHandleToTool;
	std::vector<int> mPortHandles;
	NdiToolPtr mReferenceTool;
	QTimer* mTimer;
	unsigned long mLastFrameNumber;
	NdiFrameRateEstimator mFrameRate;
	bool mInFastPollMode;
	int mConsecutiveReadFailures;
	ToolFileParser::TrackerInternalStructure mTrackerStructure;
	bool mInitialized;
	bool mTracking;
};
typedef boost::shared_ptr<NdiTrackerManager> NdiTrackerManagerPtr;

} // namespace cx

#endif /* CXNDITRACKERMANAGER_H_ */
