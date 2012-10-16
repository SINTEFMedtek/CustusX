/**
 * cxRenderTimer.cpp
 *
 *  \date Oct 19, 2010
 *      \author christiana
 */

#include "cxRenderTimer.h"
#include <numeric>
#include <sstream>
#include "sscTypeConversions.h"

namespace cx
{

RenderTimer::RenderTimer()
{
  mRenderClock.start();
  mIntervalClock.start();
  this->reset();
}

RenderTimer::RenderTimer(QString name)
{
	mName = name;
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

QString RenderTimer::dumpStatistics()
{
  std::stringstream ss;
  ss << "=== " << mName << " statistics ===" << std::endl;
  ss << "Interval \t= " << mInterval << " ms" << std::endl;
  ss << "Elapsed  \t= " << mIntervalClock.elapsed() <<  " ms" << std::endl;
  ss << "FPS      \t= " << this->getFPS() << " frames/s" <<  std::endl;


  if (mRenderTime.empty() || mOffRenderTime.empty())
    return qstring_cast(ss.str());

  double maxRenderTime = *std::max_element(mRenderTime.begin(), mRenderTime.end());
  double maxOffRenderTime = *std::max_element(mOffRenderTime.begin(), mOffRenderTime.end());

  double meanRenderTime = std::accumulate(mRenderTime.begin(), mRenderTime.end(), 0)/mRenderTime.size();
  double meanOffRenderTime = std::accumulate(mOffRenderTime.begin(), mOffRenderTime.end(), 0)/mOffRenderTime.size();
  double meanTotalTime = meanRenderTime + meanOffRenderTime;

  ss << "Mean time:\t= " << meanTotalTime << " ms/frame" << std::endl;
  ss << std::endl;
  ss << "Mean times: \t"<< "render: " << meanRenderTime << "\tother: " << meanOffRenderTime << std::endl;
  ss << "Max times: \t" << "render: " << maxRenderTime  << "\tother: " << maxOffRenderTime  << std::endl;
  ss << std::endl;

  ss << "Render Times: " << mRenderTime.size() << std::endl;
  for (unsigned i = 0; i < mRenderTime.size(); ++i)
    ss << mRenderTime[i] << "  ";
  ss << std::endl;

  ss << "Off Render Times: " << mOffRenderTime.size() << std::endl;
  for (unsigned i = 0; i < mOffRenderTime.size(); ++i)
    ss << mOffRenderTime[i] << "  ";
  ss << std::endl;

  ss << "================================" << std::endl;
  ss << std::endl;
  return qstring_cast(ss.str());
}

QString RenderTimer::dumpStatisticsSmall()
{
	  std::stringstream ss;
	  ss << mName << ":\t";
	  ss << "Elapsed=" << mIntervalClock.elapsed() <<  " ms";
	  ss << "\tFPS=" << this->getFPS() << " fps";

	  if (mRenderTime.empty() || mOffRenderTime.empty())
	    return qstring_cast(ss.str());

	  double maxRenderTime = *std::max_element(mRenderTime.begin(), mRenderTime.end());
	  double maxOffRenderTime = *std::max_element(mOffRenderTime.begin(), mOffRenderTime.end());

	  double meanRenderTime = std::accumulate(mRenderTime.begin(), mRenderTime.end(), 0)/mRenderTime.size();
	  double meanOffRenderTime = std::accumulate(mOffRenderTime.begin(), mOffRenderTime.end(), 0)/mOffRenderTime.size();
	  double meanTotalTime = meanRenderTime + meanOffRenderTime;

	  ss << "\tMean="<< "(total=in+other)=(" << meanTotalTime << "=" << meanRenderTime << "+" << meanOffRenderTime << ") ms/frame";
	  ss << "\tMax=(" << maxRenderTime  << "+" << maxOffRenderTime << ")";

	  return qstring_cast(ss.str());
}


} // namespace cx
