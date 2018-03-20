/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

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


