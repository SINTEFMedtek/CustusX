/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXIGSTKTOOLMANAGER_H_
#define CXIGSTKTOOLMANAGER_H_

#include "org_custusx_core_tracking_system_igstk_Export.h"

#include <map>
#include <vector>
#include <QThread>
#include <QTimer>
#include <QMutex>
#include "cxIgstkTracker.h"
#include "cxToolUsingIGSTK.h"
#include "cxIgstkTool.h"

namespace cx
{
/**
 * \file
 * \addtogroup org_custusx_core_tracking_igstk
 * @{
 */


void sampleInfo2xml(const igstk::NDITracker::TrackingSampleInfo& info, QDomElement &node);


/**
 * \brief Manager for IGSTK interface.
 * \ingroup org_custusx_core_tracking_igstk
 *
 * The manager exists inside IgstkTrackerThread .
 *
 * \date Mar 17, 2011
 * \author Janne Beate Bakeng, SINTEF
 * \author Christian Askeland, SINTEF
 */
class org_custusx_core_tracking_system_igstk_EXPORT IgstkToolManager: public QObject
{
Q_OBJECT

public:
    IgstkToolManager(ToolFileParser::TrackerInternalStructure trackerStructure,
										std::vector<ToolFileParser::ToolInternalStructurePtr> toolStructures,
										ToolFileParser::ToolInternalStructurePtr referenceToolStructure);
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

private slots:
	void trackerTrackingSlot(bool);
	void checkTimeoutsAndRequestTransformSlot();
	void deviceInitializedSlot(bool);
	void attachToolsWhenTrackerIsInitializedSlot(bool);

private:
    void createTracker(ToolFileParser::TrackerInternalStructure trackerStructure);
		void createTools(std::vector<ToolFileParser::ToolInternalStructurePtr> toolStructures,
										ToolFileParser::ToolInternalStructurePtr referenceToolStructure);
		IgstkToolPtr addIgstkTools(ToolFileParser::ToolInternalStructurePtr toolStructure);
	void setReferenceAndTrackerOnTools();

	void printStatus(); ///< just for debugging

	TrackerPtr mTracker;
	QMutex mToolMutex; ///< protects mTools
	std::map<QString, IgstkToolPtr> mTools;
	size_t mInitAnsweres; ///< keeps track of how many tools and trackers have gotten an answer from the hardware
	QMutex mReferenceMutex; ///< protects mReferenceTool
	IgstkToolPtr mReferenceTool;

	QTimer* mTimer; ///< timer controlling the demand of transforms

	bool mInternalInitialized;

};
typedef boost::shared_ptr<IgstkToolManager> IgstkToolManagerPtr;

/**
 * @}
 */
}

#endif /* CXIGSTKTOOLMANAGER_H_ */
