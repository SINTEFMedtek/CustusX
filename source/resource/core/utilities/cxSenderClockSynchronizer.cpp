/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxSenderClockSynchronizer.h"
#include <limits>

namespace cx
{

SenderClockSynchronizer::SenderClockSynchronizer() :
	mResyncCount(0)
{
	this->reset();
}

void SenderClockSynchronizer::reset()
{
	mDifferences.clear();
	mLastSent_ms = std::numeric_limits<double>::lowest();
	mLateSince_ms = -1.0;
	mUsedReceiveTime = false;
}

double SenderClockSynchronizer::getTimestamp(double sent_ms, double received_ms)
{
	double retval = received_ms;
	if (sent_ms > 0.0)
	{
		retval = this->getSynchronizedTimestamp(sent_ms, received_ms);
	}
	return retval;
}

double SenderClockSynchronizer::getSynchronizedTimestamp(double sent_ms, double received_ms)
{
	double retval = received_ms;
	bool late = this->isLate(sent_ms, received_ms);
	if (late && mLateSince_ms < 0.0)
	{
		mLateSince_ms = received_ms;
	}
	if (late && received_ms - mLateSince_ms >= kResyncAfter_ms)
	{
		this->reset();
		mResyncCount++;
		late = false;
	}

	mUsedReceiveTime = late;
	if (!late)
	{
		mLateSince_ms = -1.0;
		mLastSent_ms = sent_ms;
		this->addDifference(received_ms, received_ms - sent_ms);
		retval = sent_ms + this->getOffset_ms();
	}
	return retval;
}

bool SenderClockSynchronizer::isLate(double sent_ms, double received_ms) const
{
	bool retval = false;
	if (this->hasOffset())
	{
		bool notIncreasing = sent_ms <= mLastSent_ms;
		bool delayed = received_ms - sent_ms - this->getOffset_ms() > kResyncLimit_ms;
		retval = notIncreasing || delayed;
	}
	return retval;
}

void SenderClockSynchronizer::addDifference(double received_ms, double difference_ms)
{
	while (!mDifferences.empty() && mDifferences.back().difference_ms >= difference_ms)
	{
		mDifferences.pop_back();
	}
	mDifferences.push_back({received_ms, difference_ms});
	while (received_ms - mDifferences.front().received_ms > kWindow_ms)
	{
		mDifferences.pop_front();
	}
}

bool SenderClockSynchronizer::hasOffset() const
{
	return !mDifferences.empty();
}

double SenderClockSynchronizer::getOffset_ms() const
{
	return this->hasOffset() ? mDifferences.front().difference_ms : 0.0;
}

bool SenderClockSynchronizer::usedReceiveTime() const
{
	return mUsedReceiveTime;
}

int SenderClockSynchronizer::getResyncCount() const
{
	return mResyncCount;
}

} // namespace cx
