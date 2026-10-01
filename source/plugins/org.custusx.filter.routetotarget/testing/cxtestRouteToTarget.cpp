/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxRouteToTarget.h"
#include "cxRouteToTargetFilterService.h"
#include "cxPointMetric.h"
#include "cxVector3D.h"
#include "cxtestSpaceProviderMock.h"

namespace cxtest
{

namespace
{
cx::PointMetricPtr createPoint(QString uid, cx::Vector3D coordinate)
{
	cx::PointMetricPtr point = cx::PointMetric::create(uid, "", cx::PatientModelServicePtr(), cxtest::SpaceProviderMock::create());
	point->setCoordinate(coordinate);
	return point;
}

std::map<QString, cx::PointMetricPtr> createPoints(std::vector<cx::Vector3D> coordinates)
{
	std::map<QString, cx::PointMetricPtr> retval;
	for (unsigned i = 0; i < coordinates.size(); ++i)
	{
		QString uid = cx::RouteToTargetFilter::getExtraAirwayPointUidPrefix() + QString::number(i + 1);
		retval[uid] = createPoint(uid, coordinates[i]);
	}
	return retval;
}
} // namespace

TEST_CASE("RouteToTargetFilter: extra airway point uid prefix is unchanged", "[unit][org.custusx.filter.routetotarget]")
{
	// Stored in saved Fraxinus patient files: changing it silently drops the via points of existing sessions.
	CHECK(cx::RouteToTargetFilter::getExtraAirwayPointUidPrefix() == "AirwayPoint");
}

TEST_CASE("RouteToTargetFilter: last extra airway point is the one with the highest number", "[unit][org.custusx.filter.routetotarget]")
{
	std::vector<cx::Vector3D> coordinates(10, cx::Vector3D(0, 0, 0));
	std::map<QString, cx::PointMetricPtr> airwayPoints = createPoints(coordinates);
	REQUIRE(airwayPoints.rbegin()->first == "AirwayPoint9"); // string order is not creation order
	CHECK(cx::RouteToTargetFilter::getLastExtraAirwayPointUid(airwayPoints) == "AirwayPoint10");

	airwayPoints.erase("AirwayPoint10");
	airwayPoints.erase("AirwayPoint5");
	CHECK(cx::RouteToTargetFilter::getLastExtraAirwayPointUid(airwayPoints) == "AirwayPoint9");

	CHECK(cx::RouteToTargetFilter::getLastExtraAirwayPointUid(std::map<QString, cx::PointMetricPtr>()).isEmpty());
}

TEST_CASE("RouteToTarget: route along one extra airway point is interpolated from the target", "[unit][org.custusx.filter.routetotarget]")
{
	cx::RouteToTarget routeToTarget;
	cx::Vector3D target(0, 0, 0);
	std::map<QString, cx::PointMetricPtr> airwayPoints = createPoints({cx::Vector3D(1, 0, 0)});

	std::vector<Eigen::Vector3d> route = routeToTarget.findRouteAlongExtraPoints(target, airwayPoints);

	REQUIRE(route.size() == 5);
	CHECK(cx::similar(route.front(), target));
	CHECK(cx::similar(route[1], cx::Vector3D(0.25, 0, 0)));
	CHECK(cx::similar(route.back(), cx::Vector3D(1, 0, 0)));
}

TEST_CASE("RouteToTarget: route visits extra airway points nearest first, regardless of uid order", "[unit][org.custusx.filter.routetotarget]")
{
	cx::RouteToTarget routeToTarget;
	cx::Vector3D target(0, 0, 0);
	cx::Vector3D farPoint(10, 0, 0);
	cx::Vector3D nearPoint(2, 0, 0);
	std::map<QString, cx::PointMetricPtr> airwayPoints = createPoints({farPoint, nearPoint});

	std::vector<Eigen::Vector3d> route = routeToTarget.findRouteAlongExtraPoints(target, airwayPoints);

	// target + 2mm/0.25mm + 8mm/0.25mm interpolated positions
	REQUIRE(route.size() == 1 + 8 + 32);
	CHECK(cx::similar(route[8], nearPoint));
	CHECK(cx::similar(route.back(), farPoint));
	for (unsigned i = 1; i < route.size(); ++i)
	{
		CHECK(route[i][0] > route[i - 1][0]);
	}
}

TEST_CASE("RouteToTarget: extra airway point closer than the interpolation distance adds no positions", "[unit][org.custusx.filter.routetotarget]")
{
	cx::RouteToTarget routeToTarget;
	cx::Vector3D target(0, 0, 0);
	std::map<QString, cx::PointMetricPtr> airwayPoints = createPoints({cx::Vector3D(0.1, 0, 0)});

	std::vector<Eigen::Vector3d> route = routeToTarget.findRouteAlongExtraPoints(target, airwayPoints);

	REQUIRE(route.size() == 1);
	CHECK(cx::similar(route.front(), target));
}

TEST_CASE("RouteToTarget: route without extra airway points contains only the target", "[unit][org.custusx.filter.routetotarget]")
{
	cx::RouteToTarget routeToTarget;
	cx::Vector3D target(1, 2, 3);

	std::vector<Eigen::Vector3d> route = routeToTarget.findRouteAlongExtraPoints(target, std::map<QString, cx::PointMetricPtr>());

	REQUIRE(route.size() == 1);
	CHECK(cx::similar(route.front(), target));
}

} // namespace cxtest
