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

#include "cxStreamedTimestampSynchronizer.h"

#include "cxLogger.h"
#include "cxImage.h"

namespace cx
{

class AbsDoubleLess
{
public:
    AbsDoubleLess(double center) : mCenter(center) { };

  bool operator()(const double& d1, const double& d2)
  {
    return fabs(d1 - mCenter) < fabs(d2 - mCenter);
  }

  double mCenter;
};

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

StreamedTimestampSynchronizer::StreamedTimestampSynchronizer() :
    mLastComputedTimestampShift(0),
	mMaxWindowSize(20)
//	mLastTimeStamp(0)
{

}

template<class ITER>
double StreamedTimestampSynchronizer::average(ITER begin, ITER end) const
{
    int size = std::distance(begin, end);
    double sum = 0;
    for (; begin != end; ++begin)
        sum += *begin;
    return sum / size;
}

void StreamedTimestampSynchronizer::syncToCurrentTime(ImagePtr imgMsg)
{
    QDateTime ts = imgMsg->getAcquisitionTime();
	imgMsg->setOriginalAcquisitionTime(ts);
//	CX_LOG_DEBUG() << "GE time: " << ts.toMSecsSinceEpoch() << " diff: " << ts.toMSecsSinceEpoch() - mLastTimeStamp;
//	mLastTimeStamp = ts.toMSecsSinceEpoch();;
    this->addTimestamp(ts);
    ts = ts.addMSecs(this->getShift());
//	CX_LOG_DEBUG() << "CX time: " << ts.toMSecsSinceEpoch() << " shift: " << std::setprecision(17) << this->getShift();

	//TODO: Store 3 time types: Original from scanner, software acquisition time, and modified time created here
    imgMsg->setAcquisitionTime(ts);
}

double StreamedTimestampSynchronizer::getShift() const
{
    if (mDeltaWindow.empty())
        return 0;

    QList<double> window = mDeltaWindow;
    std::sort(window.begin(), window.end(), AbsDoubleLess(mLastComputedTimestampShift));

    double shrinkedWindowSize = double(window.size()) * 0.75;
    int useCount = (int)(shrinkedWindowSize + 0.5);

    mLastComputedTimestampShift = this->average(window.begin(), window.begin()+useCount);
    return mLastComputedTimestampShift;
}


void StreamedTimestampSynchronizer::addTimestamp(QDateTime timestamp)
{
    double delta = timestamp.msecsTo(QDateTime::currentDateTime());
    mDeltaWindow.push_back(delta);
    while (mDeltaWindow.size() > mMaxWindowSize)
        mDeltaWindow.pop_front();
}

void StreamedTimestampSynchronizer::addTimestamp(double timestamp)
{
    this->addTimestamp(QDateTime::fromMSecsSinceEpoch(timestamp));
}


} /* namespace cx */

