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
#include <QString>
#include "cxSonixProbeFileReader.h"
#include "cxDataLocations.h"

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

TEST_CASE("SonixProbeFileReader can process XML file", "[resource][sonix][unit]")
{
	QString probeFile = cx::DataLocations::getRootConfigPath() + "/ultrasonix/probes.xml";
	cx::SonixProbeFileReader reader(probeFile);
	REQUIRE(reader.init());
	REQUIRE(!reader.getProbes().isNull());

	SECTION("CLA probe")
			testCLAProbe(reader);
	SECTION("Linear probe")
			testLinearProbe(reader);
}

}
