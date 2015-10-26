/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#include "catch.hpp"
#include "cxtestProbeFixture.h"

#include <QString>

#include "ProbeXmlConfigParserMock.h"
#include "cxProbeDefinition.h"
#include "cxTestVideoSource.h"


//Disabled for now. Test will output a warning. Use the test below with the Mock XmlParser instead
TEST_CASE_METHOD(cxtest::ProbeFixture, "Probe: Create with default XML parser", "[unit][service][tracking][hide][org.custus.core.tracking]")
{
	mProbe = cx::ProbeImpl::New(mProbeName, mScannerName);

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

TEST_CASE_METHOD(cxtest::ProbeFixture, "Probe: Create with mock XML parser", "[unit][service][tracking][org.custus.core.tracking]")
{
	REQUIRE(mProbe);
	INFO(mProbe->getConfigId().toStdString());
	CHECK(!mProbe->getConfigId().isEmpty());
	CHECK(mProbe->getConfigId().compare(mScannerName + " " + mProbeName + " " + mDefaultRtSourceName) == 0);
	CHECK(mProbe->isValid());
}

TEST_CASE_METHOD(cxtest::ProbeFixture, "Probe: Use digital video setting", "[unit][service][tracking][org.custus.core.tracking]")
{
	CHECK(!mProbe->getProbeDefinition().getUseDigitalVideo());
	INFO(mProbe->getRtSourceName().toStdString());
	CHECK(mProbe->getRtSourceName().compare(mDefaultRtSourceName) == 0);
	cx::ProbeDefinition data = mProbe->getProbeDefinition();
	data.setUseDigitalVideo(true);
	mProbe->setProbeDefinition(data);
	CHECK(mProbe->getProbeDefinition().getUseDigitalVideo());
//	CHECK(mProbe->getRtSourceName().compare("Digital") == 0);
	CHECK(mProbe->getConfigId().compare("Digital") == 0);
}

TEST_CASE_METHOD(cxtest::ProbeFixture, "Probe: Use digital video setting with no RT source", "[unit][service][tracking][org.custus.core.tracking]")
{
	testProbeWithNoRTSource();
	CHECK(!mProbe->getProbeDefinition().getUseDigitalVideo());
	INFO(mProbe->getRtSourceName().toStdString());
	CHECK(mProbe->getRtSourceName().compare(mDefaultRtSourceName) == 0);
	cx::ProbeDefinition data = mProbe->getProbeDefinition();
	data.setUseDigitalVideo(true);
	mProbe->setProbeDefinition(data);
	CHECK(mProbe->getProbeDefinition().getUseDigitalVideo());
//	CHECK(mProbe->getRtSourceName().compare("Digital") == 0);
	CHECK(mProbe->getConfigId().compare("Digital") == 0);
}

TEST_CASE_METHOD(cxtest::ProbeFixture, "Probe: Use VideoSource", "[unit][service][tracking][org.custus.core.tracking]")
{
	CHECK(!mProbe->getRTSource());
	cx::TestVideoSourcePtr videoSource(new cx::TestVideoSource("TestVideoSourceUid", "TestVideoSource" , 80, 40));
	mProbe->setRTSource(videoSource);
	CHECK(mProbe->getRTSource());
	CHECK(mProbe->getRTSource()->getUid().compare("TestVideoSourceUid") == 0);
	CHECK(mProbe->getRTSource()->getName().compare("TestVideoSource") == 0);
}

TEST_CASE_METHOD(cxtest::ProbeFixture, "Probe: Use Default probe sector", "[unit][service][tracking][org.custus.core.tracking]")
{
	CHECK(mProbe->getSector());
	CHECK(mProbe->getProbeDefinition().getUid().compare(mDefaultProbeDefinitionUid) == 0);
	CHECK(cx::similar(mProbe->getProbeDefinition().getTemporalCalibration(), mDefaultTemporalCalibration));
}

TEST_CASE_METHOD(cxtest::ProbeFixture, "Probe: Use Custom probe sector", "[unit][service][tracking][org.custus.core.tracking]")
{
	mProbe->setProbeDefinition(this->createProbeDefinition());
	CHECK(mProbe->getSector(mProbeDefinitionUid));
	CHECK(mProbe->getProbeDefinition(mProbeDefinitionUid).getUid().compare(mProbeDefinitionUid) == 0);
	CHECK(cx::similar(mProbe->getProbeDefinition(mProbeDefinitionUid).getTemporalCalibration(), mTemporalCalibration));
}

TEST_CASE_METHOD(cxtest::ProbeFixture, "Probe: Set active stream to custom probe sector", "[unit][service][tracking][org.custus.core.tracking]")
{
	mProbe->setProbeDefinition(this->createProbeDefinition());
	CHECK(mProbe->getProbeDefinition().getUid().compare(mDefaultProbeDefinitionUid) == 0);

	mProbe->setActiveStream(mProbeDefinitionUid);
	CHECK(mProbe->getProbeDefinition().getUid().compare(mProbeDefinitionUid) == 0);
	CHECK(cx::similar(mProbe->getProbeDefinition().getTemporalCalibration(), mTemporalCalibration));

	mProbe->setActiveStream(mDefaultProbeDefinitionUid);
	CHECK(mProbe->getSector());
	CHECK(mProbe->getProbeDefinition().getUid().compare(mDefaultProbeDefinitionUid) == 0);
	CHECK(cx::similar(mProbe->getProbeDefinition().getTemporalCalibration(), mDefaultTemporalCalibration));
}

TEST_CASE_METHOD(cxtest::ProbeFixture, "Probe: Set active stream", "[unit][service][tracking][org.custus.core.tracking]")
{
	CHECK(mProbe->getActiveStream().compare(mDefaultProbeDefinitionUid) == 0);
	QString streamName = "TestStream";
	mProbe->setActiveStream(streamName);
	CHECK(mProbe->getActiveStream().compare(streamName) == 0);
	CHECK(mProbe->getProbeDefinition().getUid().compare(streamName) == 0);
}

