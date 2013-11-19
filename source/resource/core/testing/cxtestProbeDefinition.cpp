#include "catch.hpp"
#include <QSize>
#include <boost/shared_ptr.hpp>
#include "sscProbeData.h"
#include "sscVector3D.h"
#include "sscBoundingBox3D.h"

namespace cxtest
{

TEST_CASE("ProbeDefinition can be constructed", "[unit][resource][core][ProbeDefinition]")
{
	cx::ProbeDataPtr probeDefinition = cx::ProbeDataPtr(new cx::ProbeData());
	CHECK(probeDefinition.get());
	CHECK(probeDefinition->getType() == cx::ProbeData::tNONE);
}

TEST_CASE("ProbeDefinition: Validating set/get", "[unit][resource][core][ProbeDefinition]")
{
	cx::ProbeDataPtr probeDefinition;

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
}

}
