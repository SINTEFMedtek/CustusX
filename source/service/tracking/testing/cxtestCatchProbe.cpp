// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "catch.hpp"
#include "cxTestProbeFixture.h"

#include <QString>
#include "sscMessageManager.h"
#include "ProbeXmlConfigParserMock.h"
#include "sscProbeData.h"
#include "sscTestVideoSource.h"


//Disabled for now. Test will output a warning. Use the test below with the Mock XmlParser instead
TEST_CASE_METHOD(cxtest::ProbeFixture, "Probe: Create with default XML parser", "[unit][service][tracking][hide]")
{
	mProbe = cx::cxProbe::New(mProbeName, mScannerName);

	REQUIRE(mProbe);

	{
		INFO("Probe's config id is not empty. It should be since the test probe setup is not present in the config id list");
		CHECK(mProbe->getConfigId().isEmpty());
	}

	{
		INFO("Test probe is valid. It should not be.");
		CHECK(!mProbe->isValid());
	}
}

TEST_CASE_METHOD(cxtest::ProbeFixture, "Probe: Create with mock XML parser", "[unit][service][tracking]")
{
	REQUIRE(mProbe);
	CHECK(!mProbe->getConfigId().isEmpty());
	CHECK(mProbe->getConfigId().compare(mScannerName + " " + mProbeName + " " + mDefaultRtSourceName) == 0);
	CHECK(mProbe->isValid());
}

TEST_CASE_METHOD(cxtest::ProbeFixture, "Probe: Use digital video setting", "[unit][service][tracking]")
{
	CHECK(!mProbe->isUsingDigitalVideo());
	CHECK(mProbe->getRtSourceName().compare(mDefaultRtSourceName) == 0);
	mProbe->useDigitalVideo(true);
	CHECK(mProbe->isUsingDigitalVideo());
	CHECK(mProbe->getRtSourceName().compare("Digital") == 0);
	CHECK(mProbe->getConfigId().compare("Digital") == 0);
}

TEST_CASE_METHOD(cxtest::ProbeFixture, "Probe: Use VideoSource", "[unit][service][tracking]")
{
	CHECK(!mProbe->getRTSource());
	cx::TestVideoSourcePtr videoSource(new cx::TestVideoSource("TestVideoSourceUid", "TestVideoSource" , 80, 40));
	mProbe->setRTSource(videoSource);
	CHECK(mProbe->getRTSource());
	CHECK(mProbe->getRTSource()->getUid().compare("TestVideoSourceUid") == 0);
	CHECK(mProbe->getRTSource()->getName().compare("TestVideoSource") == 0);
}

TEST_CASE_METHOD(cxtest::ProbeFixture, "Probe: Use Default probe sector", "[unit][service][tracking]")
{
	CHECK(mProbe->getSector());
	CHECK(mProbe->getProbeData().getUid().compare(mDefaultProbeDataUid) == 0);
	CHECK(cx::similar(mProbe->getProbeData().getTemporalCalibration(), mDefaultTemporalCalibration));
}

TEST_CASE_METHOD(cxtest::ProbeFixture, "Probe: Use Custom probe sector", "[unit][service][tracking]")
{
	mProbe->setProbeSector(this->createProbeData());
	CHECK(mProbe->getSector(mProbeDataUid));
	CHECK(mProbe->getProbeData(mProbeDataUid).getUid().compare(mProbeDataUid) == 0);
	CHECK(cx::similar(mProbe->getProbeData(mProbeDataUid).getTemporalCalibration(), mTemporalCalibration));
}

TEST_CASE_METHOD(cxtest::ProbeFixture, "Probe: Set active stream to custom probe sector", "[unit][service][tracking]")
{
	mProbe->setProbeSector(this->createProbeData());
	CHECK(mProbe->getProbeData().getUid().compare(mDefaultProbeDataUid) == 0);

	mProbe->setActiveStream(mProbeDataUid);
	CHECK(mProbe->getProbeData().getUid().compare(mProbeDataUid) == 0);
	CHECK(cx::similar(mProbe->getProbeData().getTemporalCalibration(), mTemporalCalibration));

	mProbe->setActiveStream(mDefaultProbeDataUid);
	CHECK(mProbe->getSector());
	CHECK(mProbe->getProbeData().getUid().compare(mDefaultProbeDataUid) == 0);
	CHECK(cx::similar(mProbe->getProbeData().getTemporalCalibration(), mDefaultTemporalCalibration));
}

TEST_CASE_METHOD(cxtest::ProbeFixture, "Probe: Set active stream", "[unit][service][tracking]")
{
	CHECK(mProbe->getActiveStream().compare(mDefaultProbeDataUid) == 0);
	QString streamName = "TestStream";
	mProbe->setActiveStream(streamName);
	CHECK(mProbe->getActiveStream().compare(streamName) == 0);
	CHECK(mProbe->getProbeData().getUid().compare(streamName) == 0);
}

