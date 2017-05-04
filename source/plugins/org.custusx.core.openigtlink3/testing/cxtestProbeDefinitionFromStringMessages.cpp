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
    bool getTestMode()
    {
        return mTestMode;
    }
};

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
    probeDefinitionFromStringMessages->parseValue("StartDepth", "10");

    REQUIRE(sectorInfo->mProbeType == 1);
    REQUIRE(cx::similar(sectorInfo->mStartDepth, 10.0));

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
    probeDefinitionFromStringMessages->parseValue("StartDepth", "10");
    probeDefinitionFromStringMessages->parseValue("StopDepth", "110");
    probeDefinitionFromStringMessages->parseValue("StartLineX", "30");
    probeDefinitionFromStringMessages->parseValue("StartLineY", "30");
    probeDefinitionFromStringMessages->parseValue("StopLineX", "300");
    probeDefinitionFromStringMessages->parseValue("StopLineY", "30");
    probeDefinitionFromStringMessages->parseValue("StartLineAngle", "1.0");
    probeDefinitionFromStringMessages->parseValue("StopLineAngle", "1.5");
    probeDefinitionFromStringMessages->parseValue("SpacingX", "0.5");
    probeDefinitionFromStringMessages->parseValue("SpacingY", "0.5");

    CHECK_FALSE(probeDefinitionFromStringMessages->haveValidValues());

    probeDefinitionFromStringMessages->parseValue("SectorLeftPixels", "10");
    probeDefinitionFromStringMessages->parseValue("SectorRightPixels", "500");
    probeDefinitionFromStringMessages->parseValue("SectorTopPixels", "10");
    probeDefinitionFromStringMessages->parseValue("SectorBottomPixels", "300");

    CHECK_FALSE(probeDefinitionFromStringMessages->haveValidValues());

    probeDefinitionFromStringMessages->parseValue("SectorLeftMm", "-10");
    probeDefinitionFromStringMessages->parseValue("SectorRightMm", "10");
    probeDefinitionFromStringMessages->parseValue("SectorTopMm", "30");
    CHECK_FALSE(probeDefinitionFromStringMessages->haveValidValues());
    probeDefinitionFromStringMessages->parseValue("SectorBottomMm", "0");

    REQUIRE(probeDefinitionFromStringMessages->haveValidValues());

    REQUIRE_FALSE(probeDefinitionFromStringMessages->getTestMode());

    REQUIRE(probeDefinitionFromStringMessages->createProbeDefintion("testProbeDefinition"));
}

}//cxtest