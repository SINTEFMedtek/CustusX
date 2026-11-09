/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFrameCounterTimestamper.h"
#include <algorithm>
#include <cmath>
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
	mStablePeriod_ms = 0.0;
	mPeriodMismatchSince_ms = -1.0;
}

void FrameCounterTimestamper::restart(unsigned long frameNumber, double received_ms)
{
	mSamples.clear();
	mFirstFrame = frameNumber;
	mFirstReceived_ms = received_ms;
	mLastTimestamp_ms = std::numeric_limits<double>::lowest();
	mPeriod_ms = 0.0;
}

/** The frame arrived much later than its frame number says: the device clock paused, e.g. while the device was reconfigured */
bool FrameCounterTimestamper::isClockPause(unsigned long frameNumber, double gap_ms) const
{
	double period_ms = (mStablePeriod_ms > 0.0) ? mStablePeriod_ms : mPeriod_ms;
	double expectedGap_ms = (frameNumber > mLastFrame) ? period_ms * double(frameNumber - mLastFrame) : 0.0;
	return period_ms > 0.0 && (gap_ms - expectedGap_ms > kMaxClockPause_ms);
}

double FrameCounterTimestamper::getTimestampOfEarlierFrame(unsigned long frameNumber) const
{
	double framesBefore = (frameNumber < mLastFrame) ? double(mLastFrame - frameNumber) : 0.0;
	return mLastTimestamp_ms - mPeriod_ms * framesBefore;
}

double FrameCounterTimestamper::getTimestamp(unsigned long frameNumber, double received_ms)
{
	double gap_ms = received_ms - mLastReceived_ms;
	bool continues = !mSamples.empty() && (frameNumber >= mLastFrame) && (gap_ms >= 0.0) && (gap_ms < kMaxGap_ms)
			&& !this->isClockPause(frameNumber, gap_ms);
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

/**
 * The slope along the earliest arrivals: the lower convex hull at the median frame. Late arrivals, e.g. a burst of frames
 * that were held back, are above the hull and don't change it. The hull is found again without the earliest
 * arrivals, so that a few frames arriving too early don't tilt it.
 */
double FrameCounterTimestamper::getLowerHullSlope() const
{
	std::vector<const Sample*> all;
	for (const Sample& sample : mSamples)
	{
		all.push_back(&sample);
	}
	double slope = getLowerHullSlope(all, mSamples[mSamples.size() / 2].frame);

	double earliestOffset = this->getOffsetPercentile(slope, kEarliestArrivalPercentile);
	std::vector<const Sample*> withoutEarliest;
	for (const Sample* sample : all)
	{
		if (sample->received_ms - slope * sample->frame >= earliestOffset)
		{
			withoutEarliest.push_back(sample);
		}
	}
	return getLowerHullSlope(withoutEarliest, mSamples[mSamples.size() / 2].frame);
}

double FrameCounterTimestamper::getLowerHullSlope(const std::vector<const Sample*>& samples, double atFrame)
{
	std::vector<const Sample*> hull;
	for (const Sample* sample : samples)
	{
		while (hull.size() >= 2 && isBelowOrOnLine(*hull[hull.size() - 2], *hull.back(), *sample))
		{
			hull.pop_back();
		}
		hull.push_back(sample);
	}

	double retval = 0.0;
	for (unsigned i = 1; i < hull.size(); ++i)
	{
		double dFrame = hull[i]->frame - hull[i - 1]->frame;
		if (dFrame > 0.0 && hull[i - 1]->frame <= atFrame && atFrame <= hull[i]->frame)
		{
			retval = (hull[i]->received_ms - hull[i - 1]->received_ms) / dFrame;
		}
	}
	return retval;
}

/** True if c makes b part of a line that is not convex from below: b is above or on the line from a to c */
bool FrameCounterTimestamper::isBelowOrOnLine(const Sample& a, const Sample& b, const Sample& c)
{
	double cross = (b.frame - a.frame) * (c.received_ms - a.received_ms) - (b.received_ms - a.received_ms) * (c.frame - a.frame);
	return cross <= 0.0;
}

bool FrameCounterTimestamper::fit(double& period_ms, double& offset_ms)
{
	double span_ms = mSamples.back().received_ms - mSamples.front().received_ms;
	bool enoughData = int(mSamples.size()) >= kMinFitFrames && (mStablePeriod_ms > 0.0 || span_ms >= kMinFitSpan_ms);
	if (enoughData)
	{
		double hullSlope = this->getLowerHullSlope();
		period_ms = (mStablePeriod_ms > 0.0) ? mStablePeriod_ms : hullSlope;
		offset_ms = this->getOffsetPercentile(period_ms, kEarliestArrivalPercentile);
		this->updateStablePeriod(hullSlope, span_ms);
	}
	return enoughData && period_ms > 0.0;
}

/**
 * The frame period of a device is very stable. Once measured on a window without queued frames, it is kept, and only
 * follows small changes. A queue or burst of late frames can then delay frames, but can't tilt the line.
 */
void FrameCounterTimestamper::updateStablePeriod(double hullSlope, double span_ms)
{
	double now_ms = mFirstReceived_ms + mSamples.back().received_ms;
	if (mStablePeriod_ms > 0.0)
	{
		if (std::abs(hullSlope - mStablePeriod_ms) < kMaxPeriodChange * mStablePeriod_ms)
		{
			mStablePeriod_ms += kPeriodUpdateWeight * (hullSlope - mStablePeriod_ms);
			mPeriodMismatchSince_ms = -1.0;
		}
		else if (mPeriodMismatchSince_ms < 0.0)
		{
			mPeriodMismatchSince_ms = now_ms;
		}
		else if (now_ms - mPeriodMismatchSince_ms > kPeriodMismatchBeforeRemeasure_ms)
		{
			// Longer than any queue: the period has changed, measure it again
			mStablePeriod_ms = 0.0;
			mPeriodMismatchSince_ms = -1.0;
		}
	}
	else if (span_ms >= kStablePeriodSpan_ms && hullSlope > 0.0)
	{
		double spread_ms = this->getOffsetPercentile(hullSlope, 0.5) - this->getOffsetPercentile(hullSlope, kEarliestArrivalPercentile);
		if (spread_ms < kMaxSpreadForStablePeriod_ms)
		{
			mStablePeriod_ms = hullSlope;
		}
	}
}

/** The percentile of (received - period * frame) over the window: the offset of the line through those arrivals */
double FrameCounterTimestamper::getOffsetPercentile(double period_ms, double percentile) const
{
	std::vector<double> offsets;
	offsets.reserve(mSamples.size());
	for (const Sample& sample : mSamples)
	{
		offsets.push_back(sample.received_ms - period_ms * sample.frame);
	}
	std::vector<double>::iterator selected = offsets.begin() + int(percentile * (offsets.size() - 1));
	std::nth_element(offsets.begin(), selected, offsets.end());
	return *selected;
}

} // namespace cx
