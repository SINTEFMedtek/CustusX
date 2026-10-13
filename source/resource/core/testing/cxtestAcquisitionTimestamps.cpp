/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include <cmath>
#include "cxFrameCounterTimestamper.h"
#include "cxSenderClockSynchronizer.h"
#include "cxTime.h"
#include <QDateTime>

namespace cxtest
{

namespace
{
const double kStart_ms = 1791535286214.0;
const double kPeriod_ms = 25.0;

/** Extra delay of frame i: repeats 0..14 ms, so every 15th frame arrives with no extra delay */
double jitter_ms(int i)
{
	return double((i * 7) % 15);
}
}

TEST_CASE("FrameCounterTimestamper: uses the receive time until the fit has enough frames", "[unit]")
{
	cx::FrameCounterTimestamper timestamper;
	CHECK(timestamper.getTimestamp(100, kStart_ms + 3) == Approx(kStart_ms + 3));
	CHECK(timestamper.getTimestamp(0, kStart_ms + 30) == Approx(kStart_ms + 30));
}

TEST_CASE("FrameCounterTimestamper: removes jitter and late polls", "[unit]")
{
	cx::FrameCounterTimestamper timestamper;
	unsigned long frameStep = 8;
	double latency_ms = 4.0;
	double timestamp_ms = 0.0;
	double device_ms = 0.0;
	for (int i = 0; i < 200; ++i)
	{
		device_ms = kStart_ms + i * kPeriod_ms;
		timestamp_ms = timestamper.getTimestamp(1000 + i * frameStep, device_ms + latency_ms + jitter_ms(i));
	}
	CHECK(std::abs(timestamp_ms - (device_ms + latency_ms)) < 0.5);

	double lateFrame_ms = device_ms + kPeriod_ms;
	double lateTimestamp_ms = timestamper.getTimestamp(1000 + 200 * frameStep, lateFrame_ms + latency_ms + 20.0);
	CHECK(std::abs(lateTimestamp_ms - (lateFrame_ms + latency_ms)) < 0.5);
}

TEST_CASE("FrameCounterTimestamper: a few frames arriving too early don't shift the timestamps", "[unit]")
{
	cx::FrameCounterTimestamper timestamper;
	double latency_ms = 4.0;
	double timestamp_ms = 0.0;
	double device_ms = 0.0;
	for (int i = 0; i < 200; ++i)
	{
		device_ms = kStart_ms + i * kPeriod_ms;
		double early_ms = (i < 3) ? 7.5 : 0.0;
		timestamp_ms = timestamper.getTimestamp(1000 + i, device_ms + latency_ms + jitter_ms(i) - early_ms);
	}
	CHECK(std::abs(timestamp_ms - (device_ms + latency_ms)) < 0.5);
}

TEST_CASE("FrameCounterTimestamper: restarts the fit when the frame number is not increasing", "[unit]")
{
	cx::FrameCounterTimestamper timestamper;
	for (int i = 0; i < 100; ++i)
	{
		timestamper.getTimestamp(1000 + i, kStart_ms + i * kPeriod_ms);
	}
	double received_ms = kStart_ms + 101 * kPeriod_ms + 7;
	CHECK(timestamper.getTimestamp(5, received_ms) == Approx(received_ms));
}

TEST_CASE("FrameCounterTimestamper: a repeated frame gets the same timestamp, and the fit continues", "[unit]")
{
	cx::FrameCounterTimestamper timestamper;
	double timestamp_ms = 0.0;
	for (int i = 0; i < 100; ++i)
	{
		timestamp_ms = timestamper.getTimestamp(1000 + i, kStart_ms + i * kPeriod_ms + 2.0);
	}
	CHECK(timestamper.getTimestamp(1099, kStart_ms + 99 * kPeriod_ms + 9.0) == Approx(timestamp_ms));
	double next_ms = timestamper.getTimestamp(1100, kStart_ms + 100 * kPeriod_ms + 12.0);
	CHECK(std::abs(next_ms - (kStart_ms + 100 * kPeriod_ms + 2.0)) < 0.5);
}

TEST_CASE("FrameCounterTimestamper: follows the clock when it is set back", "[unit]")
{
	cx::FrameCounterTimestamper timestamper;
	for (int i = 0; i < 100; ++i)
	{
		timestamper.getTimestamp(1000 + i, kStart_ms + i * kPeriod_ms);
	}
	double received_ms = kStart_ms - 3600000.0;
	CHECK(timestamper.getTimestamp(1100, received_ms) == Approx(received_ms));
}

TEST_CASE("FrameCounterTimestamper: an earlier frame is timestamped one period back per frame", "[unit]")
{
	cx::FrameCounterTimestamper timestamper;
	double timestamp_ms = 0.0;
	for (int i = 0; i < 100; ++i)
	{
		timestamp_ms = timestamper.getTimestamp(1000 + i * 8, kStart_ms + i * kPeriod_ms);
	}
	CHECK(std::abs(timestamper.getTimestampOfEarlierFrame(1000 + 98 * 8) - (timestamp_ms - kPeriod_ms)) < 0.01);
	CHECK(timestamper.getTimestampOfEarlierFrame(1000 + 99 * 8) == Approx(timestamp_ms));
}

TEST_CASE("FrameCounterTimestamper: timestamps are increasing and never later than received", "[unit]")
{
	cx::FrameCounterTimestamper timestamper;
	double previous_ms = 0.0;
	for (int i = 0; i < 300; ++i)
	{
		double received_ms = kStart_ms + i * kPeriod_ms + jitter_ms(i) * 3.0;
		double timestamp_ms = timestamper.getTimestamp(1 + i, received_ms);
		CHECK(timestamp_ms > previous_ms);
		CHECK(timestamp_ms <= received_ms);
		previous_ms = timestamp_ms;
	}
}

TEST_CASE("SenderClockSynchronizer: uses the offset of the fastest message", "[unit]")
{
	cx::SenderClockSynchronizer synchronizer;
	double clockOffset_ms = -5000.0;
	double latency_ms = 3.0;
	double timestamp_ms = 0.0;
	double sent_ms = 0.0;
	for (int i = 0; i < 100; ++i)
	{
		sent_ms = kStart_ms + clockOffset_ms + i * kPeriod_ms;
		timestamp_ms = synchronizer.getTimestamp(sent_ms, sent_ms - clockOffset_ms + latency_ms + jitter_ms(i));
	}
	CHECK(synchronizer.getOffset_ms() == Approx(latency_ms - clockOffset_ms));
	CHECK(timestamp_ms == Approx(sent_ms - clockOffset_ms + latency_ms));
	CHECK_FALSE(synchronizer.usedReceiveTime());
}

TEST_CASE("SenderClockSynchronizer: forgets the fastest message after the window", "[unit]")
{
	cx::SenderClockSynchronizer synchronizer;
	synchronizer.getTimestamp(kStart_ms, kStart_ms + 1.0);
	double sent_ms = 0.0;
	for (int i = 1; sent_ms < kStart_ms + cx::SenderClockSynchronizer::kWindow_ms + 1000; ++i)
	{
		sent_ms = kStart_ms + i * kPeriod_ms;
		synchronizer.getTimestamp(sent_ms, sent_ms + 10.0);
	}
	CHECK(synchronizer.getOffset_ms() == Approx(10.0));
}

TEST_CASE("SenderClockSynchronizer: uses the receive time while the sender clock is frozen, then resyncs", "[unit]")
{
	cx::SenderClockSynchronizer synchronizer;
	double sent_ms = kStart_ms;
	for (int i = 0; i < 10; ++i)
	{
		sent_ms = kStart_ms + i * kPeriod_ms;
		synchronizer.getTimestamp(sent_ms, sent_ms + 2.0);
	}

	double frozen_ms = sent_ms;
	double received_ms = sent_ms;
	for (int i = 1; received_ms < frozen_ms + 1500; ++i)
	{
		received_ms = frozen_ms + i * kPeriod_ms;
		CHECK(synchronizer.getTimestamp(frozen_ms, received_ms) == Approx(received_ms));
		CHECK(synchronizer.usedReceiveTime());
	}

	// the clock runs again, but behind by the time it was frozen
	double behind_ms = received_ms - frozen_ms;
	for (int i = 1; synchronizer.usedReceiveTime(); ++i)
	{
		received_ms = frozen_ms + behind_ms + i * kPeriod_ms;
		synchronizer.getTimestamp(frozen_ms + i * kPeriod_ms, received_ms);
	}
	CHECK(synchronizer.getResyncCount() == 1);
	CHECK(synchronizer.getOffset_ms() == Approx(behind_ms));
	double resyncedAfter_ms = received_ms - frozen_ms;
	CHECK(resyncedAfter_ms < 2 * cx::SenderClockSynchronizer::kResyncAfter_ms);
}

TEST_CASE("SenderClockSynchronizer: a sender time of 0 gives the receive time without changing the offset", "[unit]")
{
	cx::SenderClockSynchronizer synchronizer;
	CHECK(synchronizer.getTimestamp(0.0, kStart_ms) == Approx(kStart_ms));
	CHECK_FALSE(synchronizer.hasOffset());

	synchronizer.getTimestamp(kStart_ms, kStart_ms + 2.0);
	CHECK(synchronizer.getTimestamp(0.0, kStart_ms + 30.0) == Approx(kStart_ms + 30.0));
	CHECK(synchronizer.getOffset_ms() == Approx(2.0));
	CHECK_FALSE(synchronizer.usedReceiveTime());
}

TEST_CASE("SenderClockSynchronizer: a sender clock jumping ahead is used at once", "[unit]")
{
	cx::SenderClockSynchronizer synchronizer;
	for (int i = 0; i < 10; ++i)
	{
		synchronizer.getTimestamp(kStart_ms + i * kPeriod_ms, kStart_ms + i * kPeriod_ms + 2.0);
	}
	double received_ms = kStart_ms + 10 * kPeriod_ms + 2.0;
	CHECK(synchronizer.getTimestamp(received_ms + 3000, received_ms) == Approx(received_ms));
	CHECK(synchronizer.getOffset_ms() == Approx(-3000));
}

TEST_CASE("getMilliSecondsSinceEpoch: follows QDateTime, with sub-millisecond resolution", "[unit]")
{
	double qtTime_ms = double(QDateTime::currentMSecsSinceEpoch());
	CHECK(std::abs(cx::getMilliSecondsSinceEpoch() - qtTime_ms) < 2.0);

	bool fractional = false;
	for (int i = 0; i < 100 && !fractional; ++i)
	{
		double time_ms = cx::getMilliSecondsSinceEpoch();
		fractional = time_ms != std::floor(time_ms);
	}
	CHECK(fractional);
}

} // namespace cxtest
