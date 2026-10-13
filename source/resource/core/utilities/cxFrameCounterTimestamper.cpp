/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFrameCounterTimestamper.h"
#include <algorithm>
#include <limits>
#include <vector>

namespace cx
{

FrameCounterTimestamper::FrameCounterTimestamper()
{
	this->reset();
}

void FrameCounterTimestamper::reset()
{
	mSamples.clear();
	mFirstFrame = 0;
	mLastFrame = 0;
	mFirstReceived_ms = 0.0;
	mLastReceived_ms = 0.0;
	mLastTimestamp_ms = std::numeric_limits<double>::lowest();
	mPeriod_ms = 0.0;
}

void FrameCounterTimestamper::restart(unsigned long frameNumber, double received_ms)
{
	mSamples.clear();
	mFirstFrame = frameNumber;
	mFirstReceived_ms = received_ms;
	mLastTimestamp_ms = std::numeric_limits<double>::lowest();
	mPeriod_ms = 0.0;
}

double FrameCounterTimestamper::getTimestampOfEarlierFrame(unsigned long frameNumber) const
{
	double framesBefore = (frameNumber < mLastFrame) ? double(mLastFrame - frameNumber) : 0.0;
	return mLastTimestamp_ms - mPeriod_ms * framesBefore;
}

double FrameCounterTimestamper::getTimestamp(unsigned long frameNumber, double received_ms)
{
	double gap_ms = received_ms - mLastReceived_ms;
	bool continues = !mSamples.empty() && (frameNumber >= mLastFrame) && (gap_ms >= 0.0) && (gap_ms < kMaxGap_ms);
	double retval = received_ms;
	if (frameNumber == 0)
	{
		this->restart(frameNumber, received_ms);
	}
	else if (continues && frameNumber == mLastFrame)
	{
		retval = mLastTimestamp_ms; // the same frame again
	}
	else
	{
		if (!continues)
		{
			this->restart(frameNumber, received_ms);
		}
		retval = std::max(this->getFittedTimestamp(frameNumber, received_ms), mLastTimestamp_ms + kMinStep_ms);
	}
	mLastFrame = frameNumber;
	mLastReceived_ms = received_ms;
	mLastTimestamp_ms = retval;
	return retval;
}

double FrameCounterTimestamper::getFittedTimestamp(unsigned long frameNumber, double received_ms)
{
	mSamples.push_back({double(frameNumber - mFirstFrame), received_ms - mFirstReceived_ms});
	while (mSamples.back().received_ms - mSamples.front().received_ms > kWindow_ms)
	{
		mSamples.pop_front();
	}

	double retval = received_ms;
	double period_ms = 0.0;
	double offset_ms = 0.0;
	if (this->fit(period_ms, offset_ms))
	{
		mPeriod_ms = period_ms;
		retval = std::min(received_ms, mFirstReceived_ms + offset_ms + period_ms * mSamples.back().frame);
	}
	return retval;
}

bool FrameCounterTimestamper::fit(double& period_ms, double& offset_ms) const
{
	double span_ms = mSamples.back().received_ms - mSamples.front().received_ms;
	if (int(mSamples.size()) < kMinFitFrames || span_ms < kMinFitSpan_ms)
	{
		return false;
	}

	// least squares slope, then shift the line down to the earliest arrival
	double n = double(mSamples.size());
	double meanFrame = 0.0;
	double meanReceived = 0.0;
	for (const Sample& sample : mSamples)
	{
		meanFrame += sample.frame / n;
		meanReceived += sample.received_ms / n;
	}
	double covariance = 0.0;
	double variance = 0.0;
	for (const Sample& sample : mSamples)
	{
		covariance += (sample.frame - meanFrame) * (sample.received_ms - meanReceived);
		variance += (sample.frame - meanFrame) * (sample.frame - meanFrame);
	}
	period_ms = (variance > 0.0) ? covariance / variance : 0.0;

	std::vector<double> offsets;
	offsets.reserve(mSamples.size());
	for (const Sample& sample : mSamples)
	{
		offsets.push_back(sample.received_ms - period_ms * sample.frame);
	}
	std::vector<double>::iterator earliest = offsets.begin() + int(kEarliestArrivalPercentile * (offsets.size() - 1));
	std::nth_element(offsets.begin(), earliest, offsets.end());
	offset_ms = *earliest;
	return period_ms > 0.0;
}

} // namespace cx
