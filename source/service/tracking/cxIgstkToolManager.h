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

#ifndef CXIGSTKTOOLMANAGER_H_
#define CXIGSTKTOOLMANAGER_H_

#include <map>
#include <vector>
#include <QThread>
#include <QTimer>
#include <QMutex>
#include "cxIgstkTracker.h"
#include "cxTool.h"
#include "cxIgstkTool.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxServiceTracking
 * @{
 */

/**
 * \brief Manager for IGSTK interface.
 * \ingroup cxServiceTracking
 *
 * The manager exists inside IgstkTrackerThread .
 *
 * \date Mar 17, 2011
 * \author Janne Beate Bakeng, SINTEF
 * \author Christian Askeland, SINTEF
 */
class IgstkToolManager: public QObject
{
Q_OBJECT

public:
	IgstkToolManager(IgstkTracker::InternalStructure trackerStructure,
					std::vector<IgstkTool::InternalStructure> toolStructures,
					IgstkTool::InternalStructure referenceToolStructure);
	virtual ~IgstkToolManager();

	std::map<QString, IgstkToolPtr> getTools(); ///< ThreadSafe
	IgstkToolPtr getRefereceTool(); ///< ThreadSafe

signals:
	void initialized(bool on); ///< when all trackers and tools are initialized == true, else false
	void tracking(bool on);
	void error();

public slots:
	void initializeSlot(bool on); ///< connects to the hardware
	void trackSlot(bool on); ///< tracking on or off
	void reattachToolsSlot(); ///< detach and attach tools

private slots:
	void trackerTrackingSlot(bool);
	void checkTimeoutsAndRequestTransformSlot();
	void deviceInitializedSlot(bool);
	void attachToolsWhenTrackerIsInitializedSlot(bool);

private:
	void createTracker(IgstkTracker::InternalStructure trackerStructure);
	void createTools(std::vector<IgstkTool::InternalStructure> toolStructures,
					IgstkTool::InternalStructure referenceToolStructure);
	IgstkToolPtr addIgstkTools(IgstkTool::InternalStructure& toolStructure);
	void setReferenceAndTrackerOnTools();

	void printStatus(); ///< just for debugging

	TrackerPtr mTracker;
	QMutex mToolMutex; ///< protects mTools
	std::map<QString, IgstkToolPtr> mTools;
	int mInitAnsweres; ///< keeps track of how many tools and trackers have gotten an answer from the hardware
	QMutex mReferenceMutex; ///< protects mReferenceTool
	IgstkToolPtr mReferenceTool;

	QTimer* mTimer; ///< timer controlling the demand of transforms
	igstk::PulseGenerator::Pointer mPulseGenerator;

	bool mInternalInitialized;

};
typedef boost::shared_ptr<IgstkToolManager> IgstkToolManagerPtr;

/**
 * @}
 */
}

#endif /* CXIGSTKTOOLMANAGER_H_ */
