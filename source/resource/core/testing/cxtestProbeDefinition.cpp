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
#include <QSize>
#include <boost/shared_ptr.hpp>
#include "cxProbeDefinition.h"
#include "cxVector3D.h"
#include "cxBoundingBox3D.h"

namespace cxtest
{

TEST_CASE("ProbeDefinition can be constructed", "[unit][resource][core][ProbeDefinition]")
{
	cx::ProbeDefinitionPtr probeDefinition = cx::ProbeDefinitionPtr(new cx::ProbeDefinition());
	CHECK(probeDefinition.get());
	CHECK(probeDefinition->getType() == cx::ProbeDefinition::tNONE);
}

TEST_CASE("ProbeDefinition: Validating set/get", "[unit][resource][core][ProbeDefinition]")
{
	cx::ProbeDefinitionPtr probeDefinition(new cx::ProbeDefinition());
	CHECK(probeDefinition.get());

	cx::Vector3D origin_p = probeDefinition->getOrigin_p();
	probeDefinition->setOrigin_p(origin_p);
	CHECK(origin_p == probeDefinition->getOrigin_p());

	cx::Vector3D spacing = probeDefinition->getSpacing();
	probeDefinition->setSpacing(spacing);
	CHECK(spacing ==  probeDefinition->getSpacing());

	cx::DoubleBoundingBox3D clipRect_p = probeDefinition->getClipRect_p();
	probeDefinition->setClipRect_p(clipRect_p);
	CHECK(clipRect_p ==  probeDefinition->getClipRect_p());

	QSize size = probeDefinition->getSize();
	probeDefinition->setSize(size);
	CHECK(size ==  probeDefinition->getSize());

	double temporalCalibration = probeDefinition->getTemporalCalibration();
	probeDefinition->setTemporalCalibration(temporalCalibration);
	CHECK(temporalCalibration == probeDefinition->getTemporalCalibration());

	cx::ProbeDefinition::TYPE type = probeDefinition->getType();
	probeDefinition->setType(type);
	CHECK(type == probeDefinition->getType());

	double depthStart = probeDefinition->getDepthStart();
	double depthEnd = probeDefinition->getDepthEnd();
	double width = probeDefinition->getWidth();
	double centerOffset = probeDefinition->getCenterOffset();
	probeDefinition->setSector(depthStart, depthEnd, width, centerOffset);
	CHECK(depthStart == probeDefinition->getDepthStart());
	CHECK(depthEnd == probeDefinition->getDepthEnd());
	CHECK(width == probeDefinition->getWidth());
	CHECK(centerOffset == probeDefinition->getCenterOffset());
}

TEST_CASE("ProbeDefinition: Validating set/get with dummy values", "[unit][resource][core][ProbeDefinition]")
{
	cx::ProbeDefinitionPtr probeDefinition(new cx::ProbeDefinition());
	CHECK(probeDefinition.get());

	cx::Vector3D origin_p(10, 20, 30);
	probeDefinition->setOrigin_p(origin_p);
	CHECK(origin_p == probeDefinition->getOrigin_p());

	cx::Vector3D spacing(1, 2, 3);
	probeDefinition->setSpacing(spacing);
	CHECK(spacing ==  probeDefinition->getSpacing());

	cx::DoubleBoundingBox3D clipRect_p(10, 20, 5, 50);
	probeDefinition->setClipRect_p(clipRect_p);
	CHECK(clipRect_p ==  probeDefinition->getClipRect_p());

	QSize size(100, 200);
	probeDefinition->setSize(size);
	CHECK(size ==  probeDefinition->getSize());
}


}
