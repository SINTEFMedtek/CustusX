/**
 * cxRenderTimer.cpp
 *
 *  \date Oct 19, 2010
 *      \author christiana
 */

#include "cxRenderTimer.h"
#include <numeric>
#include <sstream>
#include <QStringList>
#include "sscTypeConversions.h"
#include <cmath>

namespace cx
{

CyclicActionTimer::CyclicActionTimer()
{
  mRenderClock.start();
  mIntervalClock.start();
  this->reset();
}

CyclicActionTimer::CyclicActionTimer(QString name)
{
	mName = name;
	mRenderClock.start();
	mIntervalClock.start();
	this->reset();
}

void CyclicActionTimer::reset(int interval)
{
  mIntervalClock.restart();
  mInterval = interval;
  mTiming.clear();
}

/** rendering engine must call this before a render
 */
void CyclicActionTimer::beginRender()
{
	this->begin();
}

/** rendering engine must call this after a render
 */
void CyclicActionTimer::endRender()
{
	this->time("render");
}

void CyclicActionTimer::begin()
{
	this->time("outside");
}

void CyclicActionTimer::time(QString id)
{
	  std::vector<Entry>::iterator iter;
	  for (iter=mTiming.begin(); iter!=mTiming.end(); ++iter)
	  {
		  if (iter->id!=id)
			  continue;
		  iter->time.push_back(mRenderClock.restart());
		  return;
	  }

	  Entry newEntry;
	  newEntry.id = id;
	  newEntry.time.push_back(mRenderClock.restart());
	  mTiming.push_back(newEntry);
}

/** return frames per second during the last interval.
 */
double CyclicActionTimer::getFPS()
{
  if (!mIntervalClock.elapsed())
    return -1;
  double numberOfRenderings =  mTiming.empty() ? 0 : mTiming.front().time.size();
  double fps = 1000.0 * numberOfRenderings / mIntervalClock.elapsed();
  return floor(fps+0.5); // round
}

bool CyclicActionTimer::intervalPassed() const
{
  return mIntervalClock.elapsed() > mInterval;
}

QString CyclicActionTimer::dumpStatistics()
{
	return this->dumpStatisticsSmall();
//  std::stringstream ss;
//  ss << "=== " << mName << " statistics ===" << std::endl;
//  ss << "Interval \t= " << mInterval << " ms" << std::endl;
//  ss << "Elapsed  \t= " << mIntervalClock.elapsed() <<  " ms" << std::endl;
//  ss << "FPS      \t= " << this->getFPS() << " frames/s" <<  std::endl;
//
//
//  if (mRenderTime.empty() || mOffRenderTime.empty())
//    return qstring_cast(ss.str());
//
//  double maxRenderTime = *std::max_element(mRenderTime.begin(), mRenderTime.end());
//  double maxOffRenderTime = *std::max_element(mOffRenderTime.begin(), mOffRenderTime.end());
//
//  double meanRenderTime = std::accumulate(mRenderTime.begin(), mRenderTime.end(), 0)/mRenderTime.size();
//  double meanOffRenderTime = std::accumulate(mOffRenderTime.begin(), mOffRenderTime.end(), 0)/mOffRenderTime.size();
//  double meanTotalTime = meanRenderTime + meanOffRenderTime;
//
//  ss << "Mean time:\t= " << meanTotalTime << " ms/frame" << std::endl;
//  ss << std::endl;
//  ss << "Mean times: \t"<< "render: " << meanRenderTime << "\tother: " << meanOffRenderTime << std::endl;
//  ss << "Max times: \t" << "render: " << maxRenderTime  << "\tother: " << maxOffRenderTime  << std::endl;
//  ss << std::endl;
//
//  ss << "Render Times: " << mRenderTime.size() << std::endl;
//  for (unsigned i = 0; i < mRenderTime.size(); ++i)
//    ss << mRenderTime[i] << "  ";
//  ss << std::endl;
//
//  ss << "Off Render Times: " << mOffRenderTime.size() << std::endl;
//  for (unsigned i = 0; i < mOffRenderTime.size(); ++i)
//    ss << mOffRenderTime[i] << "  ";
//  ss << std::endl;
//
//  ss << "================================" << std::endl;
//  ss << std::endl;
//  return qstring_cast(ss.str());
}

QString CyclicActionTimer::dumpStatisticsSmall()
{
	  std::stringstream ss;
	  ss << mName << ":\t";
	  ss << "Elapsed=" << mIntervalClock.elapsed() <<  " ms";
	  ss << "\tFPS=" << this->getFPS() << " fps";

	  if (mTiming.empty())
	    return qstring_cast(ss.str());

	  QStringList meanTimes;
	  QStringList maxTimes;
	  QStringList ids;
	  double totalTime=0;

	  std::vector<Entry>::iterator entry;
	  for (entry=mTiming.begin(); entry!=mTiming.end(); ++entry)
	  {
		  double maxTime = *std::max_element(entry->time.begin(), entry->time.end());
		  double meanTime = std::accumulate(entry->time.begin(), entry->time.end(), 0)/entry->time.size();
		  totalTime += meanTime;

		  meanTimes << qstring_cast(int(meanTime));
		  maxTimes << qstring_cast(int(maxTime));
		  ids << entry->id;
	  }

	  ss << QString("\t(total=%1)").arg(ids.join("+"));
	  ss << QString("\tMean:(%2=%3) ms/frame").arg(totalTime).arg(meanTimes.join("+"));
	  ss << QString("\tMax:(%1)").arg(maxTimes.join("+"));

	  return qstring_cast(ss.str());
}


} // namespace cx
