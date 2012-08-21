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

#ifndef CXIGSTKTRACKERTHREAD_H_
#define CXIGSTKTRACKERTHREAD_H_

#include <map>
#include <vector>
#include <QThread>
#include <QTimer>
#include "cxIgstkTracker.h"
#include "cxIgstkTrackerThread.h"
#include "cxTool.h"
#include "cxIgstkTool.h"
#include <cxIgstkToolManager.h>

namespace cx
{
/**
 * \file
 * \addtogroup cxServiceTracking
 * @{
 */

/**
 * \brief Thread containing all of IGSTK
 * \ingroup cxServiceTracking
 *
 * \sa ToolManager
 *
 * \date Mar 16, 2011
 * \author Janne Beate Bakeng, SINTEF
 * \author Christian Askeland, SINTEF
 */
class IgstkTrackerThread: public QThread
{
Q_OBJECT

public:
	IgstkTrackerThread(IgstkTracker::InternalStructure trackerStructure,
					std::vector<IgstkTool::InternalStructure> toolStructures,
					IgstkTool::InternalStructure referenceToolStructure);
	virtual ~IgstkTrackerThread();

	void initialize(bool on); ///< connects to the hardware. Threadsafe.
	void track(bool on); ///< tracking on or off. Threadsafe.
	void reattachTools(); ///< detach and attach tools. Threadsafe.

	std::map<QString, IgstkToolPtr> getTools(); ///< ThreadSafe.
	IgstkToolPtr getRefereceTool(); ///< ThreadSafe

signals:
	void configured(bool on);
	void initialized(bool on); ///< system is initialized
	void tracking(bool on);
	void error();

	void requestInitialize(bool on); ///< internal signal
	void requestTrack(bool on); ///< internal signal
	void requestReattachTools(); ///< internal signal

private:
	virtual void run();
	void configure();
	void deconfigure();

	IgstkTracker::InternalStructure mInitTrackerStructure;
	std::vector<IgstkTool::InternalStructure> mInitToolStructures;
	IgstkTool::InternalStructure mInitReferenceToolStructure;

	IgstkToolManagerPtr mManager;
};
typedef boost::shared_ptr<IgstkTrackerThread> IgstkTrackerThreadPtr;

/**
 * @}
 */
}

#endif /* CXIGSTKTRACKERTHREAD_H_ */
