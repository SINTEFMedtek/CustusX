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
                    std::vector<ToolFileParser::ToolInternalStructure> toolStructures,
                    ToolFileParser::ToolInternalStructure referenceToolStructure);
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
    void createTools(std::vector<ToolFileParser::ToolInternalStructure> toolStructures,
                    ToolFileParser::ToolInternalStructure referenceToolStructure);
    IgstkToolPtr addIgstkTools(ToolFileParser::ToolInternalStructure& toolStructure);
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
