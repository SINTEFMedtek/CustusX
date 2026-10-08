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
#include <memory>

#include <map>
#include <vector>
#include <QObject>
#include <QMutex>
#include <QStringList>

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
	static QString combinePositionWarnings(QString toolWarning, QString referenceWarning);
	/** Messages for the system status bits that are set in status but not in previousStatus. */
	static QStringList describeNewSystemStatus(int previousStatus, int status);
	/** A tool was plugged in or removed: the tools are set up again before tracking starts the next time. */
	static bool hasNewToolChange(int previousStatus, int status);
	static double getDefaultMaxErrorRMS(TRACKING_SYSTEM trackerType);
	static bool isPolaris(TRACKING_SYSTEM trackerType);
	static int getIlluminatorRateToSet(TRACKING_SYSTEM trackerType, int rate_Hz); ///< 0: leave the device's default
	static const int kMaxConsecutiveReadFailures = 3;

signals:
	void initialized(bool on);
	void tracking(bool on);
	void deviceInUse(); ///< Before error(): the device is used by another application
	void error();

public slots:
	void initializeSlot(bool on); ///< Connect/disconnect from hardware
	void trackSlot(bool on);      ///< Start/stop tracking mode
	void poll();                  ///< Read one frame from the device. Called by the timer while tracking.

public:
	/** How long to wait before the next poll.
	 *  A device whose BX reply waits for its next frame (Aurora) is polled again at once after a new
	 *  frame: any wait would lose frames. Repeated frames from such a device (Polaris tools updated at a
	 *  lower rate than the camera) also come after a wait for the next frame. A device that replies at
	 *  once with the last frame gives quick repeats: after its next new frame, wait until shortly
	 *  before the frame after that, so it isn't polled continuously. After a repeated frame, poll again soon. */
	static int getPollInterval_ms(bool newFrame, int quickRepeatsBeforeNewFrame, int waitBeforeNextFrame_ms);
	static constexpr double kQuickReply_ms = 5.0; ///< A repeated frame that came faster than this was not waited for
	int getCurrentPollInterval_ms() const;
	static const int kNewFramePollInterval_ms = 0;
	static const int kRepeatPollInterval_ms = 2;

private:
	bool connectToDevice();
	void disconnectFromDevice();
	bool setupTools();
	void setupToolsAgain(); ///< After tools were plugged in or removed
	void distributeFrame(const NdiFrame& frame, double timestamp);
	void setAllToolsInvisible();
	void reportSystemStatus(int status);
	void setIlluminatorRate(int rate_Hz);
	void setMaxErrorRMS(double maxErrorRMS_mm);

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
	int mConsecutiveReadFailures;
	int mLastSystemStatus; ///< Kept while tracking is restarted: the device keeps reporting the same bits
	bool mToolsChanged;
	int mIlluminatorRate_Hz;
	NdiFrameRateEstimator mFrameRate;
	int mQuickRepeats; ///< Polls that quickly gave the last frame again, since the last new frame
	ToolFileParser::TrackerInternalStructure mTrackerStructure;
	bool mInitialized;
	bool mTracking;
};
typedef std::shared_ptr<NdiTrackerManager> NdiTrackerManagerPtr;

} // namespace cx

#endif /* CXNDITRACKERMANAGER_H_ */
