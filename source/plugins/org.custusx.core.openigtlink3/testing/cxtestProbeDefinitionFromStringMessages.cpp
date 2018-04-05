/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "catch.hpp"

#include "cxProbeDefinitionFromStringMessages.cpp"
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
	this->parseValue(KEY_PROBE_TYPE, "1");
	this->parseValue(KEY_ORIGIN, "0.0 0.0 0.0");
	this->parseValue(KEY_ANGLES, "0.0 0.0");
	this->parseValue(KEY_BOUNDING_BOX, "0 30 0 50");
	this->parseValue(KEY_DEPTHS, "10 30");
	this->parseValue(KEY_LINEAR_WIDTH, "30");
	this->parseValue(KEY_SPACING_X, "0.5");
	this->parseValue(KEY_SPACING_Y, "0.5");
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


TEST_CASE("ProbeDefinitionFromStringMessages init", "[plugins][org.custusx.core.openigtlink3][plus][unit]")
{
    cx::ProbeDefinitionFromStringMessagesPtr probeDefinitionFromStringMessages;
    probeDefinitionFromStringMessages.reset(new cx::ProbeDefinitionFromStringMessages);

    REQUIRE_FALSE(probeDefinitionFromStringMessages->haveValidValues());
    REQUIRE_FALSE(probeDefinitionFromStringMessages->createProbeDefintion("testProbeDefinition"));
}

TEST_CASE("ProbeDefinitionFromStringMessages set/get values", "[plugins][org.custusx.core.openigtlink3][plus][unit]")
{
    ProbeDefinitionFromStringMessagesTestPtr probeDefinitionFromStringMessages;
    probeDefinitionFromStringMessages.reset(new ProbeDefinitionFromStringMessagesTest);

    cx::SectorInfoPtr sectorInfo = probeDefinitionFromStringMessages->getSectorInfo();

		probeDefinitionFromStringMessages->parseValue(KEY_PROBE_TYPE, "1");
		probeDefinitionFromStringMessages->parseValue(KEY_LINEAR_WIDTH, "10");

    REQUIRE(sectorInfo->mProbeType == 1);
		REQUIRE(cx::similar(sectorInfo->mLinearWidth, 10.0));

    REQUIRE_FALSE(probeDefinitionFromStringMessages->createProbeDefintion("testProbeDefinition"));
}

TEST_CASE("ProbeDefinitionFromStringMessages create ProbeDefinition", "[plugins][org.custusx.core.openigtlink3][plus][unit]")
{
    ProbeDefinitionFromStringMessagesTestPtr probeDefinitionFromStringMessages;
    probeDefinitionFromStringMessages.reset(new ProbeDefinitionFromStringMessagesTest);

//    cx::SectorInfoPtr sectorInfo = probeDefinitionFromStringMessages->getSectorInfo();

		cx::ImagePtr image = Utilities::create3DImage();
		probeDefinitionFromStringMessages->setImage(image);

		probeDefinitionFromStringMessages->parseValue(KEY_PROBE_TYPE, "1");
		CHECK_FALSE(probeDefinitionFromStringMessages->haveValidValues());
		probeDefinitionFromStringMessages->parseValue(KEY_ORIGIN, "0.0 0.0 0.0");
		probeDefinitionFromStringMessages->parseValue(KEY_ANGLES, "0.0 0.0");
		probeDefinitionFromStringMessages->parseValue(KEY_BOUNDING_BOX, "0 30 0 50");
		probeDefinitionFromStringMessages->parseValue(KEY_DEPTHS, "10 30");
		probeDefinitionFromStringMessages->parseValue(KEY_LINEAR_WIDTH, "30");
		probeDefinitionFromStringMessages->parseValue(KEY_SPACING_X, "0.5");
		CHECK_FALSE(probeDefinitionFromStringMessages->haveValidValues());
		probeDefinitionFromStringMessages->parseValue(KEY_SPACING_Y, "0.5");

    REQUIRE(probeDefinitionFromStringMessages->haveValidValues());

    REQUIRE(probeDefinitionFromStringMessages->createProbeDefintion("testProbeDefinition"));
}

TEST_CASE("ProbeDefinitionFromStringMessages require valid parameters", "[plugins][org.custusx.core.openigtlink3][plus][unit]")
{
		ProbeDefinitionFromStringMessagesTestPtr probeDefinitionFromStringMessages;
		probeDefinitionFromStringMessages.reset(new ProbeDefinitionFromStringMessagesTest);

		{
			probeDefinitionFromStringMessages->initWithValidParameters();
			REQUIRE(probeDefinitionFromStringMessages->haveValidValues());
			probeDefinitionFromStringMessages->parseValue(KEY_PROBE_TYPE, "5");//Should be 1 or 2
			INFO("ProbeType: " + string_cast(probeDefinitionFromStringMessages->getSectorInfo()->mProbeType));
			CHECK_FALSE(probeDefinitionFromStringMessages->haveValidValues());
		}
		{
			probeDefinitionFromStringMessages->initWithValidParameters();
			REQUIRE(probeDefinitionFromStringMessages->haveValidValues());
			probeDefinitionFromStringMessages->parseValue(KEY_ORIGIN, "0 0");//Should have 3 values
			INFO("Origin: " + stringFromDoubleVector(probeDefinitionFromStringMessages->getSectorInfo()->mOrigin));
			CHECK_FALSE(probeDefinitionFromStringMessages->haveValidValues());
		}
		{
			probeDefinitionFromStringMessages->initWithValidParameters();
			probeDefinitionFromStringMessages->parseValue(KEY_ANGLES, "0");//Should have 2 or 4 values
			INFO("Angles: " + stringFromDoubleVector(probeDefinitionFromStringMessages->getSectorInfo()->mAngles));
			CHECK_FALSE(probeDefinitionFromStringMessages->haveValidValues());
		}
		{
			probeDefinitionFromStringMessages->initWithValidParameters();
			probeDefinitionFromStringMessages->parseValue(KEY_BOUNDING_BOX, "0 0");//Should have 4 or 6 values
			INFO("BouningBox: " + stringFromDoubleVector(probeDefinitionFromStringMessages->getSectorInfo()->mBouningBox));
			CHECK_FALSE(probeDefinitionFromStringMessages->haveValidValues());
		}
		{
			probeDefinitionFromStringMessages->initWithValidParameters();
			probeDefinitionFromStringMessages->parseValue(KEY_DEPTHS, "0");//Should have 2 values
			INFO("Depths: " + stringFromDoubleVector(probeDefinitionFromStringMessages->getSectorInfo()->mDepths));
			CHECK_FALSE(probeDefinitionFromStringMessages->haveValidValues());
		}
		{
			probeDefinitionFromStringMessages->initWithValidParameters();
			probeDefinitionFromStringMessages->parseValue(KEY_LINEAR_WIDTH, "1000000");//Only checked to linear probes (ProbeType==2), Should be less than SectorInfo::toolarge (100000)
			INFO("LinearWidth: " + string_cast(probeDefinitionFromStringMessages->getSectorInfo()->mLinearWidth));
			probeDefinitionFromStringMessages->parseValue(KEY_PROBE_TYPE, "1");
			CHECK(probeDefinitionFromStringMessages->haveValidValues());
			probeDefinitionFromStringMessages->parseValue(KEY_PROBE_TYPE, "2");
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
