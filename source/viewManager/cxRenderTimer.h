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

namespace cx
{

typedef boost::shared_ptr<class RenderTimer> RenderTimerPtr;

/**Helper class for counting time spent by the rendering process
 *
 */
class RenderTimer
{
public:
  RenderTimer()
  {
    mRenderingTime = new QTime;
    mNumberOfRenderings = 0;
    mRenderingTime->start();
  }
 ~RenderTimer()
  {
    delete mRenderingTime;
  }
  void reset()
  {
    mRenderingTime->restart();
    mNumberOfRenderings = 0;
  }
  void beginRender()
  {

  }
  void endRender()
  {
    ++mNumberOfRenderings;
  }
  int getRenderCount()
  {
    return mNumberOfRenderings;
  }
  QTime* getTime()
  {
    return mRenderingTime;
  }
//  void setInterval(int val)
//  {
//    mInterval = val;
//  }
//  bool PassedInterval() const
//  {
//    return
//  }

private:
  int mInterval; ///< the interval between each readout+reset of the calculated values.
  QTime* mRenderingTime; ///< Time object used to calculate number of renderings per second (FPS)
  int mNumberOfRenderings; ///< Variable used to calculate FPS
};

} // namespace cx

#endif /* CXRENDERTIMER_H_ */
