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
