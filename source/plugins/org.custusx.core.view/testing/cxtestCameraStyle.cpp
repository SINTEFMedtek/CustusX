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
#include "cxCameraStyleForView.h"

void test_findCameraPosOnLineFixedDistanceFromFocus(cx::Vector3D p_line, cx::Vector3D e_line, double distance, cx::Vector3D focus)
{
	cx::Vector3D p = cx::CameraStyleForView::findCameraPosOnLineFixedDistanceFromFocus(p_line, e_line, distance, focus);
	CHECK(cx::similar((p-focus).length(), distance));
}

TEST_CASE("CameraStyleForView: findCameraPosOnLineFixedDistanceFromFocus ", "[unit]")
{
	test_findCameraPosOnLineFixedDistanceFromFocus(cx::Vector3D(0,0,0), cx::Vector3D(0,1,0),  2, cx::Vector3D(1,0,0));
	test_findCameraPosOnLineFixedDistanceFromFocus(cx::Vector3D(0,0,0), cx::Vector3D(0,1,0), 10, cx::Vector3D(1,1,0));
	test_findCameraPosOnLineFixedDistanceFromFocus(cx::Vector3D(2,2,0), cx::Vector3D(0,1,0), 10, cx::Vector3D(1,1,0));
	test_findCameraPosOnLineFixedDistanceFromFocus(cx::Vector3D(2,2,0), cx::Vector3D(1,1,0), 20, cx::Vector3D(1,2,0));
	test_findCameraPosOnLineFixedDistanceFromFocus(cx::Vector3D(2,2,0), cx::Vector3D(1,1,0), 20, cx::Vector3D(1,2,4));
}

