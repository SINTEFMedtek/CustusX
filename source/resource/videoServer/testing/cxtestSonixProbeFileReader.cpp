/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include <QString>
#include "cxSonixProbeFileReader.h"
#include "cxDataLocations.h"
#include "cxConfig.h"
namespace cxtest
{

namespace {

void testCLAProbe(cx::SonixProbeFileReader &reader)
{
		QDomNode probeNode = reader.getProbeNode("C5-2/60");
		REQUIRE_FALSE(probeNode.isNull());
		REQUIRE_FALSE(reader.isProbeLinear(probeNode));

		int pitch = 500;
		int numElements = 128;
		CHECK(reader.getProbeParam(probeNode, "radius") == 60000);
		REQUIRE(reader.getProbeParam(probeNode, "pitch") == pitch);
		REQUIRE(reader.getProbeParam(probeNode, "numElements") == numElements);
		REQUIRE(reader.getProbeLenght(probeNode) == pitch*numElements);
}

void testLinearProbe(cx::SonixProbeFileReader &reader)
{
		QDomNode probeNode = reader.getProbeNode("L14-5/38");
		REQUIRE_FALSE(probeNode.isNull());
		REQUIRE(reader.isProbeLinear(probeNode));

		int pitch = 300;
		int numElements = 128;
		CHECK(reader.getProbeParam(probeNode, "radius") == 0);
		REQUIRE(reader.getProbeParam(probeNode, "pitch") == pitch);
		REQUIRE(reader.getProbeParam(probeNode, "numElements") == numElements);
		REQUIRE(reader.getProbeLenght(probeNode) == pitch*numElements);
}

} //empty namespace


#ifdef CX_CUSTUS_SINTEF
TEST_CASE("SonixProbeFileReader can process XML file", "[resource][sonix][unit]")
{
	QString probeFile = cx::DataLocations::findConfigFilePath("probes.xml", "/ultrasonix");
	cx::SonixProbeFileReader reader(probeFile);
	REQUIRE(reader.init());
	REQUIRE(!reader.getProbes().isNull());

	SECTION("CLA probe")
			testCLAProbe(reader);
	SECTION("Linear probe")
			testLinearProbe(reader);
}
#endif
}
