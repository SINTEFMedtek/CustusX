/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXIGSTKTRACKERTHREAD_H_
#define CXIGSTKTRACKERTHREAD_H_

#include "org_custusx_core_tracking_system_igstk_Export.h"

#include <map>
#include <vector>
#include <QThread>
#include "cxIgstkTracker.h"
#include "cxIgstkTrackerThread.h"
#include "cxToolUsingIGSTK.h"
#include "cxIgstkTool.h"
#include "cxIgstkToolManager.h"

namespace cx
{
/**
 * \file
 * \addtogroup org_custusx_core_tracking_igstk
 * @{
 */

/**
 * \brief Thread containing all of IGSTK
 * \ingroup org_custusx_core_tracking_igstk
 *
 * \sa ToolManager
 *
 * \date Mar 16, 2011
 * \author Janne Beate Bakeng, SINTEF
 * \author Christian Askeland, SINTEF
 */
class org_custusx_core_tracking_system_igstk_EXPORT IgstkTrackerThread: public QThread
{
Q_OBJECT

public:
		IgstkTrackerThread(ToolFileParser::TrackerInternalStructure trackerStructure,
										std::vector<ToolFileParser::ToolInternalStructurePtr> toolStructures,
										ToolFileParser::ToolInternalStructurePtr referenceToolStructure);
	virtual ~IgstkTrackerThread();

	void initialize(bool on); ///< connects to the hardware. Threadsafe.
	void track(bool on); ///< tracking on or off. Threadsafe.

	std::map<QString, IgstkToolPtr> getTools(); ///< ThreadSafe.
	IgstkToolPtr getRefereceTool(); ///< ThreadSafe

signals:
	void configured(bool on);
	void initialized(bool on); ///< system is initialized
	void tracking(bool on);
	void error();

	void requestInitialize(bool on); ///< internal signal
	void requestTrack(bool on); ///< internal signal

private:
	virtual void run();
	void configure();
	void deconfigure();

	ToolFileParser::TrackerInternalStructure mInitTrackerStructure;
	std::vector<ToolFileParser::ToolInternalStructurePtr> mInitToolStructures;
	ToolFileParser::ToolInternalStructurePtr mInitReferenceToolStructure;

	IgstkToolManagerPtr mManager;
};
typedef boost::shared_ptr<IgstkTrackerThread> IgstkTrackerThreadPtr;

/**
 * @}
 */
}

#endif /* CXIGSTKTRACKERTHREAD_H_ */
