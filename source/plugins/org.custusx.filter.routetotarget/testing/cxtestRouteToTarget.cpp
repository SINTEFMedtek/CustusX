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
#include "cxBranchList.h"
#include "cxBranch.h"

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

// Trachea along -z, with a child branch along +y. Radius is 100+index in trachea and 200+index in child branch.
cx::BranchListPtr createTwoBranchesWithIndexedRadius()
{
	int numberOfPositions = 11;
	Eigen::MatrixXd tracheaPositions(3, numberOfPositions);
	Eigen::MatrixXd childPositions(3, numberOfPositions);
	Eigen::VectorXd tracheaRadius(numberOfPositions);
	Eigen::VectorXd childRadius(numberOfPositions);
	for (int i = 0; i < numberOfPositions; ++i)
	{
		tracheaPositions.col(i) = Eigen::Vector3d(0, 0, 100 - i);
		childPositions.col(i) = Eigen::Vector3d(0, i + 1, 90);
		tracheaRadius(i) = 100 + i;
		childRadius(i) = 200 + i;
	}

	cx::BranchPtr trachea(new cx::Branch());
	trachea->setPositions(tracheaPositions);
	trachea->setRadius(tracheaRadius);
	cx::BranchPtr child(new cx::Branch());
	child->setPositions(childPositions);
	child->setRadius(childRadius);
	child->setParentBranch(trachea);
	trachea->addChildBranch(child);

	cx::BranchListPtr branchList(new cx::BranchList());
	branchList->addBranch(trachea);
	branchList->addBranch(child);
	return branchList;
}

double findRadiusAtBranchPosition(cx::BranchListPtr branchList, Eigen::Vector3d position, bool& found)
{
	std::vector<cx::BranchPtr> branches = branchList->getBranches();
	for (unsigned i = 0; i < branches.size(); ++i)
	{
		Eigen::MatrixXd positions = branches[i]->getPositions();
		for (int j = 0; j < positions.cols(); ++j)
			if (cx::similar(cx::Vector3D(positions.col(j)), cx::Vector3D(position)))
			{
				found = true;
				return branches[i]->getRadius()(j);
			}
	}
	found = false;
	return 0;
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

TEST_CASE("RouteToTarget: radius along route belongs to the same branch position as the route position", "[unit][org.custusx.filter.routetotarget]")
{
	cx::BranchListPtr branchList = createTwoBranchesWithIndexedRadius();
	cx::RouteToTarget routeToTarget;
	routeToTarget.setSmoothing(false);
	routeToTarget.setBranchList(branchList);
	cx::PointMetricPtr target = createPoint("target", cx::Vector3D(0, 13, 90));

	routeToTarget.findRouteToTarget(target, "");
	routeToTarget.findExtendedRoute(target);

	std::vector<Eigen::Vector3d> route = routeToTarget.getRoutePositions(true);
	std::vector<double> radius = routeToTarget.getRadius();
	REQUIRE(route.size() == radius.size());

	int positionsOnBranches = 0;
	for (unsigned i = 0; i < route.size(); ++i)
	{
		bool onBranch = false;
		double expectedRadius = findRadiusAtBranchPosition(branchList, route[i], onBranch);
		if (onBranch)
		{
			++positionsOnBranches;
			CHECK(radius[i] == Approx(expectedRadius));
		}
		else
			CHECK(radius[i] == Approx(0)); // extension from airway to target
	}
	CHECK(positionsOnBranches == 22);
}

TEST_CASE("RouteToTarget: with smoothing, the position at the branching point gets the parent's radius", "[unit][org.custusx.filter.routetotarget]")
{
	cx::BranchListPtr branchList = createTwoBranchesWithIndexedRadius();
	cx::RouteToTarget routeToTarget;
	routeToTarget.setSmoothing(true);
	routeToTarget.setBranchList(branchList);
	cx::PointMetricPtr target = createPoint("target", cx::Vector3D(0, 13, 90));

	routeToTarget.findRouteToTarget(target, "");
	routeToTarget.findExtendedRoute(target);

	std::vector<Eigen::Vector3d> route = routeToTarget.getRoutePositions(true);
	std::vector<double> radius = routeToTarget.getRadius();
	REQUIRE(route.size() == radius.size());

	Eigen::Vector3d branchingPoint(0, 0, 90); // last position of the trachea
	int positionsAtBranchingPoint = 0;
	for (unsigned i = 0; i < route.size(); ++i)
	{
		if (cx::similar(cx::Vector3D(route[i]), cx::Vector3D(branchingPoint), 1e-6))
		{
			++positionsAtBranchingPoint;
			CHECK(radius[i] == Approx(110)); // last trachea radius
		}
	}
	CHECK(positionsAtBranchingPoint >= 1);
}

TEST_CASE("RouteToTarget: radius and generation numbers are aligned with route when using extra airway points", "[unit][org.custusx.filter.routetotarget]")
{
	cx::BranchListPtr branchList = createTwoBranchesWithIndexedRadius();
	cx::RouteToTarget routeToTarget;
	routeToTarget.setSmoothing(false);
	routeToTarget.setBranchList(branchList);
	cx::PointMetricPtr target = createPoint("target", cx::Vector3D(0, 16, 90));
	std::map<QString, cx::PointMetricPtr> airwayPoints = createPoints({cx::Vector3D(0, 14, 90)});

	routeToTarget.findRouteToTarget(target, "", airwayPoints);
	routeToTarget.findExtendedRoute(target);

	size_t routeSize = routeToTarget.getRoutePositions(true).size();
	CHECK(routeToTarget.getRadius().size() == routeSize);
	CHECK(routeToTarget.getGenerationNumbers().size() == routeSize);
	CHECK(routeToTarget.getCameraRotation().size() == routeSize);
}

} // namespace cxtest
