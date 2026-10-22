/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXNDIFRAMERATEESTIMATOR_H_
#define CXNDIFRAMERATEESTIMATOR_H_

#include "org_custusx_core_tracking_system_ndi_Export.h"

namespace cx
{

/** \brief Estimates the device's frame interval, to poll just before the next frame.
 *
 * The polling strategy of the first NDI plugin (CustusX#31): poll quickly until a
 * new frame arrives, then wait until shortly before the next frame is expected.
 * This gives low latency without polling the device continuously.
 *
 * \ingroup org_custusx_core_tracking_system_ndi
 */
class org_custusx_core_tracking_system_ndi_EXPORT NdiFrameRateEstimator
{
public:
	static constexpr double kInitialInterval_ms = 50.0;
	static constexpr double kMinInterval_ms = 10.0;
	static constexpr double kMaxInterval_ms = 200.0;
	static constexpr double kIgnoreIntervalsAbove_ms = 500.0; ///< e.g. after the tools were out of view
	static constexpr double kLookAhead_ms = 5.0;
	static const int kFastPollInterval_ms = 2;

	NdiFrameRateEstimator() : mEstimatedInterval_ms(kInitialInterval_ms), mLastFrameTime_ms(0.0) {}

	void reset()
	{
		mEstimatedInterval_ms = kInitialInterval_ms;
		mLastFrameTime_ms = 0.0;
	}

	/** Call when a new frame arrives. */
	void addFrame(double time_ms)
	{
		if (mLastFrameTime_ms > 0.0)
		{
			double interval = time_ms - mLastFrameTime_ms;
			if (interval > 0.0 && interval < kIgnoreIntervalsAbove_ms)
			{
				double estimate = 0.8 * mEstimatedInterval_ms + 0.2 * interval;
				mEstimatedInterval_ms = (estimate < kMinInterval_ms) ? kMinInterval_ms : ((estimate > kMaxInterval_ms) ? kMaxInterval_ms : estimate);
			}
		}
		mLastFrameTime_ms = time_ms;
	}

	/** How long to wait before polling again, or 0 to start fast polling now. */
	int getWaitBeforeNextPoll_ms(double now_ms) const
	{
		double wait = mLastFrameTime_ms + mEstimatedInterval_ms - now_ms - kLookAhead_ms;
		return (wait > 1.0) ? static_cast<int>(wait) : 0;
	}

	double getEstimatedInterval_ms() const { return mEstimatedInterval_ms; }

private:
	double mEstimatedInterval_ms;
	double mLastFrameTime_ms;
};

} // namespace cx

#endif /* CXNDIFRAMERATEESTIMATOR_H_ */
