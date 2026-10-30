/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXFRAMECOUNTERTIMESTAMPER_H_
#define CXFRAMECOUNTERTIMESTAMPER_H_

#include "cxResourceExport.h"
#include <deque>
#include <vector>

namespace cx
{

/** \brief Timestamps device frames from the device's frame counter instead of the time they were read.
 *
 * Model: received = offset + period * frame + delay, where the delay is 0 or more and short for most frames.
 * Fits the line along the earliest reads in a 10 s window: the frame period from the lower convex hull,
 * kept once measured, and the offset from the 5th percentile. This removes the read jitter, and late or
 * queued frames don't tilt the line. The device's own latency is not subtracted.
 *
 * \sa \ref dev_acquisition_timestamps for the whole algorithm and where it is used.
 *
 * \ingroup cx_resource_core_utilities
 */
class cxResource_EXPORT FrameCounterTimestamper
{
public:
	static constexpr double kWindow_ms = 10000.0; ///< Frames used in the fit
	static constexpr double kMinFitSpan_ms = 500.0;
	static const int kMinFitFrames = 10;
	static constexpr double kMaxGap_ms = 1000.0; ///< A longer pause, a step back of the clock, a clock pause or a decreasing frame number restarts the fit
	static constexpr double kMinStep_ms = 0.01; ///< Timestamps are kept increasing
	static constexpr double kEarliestArrivalPercentile = 0.05; ///< The arrivals that give the offset
	static constexpr double kStablePeriodSpan_ms = 5000.0; ///< Window needed to measure the stable frame period
	static constexpr double kMaxSpreadForStablePeriod_ms = 30.0; ///< Median minus earliest arrival delay, larger means queued frames
	static constexpr double kMaxPeriodChange = 0.005; ///< Relative change of the period that the stable period follows
	static constexpr double kPeriodUpdateWeight = 0.05;
	static constexpr double kPeriodMismatchBeforeRemeasure_ms = 20000.0; ///< A different period for this long is measured again
	static constexpr double kMaxClockPause_ms = 200.0; ///< A frame this much later than its frame number says restarts the fit

	FrameCounterTimestamper();
	void reset();
	double getTimestamp(unsigned long frameNumber, double received_ms); ///< \return the timestamp of the frame, normally not later than received_ms
	double getTimestampOfEarlierFrame(unsigned long frameNumber) const; ///< For a frame before the last one, e.g. a tool not updated in the last frame

private:
	struct Sample
	{
		double frame; ///< relative to mFirstFrame
		double received_ms; ///< relative to mFirstReceived_ms
	};
	void restart(unsigned long frameNumber, double received_ms);
	double getFittedTimestamp(unsigned long frameNumber, double received_ms);
	bool isClockPause(unsigned long frameNumber, double gap_ms) const;
	bool fit(double& period_ms, double& offset_ms); ///< received = offset + period * frame, along the earliest arrivals
	void updateStablePeriod(double hullSlope, double span_ms);
	double getOffsetPercentile(double period_ms, double percentile) const;
	double getLowerHullSlope() const;
	static double getLowerHullSlope(const std::vector<const Sample*>& samples, double atFrame);
	static bool isBelowOrOnLine(const Sample& a, const Sample& b, const Sample& c);

	std::deque<Sample> mSamples;
	unsigned long mFirstFrame;
	unsigned long mLastFrame;
	double mFirstReceived_ms;
	double mLastReceived_ms;
	double mLastTimestamp_ms;
	double mPeriod_ms; ///< From the last fit, 0 if none
	double mStablePeriod_ms; ///< Measured once, kept after a restart of the fit. 0 if not measured yet
	double mPeriodMismatchSince_ms; ///< When the measured period started to differ from mStablePeriod_ms, -1 if it doesn't
};

} // namespace cx

#endif /* CXFRAMECOUNTERTIMESTAMPER_H_ */
