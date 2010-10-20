/*
 * cxRenderTimer.cpp
 *
 *  Created on: Oct 19, 2010
 *      Author: christiana
 */

#include "cxRenderTimer.h"
#include <numeric>

namespace cx
{

RenderTimer::RenderTimer()
{
  mRenderClock.start();
  mIntervalClock.start();
  this->reset();
}

void RenderTimer::reset(int interval)
{
  mIntervalClock.restart();
  mNumberOfRenderings = 0;
  mInterval = interval;
  mOffRenderTime.clear();
  mRenderTime.clear();
}

/** rendering engine must call this before a render
 */
void RenderTimer::beginRender()
{
  //    mLastBeginRenderTime = QTime::currentTime();
  //    int other = mLastEndRenderTime.msecsTo(mLastBeginRenderTime);
  mOffRenderTime.push_back(mRenderClock.restart());
}

/** rendering engine must call this after a render
 */
void RenderTimer::endRender()
{
  //    mLastEndRenderTime = QTime::currentTime();
  //    int other = mLastBeginRenderTime.msecsTo(mLastEndRenderTime);
  //    mRenderTime.push_back(other);
  mRenderTime.push_back(mRenderClock.restart());

  ++mNumberOfRenderings;
}

/** return frames per second during the last interval.
 */
double RenderTimer::getFPS()
{
  if (!mIntervalClock.elapsed())
    return -1;
  return 1000.0 * double(mNumberOfRenderings) / mIntervalClock.elapsed();
}

bool RenderTimer::intervalPassed() const
{
  return mIntervalClock.elapsed() > mInterval;
}

void RenderTimer::dumpStatistics()
{
  std::cout << "=== Render time statistics ===" << std::endl;
  std::cout << "Interval \t= " << mInterval << " ms" << std::endl;
  std::cout << "Elapsed  \t= " << mIntervalClock.elapsed() <<  " ms" << std::endl;
  std::cout << "FPS      \t= " << this->getFPS() << " frames/s" <<  std::endl;

  double maxRenderTime = *std::max_element(mRenderTime.begin(), mRenderTime.end());
  double maxOffRenderTime = *std::max_element(mOffRenderTime.begin(), mOffRenderTime.end());

  double meanRenderTime = std::accumulate(mRenderTime.begin(), mRenderTime.end(), 0)/mRenderTime.size();
  double meanOffRenderTime = std::accumulate(mOffRenderTime.begin(), mOffRenderTime.end(), 0)/mOffRenderTime.size();
  double meanTotalTime = meanRenderTime + meanOffRenderTime;

  std::cout << "Mean time:\t= " << meanTotalTime << " ms/frame" << std::endl;
  std::cout << std::endl;
  std::cout << "Mean times: \t"<< "render: " << meanRenderTime << "\toff: " << meanOffRenderTime << std::endl;
  std::cout << "Max times: \t" << "render: " << maxRenderTime << "\tother: " << maxOffRenderTime << std::endl;
  std::cout << std::endl;

  std::cout << "Render Times: " << mRenderTime.size() << std::endl;
  for (unsigned i = 0; i < mRenderTime.size(); ++i)
    std::cout << mRenderTime[i] << "  ";
  std::cout << std::endl;

  std::cout << "Off Render Times: " << mOffRenderTime.size() << std::endl;
  for (unsigned i = 0; i < mOffRenderTime.size(); ++i)
    std::cout << mOffRenderTime[i] << "  ";
  std::cout << std::endl;

  std::cout << "================================" << std::endl;
  std::cout << std::endl;
}

} // namespace cx
