/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
