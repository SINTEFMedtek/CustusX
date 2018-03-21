/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
