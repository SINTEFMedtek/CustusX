#include "catch.hpp"
#include <QSize>
#include <boost/shared_ptr.hpp>
#include "sscProbeData.h"
#include "sscVector3D.h"
#include "sscBoundingBox3D.h"

namespace cxtest
{

TEST_CASE("ProbeDefinition can be constructed", "[unit][resource][core][ProbeDefinition][unstable]")
{
	cx::ProbeDataPtr probeDefinition = cx::ProbeDataPtr(new cx::ProbeData());
	CHECK(probeDefinition.get());
	CHECK(probeDefinition->getType() == cx::ProbeData::tNONE);
}

TEST_CASE("ProbeDefinition: Validating set/get", "[unit][resource][core][ProbeDefinition][unstable]")
{
	cx::ProbeDataPtr probeDefinition(new cx::ProbeData());
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

	cx::ProbeData::TYPE type = probeDefinition->getType();
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



}
