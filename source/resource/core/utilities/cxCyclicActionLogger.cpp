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

#include "cxCyclicActionLogger.h"
#include <numeric>
#include <sstream>
#include <QStringList>
#include "cxTypeConversions.h"
#include <cmath>
#include "cxLogger.h"
#include "cxMathUtils.h"

namespace cx
{

CyclicActionLogger::CyclicActionLogger()
{
  mRenderClock.start();
  mIntervalClock.start();
  this->reset();
}

CyclicActionLogger::CyclicActionLogger(QString name)
{
	mName = name;
	mRenderClock.start();
	mIntervalClock.start();
	this->reset();
}

void CyclicActionLogger::reset(int interval)
{
  mIntervalClock.restart();
  mInterval = interval;
  mTiming.clear();
}

void CyclicActionLogger::begin()
{
	this->time("outside");
}

void CyclicActionLogger::time(QString id)
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
double CyclicActionLogger::getFPS()
{
  if (!mIntervalClock.elapsed())
    return -1;
  double numberOfRenderings =  mTiming.empty() ? 0 : mTiming.front().time.size();
  double fps = 1000.0 * numberOfRenderings / mIntervalClock.elapsed();
  return roundAwayFromZero(fps);
}

bool CyclicActionLogger::intervalPassed() const
{
  return mIntervalClock.elapsed() > mInterval;
}

QString CyclicActionLogger::dumpStatistics()
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

QString CyclicActionLogger::dumpStatisticsSmall()
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
			double maxTime = this->getMaxTime(entry->time);
			double meanTime = this->getMeanTime(entry->time);
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

double CyclicActionLogger::getMeanTime(std::vector<double> &time)
{
	return std::accumulate(time.begin(), time.end(), 0)/time.size();
}

double CyclicActionLogger::getMaxTime(std::vector<double> &time)
{
	return *std::max_element(time.begin(), time.end());
}

int CyclicActionLogger::getTime(QString id)
{
	std::vector<Entry>::iterator entry = getTimingVectorIterator(id);
	if(entry == mTiming.end())
	{
		reportWarning("CyclicActionLogger::getTime() unknown id: " + id);
		return 0;
	}
	return getMeanTime(entry->time);
}

std::vector<CyclicActionLogger::Entry>::iterator CyclicActionLogger::getTimingVectorIterator(QString id)
{
	std::vector<Entry>::iterator entry;
	for (entry=mTiming.begin(); entry!=mTiming.end(); ++entry)
	{
		if(entry->id == id)
			break;
	}
	return entry;
}

int CyclicActionLogger::getTotalLoggedTime()
{
	double totalTime = 0;
	std::vector<Entry>::iterator entry;
	for (entry=mTiming.begin(); entry!=mTiming.end(); ++entry)
	{
		if(entry->id != "outside")
		{
			double meanTime = this->getMeanTime(entry->time);
			totalTime += meanTime;
		}
	}
	return totalTime;
}

} // namespace cx
