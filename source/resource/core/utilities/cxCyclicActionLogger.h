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
#ifndef CXCYCLICACTIONLOGGER_H_
#define CXCYCLICACTIONLOGGER_H_

#include "cxResourceExport.h"

#include "boost/shared_ptr.hpp"
#include <QTime>
#include <vector>

namespace cx
{

/**
* \file
* \addtogroup cx_resource_core_utilities
* @{
*/

/**Helper class for counting time spent by the rendering process or other
 * process running in a cycle.
 *
 *  \date Oct 19, 2010
 *  \date Oct 16, 2012
 *  \author christiana
 */
class cxResource_EXPORT CyclicActionLogger
{
public:
	CyclicActionLogger();
	explicit CyclicActionLogger(QString name);
	void reset(int interval = 1000);

	void begin(); ///< start timing for this cycle
	void time(QString id); ///< store time from begin or last time()

	double getFPS();
	bool intervalPassed() const;
	QString dumpStatistics();
	QString dumpStatisticsSmall();

	int getTime(QString id);
	int getTotalLoggedTime();///< Total time contained in entered id's (id outside is not counted)

private:
	QString mName;
	struct Entry
	{
		QString id;
		std::vector<double> time;
	};
	std::vector<Entry> mTiming;
	QTime mRenderClock; ///< clock for counting time between and inside renderings
	int mInterval; ///< the interval between each readout+reset of the calculated values.
	QTime mIntervalClock; ///< Time object used to calculate number of renderings per second (FPS)

	double getMeanTime(std::vector<double> &time);
	double getMaxTime(std::vector<double> &time);
	std::vector<Entry>::iterator getTimingVectorIterator(QString id);
};

/**
* @}
*/

} // namespace cx

#endif /* CXCYCLICACTIONLOGGER_H_ */
