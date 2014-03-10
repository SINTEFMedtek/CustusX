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
#include "cxTrackingPositionFilter.h"

namespace cxtest
{

TEST_CASE("TrackingPositionFilter: One position is transmitted unchanged", "[unit]")
{
	cx::Transform3D expected = cx::createTransformTranslate(cx::Vector3D(1,2,3));
	cx::TrackingPositionFilter filter;

	filter.addPosition(expected, 0);
	cx::Transform3D result = filter.getFilteredPosition();

	INFO(expected << " == " << result);
	CHECK(cx::similar(expected, result));
}

TEST_CASE("TrackingPositionFilter: No positions return identity", "[unit]")
{
	cx::Transform3D expected = cx::Transform3D::Identity();
	cx::TrackingPositionFilter filter;

	cx::Transform3D result = filter.getFilteredPosition();

	INFO(expected << " == " << result);
	CHECK(cx::similar(expected, result));
}

TEST_CASE("TrackingPositionFilter: Check if interpolation is correct", "[unit]")
{
	//cx::Transform3D expected = cx::createTransformTranslate(cx::Vector3D(1,2,3));
	cx::TrackingPositionFilter filter;
	for (int i = 0; i < 50; i++){
		cx::Transform3D pos = cx::createTransformTranslate(cx::Vector3D(i,i,i));
		filter.addPosition(pos, i*40);
		cx::Transform3D result = filter.getFilteredPosition();
		//std::cout << pos << std::endl;
		//std::cout << result << std::endl;
		//std::cout << std::endl;
	}

	//INFO(expected << " == " << result);
	//CHECK(cx::similar(expected, result));
}


} // namespace cx


