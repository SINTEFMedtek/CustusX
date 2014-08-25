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


