/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxImageStreamerOpenCV.h"

namespace cxtest
{

TEST_CASE("ImageStreamerOpenCV: uses the driver's frame time when it is plausible", "[unit]")
{
	double grabTime_ms = 1791535286214.0;
	double driverClockToSystemClock_ms = 1791535000000.0;
	double driverTime_ms = grabTime_ms - driverClockToSystemClock_ms - 40.0;
	CHECK(cx::ImageStreamerOpenCV::getFrameTime_ms(grabTime_ms, driverTime_ms, driverClockToSystemClock_ms) == Approx(grabTime_ms - 40.0));
}

TEST_CASE("ImageStreamerOpenCV: uses the grab time when the driver's frame time is missing or not plausible", "[unit]")
{
	double grabTime_ms = 1791535286214.0;
	double driverClockToSystemClock_ms = 1791535000000.0;
	double inTheFuture_ms = grabTime_ms - driverClockToSystemClock_ms + 5.0;
	double tooOld_ms = grabTime_ms - driverClockToSystemClock_ms - 2 * cx::ImageStreamerOpenCV::kMaxCaptureDelay_ms;
	CHECK(cx::ImageStreamerOpenCV::getFrameTime_ms(grabTime_ms, 0.0, driverClockToSystemClock_ms) == Approx(grabTime_ms));
	CHECK(cx::ImageStreamerOpenCV::getFrameTime_ms(grabTime_ms, inTheFuture_ms, driverClockToSystemClock_ms) == Approx(grabTime_ms));
	CHECK(cx::ImageStreamerOpenCV::getFrameTime_ms(grabTime_ms, tooOld_ms, driverClockToSystemClock_ms) == Approx(grabTime_ms));
}

} // namespace cxtest
