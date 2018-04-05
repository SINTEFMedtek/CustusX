/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
    this->addTimestamp(ts);
    ts = ts.addMSecs(this->getShift());

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

