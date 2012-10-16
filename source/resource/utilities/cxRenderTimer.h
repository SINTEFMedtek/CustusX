/*
 * cxRenderTimer.h
 *
 *  \date Oct 19, 2010
 *      \author christiana
 */

#ifndef CXRENDERTIMER_H_
#define CXRENDERTIMER_H_

#include "boost/shared_ptr.hpp"
#include <QTime>
#include <vector>
#include <iostream>
#include <map>

namespace cx
{

/**
* \file
* \addtogroup cxResourceUtilities
* @{
*/

typedef boost::shared_ptr<class RenderTimer> RenderTimerPtr;

/**Helper class for counting time spent by the rendering process
 *
 */
class RenderTimer
{
public:
  RenderTimer();
  explicit RenderTimer(QString name);
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
//  std::map<QString, std::vector<double> > mTiming;
//  std::vector<std::map<QString, double> > mRenderTimes;
//  std::vector<double> mOffRenderTime;
//  std::vector<double> mRenderTime;
  //QTime mLastBeginRenderTime, mLastEndRenderTime;
  QTime mRenderClock; ///< clock for counting time between and inside renderings
  int mInterval; ///< the interval between each readout+reset of the calculated values.
  QTime mIntervalClock; ///< Time object used to calculate number of renderings per second (FPS)
//  int mNumberOfRenderings; ///< Variable used to calculate FPS
};

/**
* @}
*/

} // namespace cx

#endif /* CXRENDERTIMER_H_ */
