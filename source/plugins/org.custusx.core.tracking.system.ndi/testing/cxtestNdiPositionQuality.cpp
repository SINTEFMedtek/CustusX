/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include "cxNdiPositionQuality.h"

namespace cxtest
{

namespace
{

cx::NdiToolSample createGoodSample(int markersUsed = -1)
{
	cx::NdiToolSample retval;
	retval.portHandle = 1;
	retval.visible = true;
	retval.errorRMS_mm = 0.1;
	retval.markersUsed = markersUsed;
	return retval;
}

void addSamples(cx::NdiPositionQuality& quality, const cx::NdiToolSample& sample, int count)
{
	for (int i = 0; i < count; ++i)
	{
		quality.update(sample);
	}
}

} // namespace

TEST_CASE("NdiPositionQuality: a good sample has no problems", "[unit][org.custusx.core.tracking.system.ndi]")
{
	CHECK(cx::NdiPositionQuality::evaluate(createGoodSample(), 0.5, 0).isEmpty());
	CHECK(cx::NdiPositionQuality::evaluate(createGoodSample(4), 0.5, 4).isEmpty());
}

TEST_CASE("NdiPositionQuality: out of volume", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::NdiToolSample sample = createGoodSample();
	sample.portStatus = cx::NdiPortStatus::PartiallyInVolume;
	CHECK(cx::NdiPositionQuality::evaluate(sample, 0.5, 0) == "partially out of the measurement volume");

	sample.portStatus = cx::NdiPortStatus::OutOfVolume | cx::NdiPortStatus::PartiallyInVolume;
	CHECK(cx::NdiPositionQuality::evaluate(sample, 0.5, 0) == "out of the measurement volume");
}

TEST_CASE("NdiPositionQuality: RMS error above the limit", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::NdiToolSample sample = createGoodSample();
	sample.errorRMS_mm = 0.75;
	CHECK(cx::NdiPositionQuality::evaluate(sample, 0.5, 0) == "RMS error 0.75 (limit 0.50)");
	CHECK(cx::NdiPositionQuality::evaluate(sample, 1.0, 0).isEmpty());
	CHECK(cx::NdiPositionQuality::evaluate(sample, 0.0, 0).isEmpty());
}

TEST_CASE("NdiPositionQuality: Polaris marker problems", "[unit][org.custusx.core.tracking.system.ndi]")
{
	CHECK(cx::NdiPositionQuality::evaluate(createGoodSample(3), 0.5, 4) == "3 of 4 markers used");
	CHECK(cx::NdiPositionQuality::evaluate(createGoodSample(-1), 0.5, 4).isEmpty());

	cx::NdiToolSample sample = createGoodSample(4);
	sample.toolInfo = cx::NdiToolInfo::BadTransformFit | cx::NdiToolInfo::NotEnoughMarkers;
	CHECK(cx::NdiPositionQuality::evaluate(sample, 0.5, 4) == "bad marker fit, not enough markers");
}

TEST_CASE("NdiPositionQuality: warns after a few bad frames and clears after a few good ones", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::NdiPositionQuality quality(0.5);
	cx::NdiToolSample bad = createGoodSample();
	bad.portStatus = cx::NdiPortStatus::PartiallyInVolume;

	addSamples(quality, bad, cx::NdiPositionQuality::kFramesBeforeWarning - 1);
	CHECK(quality.getWarning().isEmpty());
	quality.update(bad);
	CHECK(quality.getWarning() == "partially out of the measurement volume");

	addSamples(quality, createGoodSample(), cx::NdiPositionQuality::kFramesBeforeClearing - 1);
	CHECK_FALSE(quality.getWarning().isEmpty());
	quality.update(createGoodSample());
	CHECK(quality.getWarning().isEmpty());
}

TEST_CASE("NdiPositionQuality: a single bad frame between good ones gives no warning", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::NdiPositionQuality quality(0.5);
	cx::NdiToolSample bad = createGoodSample();
	bad.errorRMS_mm = 2.0;
	for (int i = 0; i < 10; ++i)
	{
		quality.update(createGoodSample());
		quality.update(bad);
	}
	CHECK(quality.getWarning().isEmpty());
}

TEST_CASE("NdiPositionQuality: learns the number of markers and warns when one is hidden", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::NdiPositionQuality quality(0.5);
	addSamples(quality, createGoodSample(4), 5);
	CHECK(quality.getExpectedMarkers() == 4);
	CHECK(quality.getWarning().isEmpty());

	addSamples(quality, createGoodSample(3), cx::NdiPositionQuality::kFramesBeforeWarning);
	CHECK(quality.getWarning() == "3 of 4 markers used");
}

TEST_CASE("NdiPositionQuality: invisible samples are ignored, reset clears the warning", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::NdiPositionQuality quality(0.5);
	cx::NdiToolSample bad = createGoodSample();
	bad.portStatus = cx::NdiPortStatus::OutOfVolume;
	addSamples(quality, bad, cx::NdiPositionQuality::kFramesBeforeWarning);
	REQUIRE_FALSE(quality.getWarning().isEmpty());

	cx::NdiToolSample invisible;
	addSamples(quality, invisible, 20);
	CHECK_FALSE(quality.getWarning().isEmpty());

	quality.reset();
	CHECK(quality.getWarning().isEmpty());
}

} // namespace cxtest
