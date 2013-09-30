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
#ifndef CXRENDERTIMER_H_
#define CXRENDERTIMER_H_

#include "boost/shared_ptr.hpp"
#include <QTime>
#include <vector>

namespace cx
{

/**
* \file
* \addtogroup cxResourceUtilities
* @{
*/

typedef boost::shared_ptr<class CyclicActionTimer> CyclicActionTimerPtr;

/**Helper class for counting time spent by the rendering process or other
 * process running in a cycle.
 *
 *  \date Oct 19, 2010
 *  \date Oct 16, 2012
 *  \author christiana
 */
class CyclicActionTimer
{
public:
	CyclicActionTimer();
	explicit CyclicActionTimer(QString name);
	void reset(int interval = 1000);
	void beginRender();
	void endRender();

	void begin(); ///< start timing for this cycle
	void time(QString id); ///< store time from begin or last time()

	double getFPS();
	bool intervalPassed() const;
	QString dumpStatistics();
	QString dumpStatisticsSmall();

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
};

/**
* @}
*/

} // namespace cx

#endif /* CXRENDERTIMER_H_ */
