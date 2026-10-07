/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXNDITRACKERTHREAD_H_
#define CXNDITRACKERTHREAD_H_

#include "org_custusx_core_tracking_system_ndi_Export.h"
#include <memory>

#include <map>
#include <vector>
#include <QThread>

#include "cxNdiTool.h"
#include "cxNdiTrackerManager.h"
#include "cxToolFileParser.h"

namespace cx
{
/**
 * \file
 * \addtogroup org_custusx_core_tracking_system_ndi
 * @{
 */

/**
 * \brief Thread that owns the NDI hardware interface (NdiTrackerManager).
 * \ingroup org_custusx_core_tracking_system_ndi
 *
 * The NdiTrackerManager lives in this thread's event loop. All hardware
 * communication is done from within this thread.
 */
class org_custusx_core_tracking_system_ndi_EXPORT NdiTrackerThread : public QThread
{
	Q_OBJECT

public:
	NdiTrackerThread(ToolFileParser::TrackerInternalStructure trackerStructure,
					 std::vector<ToolFileParser::ToolInternalStructurePtr> toolStructures,
					 ToolFileParser::ToolInternalStructurePtr referenceToolStructure);
	virtual ~NdiTrackerThread();

	void initialize(bool on); ///< Connect/disconnect from hardware. Thread-safe.
	void track(bool on);      ///< Start/stop tracking mode. Thread-safe.
	void setConnectionSettings(const NdiConnectionSettings& connection); ///< Thread-safe, call before initialize()

	std::map<QString, NdiToolPtr> getTools(); ///< Thread-safe
	NdiToolPtr getReferenceTool();            ///< Thread-safe

signals:
	void configured(bool on);
	void initialized(bool on);
	void tracking(bool on);
	void error();

	void requestInitialize(bool on); ///< internal signal
	void requestTrack(bool on);      ///< internal signal

private:
	void run() override;
	void configure();
	void deconfigure();

	ToolFileParser::TrackerInternalStructure mInitTrackerStructure;
	std::vector<ToolFileParser::ToolInternalStructurePtr> mInitToolStructures;
	ToolFileParser::ToolInternalStructurePtr mInitReferenceToolStructure;

	NdiTrackerManagerPtr mManager;
};
typedef std::shared_ptr<NdiTrackerThread> NdiTrackerThreadPtr;

/**
 * @}
 */
} // namespace cx

#endif /* CXNDITRACKERTHREAD_H_ */
