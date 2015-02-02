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
	QString probeFile = cx::DataLocations::findConfigFile("probes.xml", "/ultrasonix");
	cx::SonixProbeFileReader reader(probeFile);
	REQUIRE(reader.init());
	REQUIRE(!reader.getProbes().isNull());

	SECTION("CLA probe")
			testCLAProbe(reader);
	SECTION("Linear probe")
			testLinearProbe(reader);
}

}
