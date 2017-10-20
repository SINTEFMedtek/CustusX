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

#include "cxProbeDefinitionFromStringMessages.h"
#include "cxtestUtilities.h"
#include "cxTypeConversions.h"

namespace cxtest
{
typedef boost::shared_ptr<class ProbeDefinitionFromStringMessagesTest> ProbeDefinitionFromStringMessagesTestPtr;

class ProbeDefinitionFromStringMessagesTest : public cx::ProbeDefinitionFromStringMessages
{
public:
		cx::SectorInfoPtr getSectorInfo()
		{
				return mSectorInfo;
		}

void initWithValidParameters()
{
	cx::ImagePtr image = cxtest::Utilities::create3DImage();
	this->setImage(image);
	this->parseValue("ProbeType", "1");
	this->parseValue("Origin", "0.0 0.0 0.0");
	this->parseValue("Angles", "0.0 0.0");
	this->parseValue("BouningBox", "0 30 0 50");
	this->parseValue("Depths", "10 30");
	this->parseValue("LinearWidth", "30");
	this->parseValue("SpacingX", "0.5");
	this->parseValue("SpacingY", "0.5");
}

};

QString stringFromDoubleVector(std::vector<double> vec, QString separator = " ")
{
	QString retval;

	for(int i = 0; i < vec.size(); ++i)
	{
		retval += QString("%1").arg(vec[i]);
		if(i != vec.size() - 1)
			retval += separator;
	}
	return retval;
}


TEST_CASE("ProbeDefinitionFromStringMessages init", "[plugins][org.custusx.core.openigtlink3][unit]")
{
    cx::ProbeDefinitionFromStringMessagesPtr probeDefinitionFromStringMessages;
    probeDefinitionFromStringMessages.reset(new cx::ProbeDefinitionFromStringMessages);

    REQUIRE_FALSE(probeDefinitionFromStringMessages->haveValidValues());
    REQUIRE_FALSE(probeDefinitionFromStringMessages->createProbeDefintion("testProbeDefinition"));
}

TEST_CASE("ProbeDefinitionFromStringMessages set/get values", "[plugins][org.custusx.core.openigtlink3][unit]")
{
    ProbeDefinitionFromStringMessagesTestPtr probeDefinitionFromStringMessages;
    probeDefinitionFromStringMessages.reset(new ProbeDefinitionFromStringMessagesTest);

    cx::SectorInfoPtr sectorInfo = probeDefinitionFromStringMessages->getSectorInfo();

    probeDefinitionFromStringMessages->parseValue("ProbeType", "1");
		probeDefinitionFromStringMessages->parseValue("LinearWidth", "10");

    REQUIRE(sectorInfo->mProbeType == 1);
		REQUIRE(cx::similar(sectorInfo->mLinearWidth, 10.0));

    REQUIRE_FALSE(probeDefinitionFromStringMessages->createProbeDefintion("testProbeDefinition"));
}

TEST_CASE("ProbeDefinitionFromStringMessages create ProbeDefinition", "[plugins][org.custusx.core.openigtlink3][unit]")
{
    ProbeDefinitionFromStringMessagesTestPtr probeDefinitionFromStringMessages;
    probeDefinitionFromStringMessages.reset(new ProbeDefinitionFromStringMessagesTest);

//    cx::SectorInfoPtr sectorInfo = probeDefinitionFromStringMessages->getSectorInfo();

		cx::ImagePtr image = Utilities::create3DImage();
		probeDefinitionFromStringMessages->setImage(image);

    probeDefinitionFromStringMessages->parseValue("ProbeType", "1");
		CHECK_FALSE(probeDefinitionFromStringMessages->haveValidValues());
		probeDefinitionFromStringMessages->parseValue("Origin", "0.0 0.0 0.0");
		probeDefinitionFromStringMessages->parseValue("Angles", "0.0 0.0");
		probeDefinitionFromStringMessages->parseValue("BouningBox", "0 30 0 50");
		probeDefinitionFromStringMessages->parseValue("Depths", "10 30");
		probeDefinitionFromStringMessages->parseValue("LinearWidth", "30");
    probeDefinitionFromStringMessages->parseValue("SpacingX", "0.5");
		CHECK_FALSE(probeDefinitionFromStringMessages->haveValidValues());
		probeDefinitionFromStringMessages->parseValue("SpacingY", "0.5");

    REQUIRE(probeDefinitionFromStringMessages->haveValidValues());

    REQUIRE(probeDefinitionFromStringMessages->createProbeDefintion("testProbeDefinition"));
}

TEST_CASE("ProbeDefinitionFromStringMessages require valid parameters", "[plugins][org.custusx.core.openigtlink3][unit]")
{
		ProbeDefinitionFromStringMessagesTestPtr probeDefinitionFromStringMessages;
		probeDefinitionFromStringMessages.reset(new ProbeDefinitionFromStringMessagesTest);

		{
			probeDefinitionFromStringMessages->initWithValidParameters();
			REQUIRE(probeDefinitionFromStringMessages->haveValidValues());
			probeDefinitionFromStringMessages->parseValue("ProbeType", "5");
			INFO("ProbeType: " + string_cast(probeDefinitionFromStringMessages->getSectorInfo()->mProbeType));
			CHECK_FALSE(probeDefinitionFromStringMessages->haveValidValues());
		}
		{
			probeDefinitionFromStringMessages->initWithValidParameters();
			REQUIRE(probeDefinitionFromStringMessages->haveValidValues());
			probeDefinitionFromStringMessages->parseValue("Origin", "0 0");//Should have 3 values
			INFO("Origin: " + stringFromDoubleVector(probeDefinitionFromStringMessages->getSectorInfo()->mOrigin));
			CHECK_FALSE(probeDefinitionFromStringMessages->haveValidValues());
		}
		{
			probeDefinitionFromStringMessages->initWithValidParameters();
			probeDefinitionFromStringMessages->parseValue("Angles", "0");//Should have 2 or 4 values
			INFO("Angles: " + stringFromDoubleVector(probeDefinitionFromStringMessages->getSectorInfo()->mAngles));
			CHECK_FALSE(probeDefinitionFromStringMessages->haveValidValues());
		}
		{
			probeDefinitionFromStringMessages->initWithValidParameters();
			probeDefinitionFromStringMessages->parseValue("BouningBox", "0 0");//Should have 4 or 6 values
			INFO("BouningBox: " + stringFromDoubleVector(probeDefinitionFromStringMessages->getSectorInfo()->mBouningBox));
			CHECK_FALSE(probeDefinitionFromStringMessages->haveValidValues());
		}
		{
			probeDefinitionFromStringMessages->initWithValidParameters();
			probeDefinitionFromStringMessages->parseValue("Depths", "0");//Should have 2 values
			INFO("Depths: " + stringFromDoubleVector(probeDefinitionFromStringMessages->getSectorInfo()->mDepths));
			CHECK_FALSE(probeDefinitionFromStringMessages->haveValidValues());
		}
		{
			probeDefinitionFromStringMessages->initWithValidParameters();
			probeDefinitionFromStringMessages->parseValue("LinearWidth", "1000000");//Only checked to linear probes (ProbeType==2), Should be less than SectorInfo::toolarge (100000)
			INFO("LinearWidth: " + string_cast(probeDefinitionFromStringMessages->getSectorInfo()->mLinearWidth));
			probeDefinitionFromStringMessages->parseValue("ProbeType", "1");
			CHECK(probeDefinitionFromStringMessages->haveValidValues());
			probeDefinitionFromStringMessages->parseValue("ProbeType", "2");
			CHECK_FALSE(probeDefinitionFromStringMessages->haveValidValues());
		}
		{
			probeDefinitionFromStringMessages->initWithValidParameters();
			probeDefinitionFromStringMessages->parseValue("SpacingX", "0");//Should be between 0 and SectorInfo::toolarge (100000)
			INFO("SpacingX: " + string_cast(probeDefinitionFromStringMessages->getSectorInfo()->mSpacingX));
			CHECK_FALSE(probeDefinitionFromStringMessages->haveValidValues());
		}

		{
			probeDefinitionFromStringMessages->initWithValidParameters();
			probeDefinitionFromStringMessages->parseValue("SpacingY", "test");//Should be between 0 and SectorInfo::toolarge (100000)
			INFO("SpacingY: " + string_cast(probeDefinitionFromStringMessages->getSectorInfo()->mSpacingY));
			CHECK_FALSE(probeDefinitionFromStringMessages->haveValidValues());
		}
}

}//cxtest
