/*
 * cxRenderTimer.h
 *
 *  Created on: Oct 19, 2010
 *      Author: christiana
 */

#ifndef CXRENDERTIMER_H_
#define CXRENDERTIMER_H_

#include "boost/shared_ptr.hpp"
#include <QTime>
#include <vector>
#include <iostream>

namespace cx
{

typedef boost::shared_ptr<class RenderTimer> RenderTimerPtr;

/**Helper class for counting time spent by the rendering process
 *
 */
class RenderTimer
{
public:
  RenderTimer();
  void reset(int interval = 1000);
  void beginRender();
  void endRender();
  double getFPS();
  bool intervalPassed() const;
  QString dumpStatistics();

private:
  std::vector<double> mOffRenderTime;
  std::vector<double> mRenderTime;
  //QTime mLastBeginRenderTime, mLastEndRenderTime;
  QTime mRenderClock; ///< clock for counting time between and inside renderings
  int mInterval; ///< the interval between each readout+reset of the calculated values.
  QTime mIntervalClock; ///< Time object used to calculate number of renderings per second (FPS)
  int mNumberOfRenderings; ///< Variable used to calculate FPS
};

} // namespace cx

#endif /* CXRENDERTIMER_H_ */
