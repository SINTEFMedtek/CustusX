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

#include "cxSliceComputer.h"
#include "cxTypeConversions.h"
#include "catch.hpp"


TEST_CASE("SliceComputer is created", "[unit][resource][core]")
{
	cx::SliceComputer obj;
	CHECK(true);
}

TEST_CASE("SliceComputer handles ACS using fixed center", "[unit][resource][core]")
{
	//testResultText("testing ACS\n");
	// test acs for the std case: a fixed center and a moving tool

	cx::SliceComputer slicer;
	slicer.setOrientationType(cx::otORTHOGONAL);
	slicer.setFollowType(cx::ftFIXED_CENTER);

	cx::Vector3D c_tool(5, 40, -50);
	cx::Vector3D center(33, 66, -3);
	cx::Transform3D R = cx::createTransformRotateY(M_PI) * cx::createTransformRotateZ(M_PI_2);
	cx::Transform3D T = cx::createTransformTranslate(c_tool);
	cx::Transform3D rMt = T*R;
	slicer.setToolPosition(rMt);
	slicer.setFixedCenter(center);

	slicer.setPlaneType(cx::ptSAGITTAL);
	cx::SlicePlane sagittalPlane(cx::Vector3D(c_tool[0], center[1], center[2]), cx::Vector3D( 0, 1, 0), cx::Vector3D(0, 0, 1));
	CHECK(similar(slicer.getPlane(), sagittalPlane));

	slicer.setPlaneType(cx::ptCORONAL);
	cx::SlicePlane coronalPlane(cx::Vector3D(center[0], c_tool[1], center[2]), cx::Vector3D(-1, 0, 0), cx::Vector3D(0, 0, 1));
	CHECK(similar(slicer.getPlane(), coronalPlane));

	slicer.setPlaneType(cx::ptAXIAL);
	cx::SlicePlane axialPlane(cx::Vector3D(center[0], center[1], c_tool[2]), cx::Vector3D(-1, 0, 0), cx::Vector3D(0, -1, 0));
	CHECK(similar(slicer.getPlane(), axialPlane));
}

TEST_CASE("SliceComputer handles ACS using follow tool", "[unit][resource][core]")
//void TestSliceComputer::testACS_FollowTool()
{
	//testResultText("testing ACS-FollowTool\n");
	cx::SliceComputer slicer;
	slicer.setOrientationType(cx::otORTHOGONAL);
	slicer.setFollowType(cx::ftFOLLOW_TOOL);

	cx::Vector3D c_tool(5, 40, -50);
	cx::Vector3D center(44, 55, 66);
	cx::Transform3D R = cx::createTransformRotateY(M_PI) * cx::createTransformRotateZ(M_PI_2);
	cx::Transform3D T = cx::createTransformTranslate(c_tool);
	cx::Transform3D rMt = T*R;
	slicer.setToolPosition(rMt);
	slicer.setFixedCenter(center);

	slicer.setPlaneType(cx::ptSAGITTAL);
	cx::SlicePlane sagittalPlane(c_tool, cx::Vector3D( 0, 1, 0), cx::Vector3D(0, 0, 1));
	CHECK(similar(slicer.getPlane(), sagittalPlane));

	slicer.setPlaneType(cx::ptCORONAL);
	cx::SlicePlane coronalPlane(c_tool, cx::Vector3D(-1, 0, 0), cx::Vector3D(0, 0, 1));
	CHECK(similar(slicer.getPlane(), coronalPlane));

	slicer.setPlaneType(cx::ptAXIAL);
	cx::SlicePlane axialPlane(c_tool, cx::Vector3D(-1, 0, 0), cx::Vector3D(0, -1, 0));
	CHECK(cx::similar(slicer.getPlane(), axialPlane));
}

/** anyplanes with no gravity, tool in standard position with
 *  tip down and from towards nose
 */
TEST_CASE("SliceComputer handles anyplanes using follow tool", "[unit][resource][core]")
{
	cx::SliceComputer slicer;
	slicer.setOrientationType(cx::otOBLIQUE);
	slicer.setFollowType(cx::ftFOLLOW_TOOL);

	cx::Vector3D c_tool(5, 40, -50);
	cx::Vector3D center(44, 55, 66);
	// position the tool with the tip pointing down
	// and the front towards the nose.
	// this is the case when the surgeon stands behind the upright head
	// and aligns the tool with his left hand with the tip towards the feet.
	cx::Transform3D R = cx::createTransformRotateY(M_PI) * cx::createTransformRotateZ(M_PI_2);
	cx::Transform3D T = cx::createTransformTranslate(c_tool);
	cx::Transform3D rMt = T*R;
	CHECK(cx::similar(rMt.vector(cx::Vector3D(0, 0, 1)), cx::Vector3D(0, 0, -1))); // tip down
	CHECK(cx::similar(rMt.vector(cx::Vector3D(0, 1, 0)), cx::Vector3D(1, 0, 0))); // leftprobe to leftpatient
	slicer.setToolPosition(rMt);
	slicer.setFixedCenter(center);

	// looking from behind
	slicer.setPlaneType(cx::ptANYPLANE);
	cx::SlicePlane anyPlane(c_tool, cx::Vector3D( -1, 0, 0), cx::Vector3D( 0, 0, 1));
	CHECK(cx::similar(slicer.getPlane(), anyPlane));

	// looking from the right
	slicer.setPlaneType(cx::ptSIDEPLANE);
	cx::SlicePlane sidePlane(c_tool, cx::Vector3D( 0, -1, 0), cx::Vector3D( 0, 0, 1));
	//		std::cout << "side template: \n" << sidePlane << std::endl;
	//		std::cout << "side result: " << slicer.getPlane() << std::endl;
	CHECK(cx::similar(slicer.getPlane(), sidePlane));

	// looking from above
	slicer.setPlaneType(cx::ptRADIALPLANE);
	cx::SlicePlane radialPlane(c_tool, cx::Vector3D(-1, 0, 0), cx::Vector3D( 0, -1, 0));
	//std::cout << "radial: \n" << slicer.getPlane() << std::endl;
	CHECK(cx::similar(slicer.getPlane(), radialPlane));

	// perform test with a default gravity vector.
	// This has no effect
	slicer.setGravity(true, cx::Vector3D(0, 0, -1));

	// looking from behind
	slicer.setPlaneType(cx::ptANYPLANE);
	//std::cout << "any: \n" << slicer.getPlane() << std::endl;
	CHECK(cx::similar(slicer.getPlane(), anyPlane));
	// looking from the right
	slicer.setPlaneType(cx::ptSIDEPLANE);
	CHECK(cx::similar(slicer.getPlane(), sidePlane));
	// looking from above
	slicer.setPlaneType(cx::ptRADIALPLANE);
	CHECK(cx::similar(slicer.getPlane(), radialPlane));

	// test viewOffset
	slicer.setToolViewOffset(true, 40, 0.25);
	slicer.setPlaneType(cx::ptANYPLANE);
	CHECK(cx::similar(slicer.getPlane().c, c_tool-10.0*cx::Vector3D(0, 0, 1)));
	slicer.setToolViewOffset(false, 40, 0.25);

	// perform test with gravity vector normal to any direction.
	// this is a singularity and should be handled by using the standard vectors
	slicer.setGravity(true, cx::Vector3D(0, 1, 0));

	// looking from behind
	slicer.setPlaneType(cx::ptANYPLANE);
	//std::cout << "any: \n" << slicer.getPlane() << std::endl;
	CHECK(cx::similar(slicer.getPlane(), anyPlane));
	// looking from the right
	slicer.setPlaneType(cx::ptSIDEPLANE);
	cx::SlicePlane sidePlaneG(c_tool, cx::Vector3D( 0, 0, -1), cx::Vector3D( 0, -1, 0));
	CHECK(cx::similar(slicer.getPlane(), sidePlaneG));
	// looking from above
	slicer.setPlaneType(cx::ptRADIALPLANE);
	//		std::cout << "r template: \n" << radialPlane << std::endl;
	//		std::cout << "r result: " << slicer.getPlane() << std::endl;
	cx::SlicePlane radialPlaneG(c_tool, cx::Vector3D(-1, 0, 0), cx::Vector3D( 0, -1, 0));
	CHECK(cx::similar(slicer.getPlane(), radialPlaneG));
}

/** ToolSidePlane where the plane should always be oriented with the gravity/up vector.
 *  anyplanes with no gravity, tool in standard position with
 *  tip down and from towards nose
 */
TEST_CASE("SliceComputer handles TOOLSIDE plane using follow tool", "[unit][resource][core]")
{
    cx::SliceComputer slicer;
    slicer.setOrientationType(cx::otOBLIQUE);
    slicer.setFollowType(cx::ftFOLLOW_TOOL);

    cx::Vector3D c_tool(5, 40, -50);
    cx::Vector3D center(44, 55, 66);
    // position the tool with the tip pointing down
    // and the front towards the nose.
    // this is the case when the surgeon stands behind the upright head
    // and aligns the tool with his left hand with the tip towards the feet.
//    cx::Transform3D R = cx::createTransformRotateY(M_PI) * cx::createTransformRotateZ(M_PI_2);
    // BRUKER DENNE cx::Transform3D R = cx::createTransformRotateX(M_PI_4) * cx::createTransformRotateY(M_PI) * cx::createTransformRotateZ(M_PI_2);
    //cx::Transform3D R = cx::createTransformRotateX(M_PI_4) * cx::createTransformRotateY(M_PI_4) * cx::createTransformRotateY(M_PI) * cx::createTransformRotateZ(M_PI_2);
    cx::Transform3D T = cx::createTransformTranslate(c_tool);
    // BRUKER DENNE cx::Transform3D rMt = T*R;
    //CHECK(cx::similar(rMt.vector(cx::Vector3D(0, 0, 1)), cx::Vector3D(0, 0, -1))); // tip down
    // BRUKER DENNE CHECK(cx::similar(rMt.vector(cx::Vector3D(0, 0, 1)), cx::Vector3D(0, 0.707107, -0.707107))); // tip pointing down and in the direction of the nose
    // BRUKER DENNE CHECK(cx::similar(rMt.vector(cx::Vector3D(0, 1, 0)), cx::Vector3D(1, 0, 0))); // leftprobe to leftpatient
    //BRUKER DENNE slicer.setToolPosition(rMt);
    slicer.setFixedCenter(center);


    //TOOLSIDE
    slicer.setGravity(true, cx::Vector3D(0, 1, 0)); //As the definition of the operating table
    slicer.setPlaneType(cx::ptTOOLSIDEPLANE);
    // OLD cx::SlicePlane toolSidePlane(c_tool, cx::Vector3D( 0, -1, 0), cx::Vector3D( 0, 0, 1));
    cx::SlicePlane toolSidePlane(c_tool, cx::Vector3D( 0, 0, -1), cx::Vector3D( 0, -1, 0));
    //BRUKER DENNE CHECK(cx::similar(slicer.getPlane(), toolSidePlane));


    //Case 1: The tool is parallel with the table, the patient lies on his back and the tool is behind the head pointing towards the feet.
    cx::Transform3D R = cx::createTransformRotateY(M_PI) * cx::createTransformRotateZ(M_PI_2);
    cx::Transform3D rMt = T*R;
    slicer.setToolPosition(rMt);
    CHECK(cx::similar(slicer.getPlane(), toolSidePlane));
    std::cout << "TOOLSIDE slicerplan, Case 1 \n" << slicer.getPlane();

    //Case 2: The tool is parallel to the table with the tip pointing towards the feet
    //        and rotated 45 degrees towards the left side of the patient
    R = cx::createTransformRotateZ(M_PI_4) * cx::createTransformRotateY(M_PI) * cx::createTransformRotateZ(M_PI_2);
    rMt = T*R;
    slicer.setToolPosition(rMt);
    CHECK(cx::similar(rMt.vector(cx::Vector3D(0, 0, 1)), cx::Vector3D(0, 0, -1))); // tip towards feet
    CHECK(cx::similar(rMt.vector(cx::Vector3D(0, 1, 0)), cx::Vector3D(0.707107, 0.707107, 0))); // left of the probe left and down
    CHECK(cx::similar(slicer.getPlane(), toolSidePlane));
    std::cout << "TOOLSIDE slicerplan, Case 2 \n" << slicer.getPlane();

    //Case 3: The tool is pointing down along the gravity with the front of the tool towards the feet
    R = cx::createTransformRotateX(M_PI_2) * cx::createTransformRotateY(M_PI) * cx::createTransformRotateZ(M_PI_2);
    rMt = T*R;
    slicer.setToolPosition(rMt);
    CHECK(cx::similar(rMt.vector(cx::Vector3D(0, 0, 1)), cx::Vector3D(0, 1, 0))); // tip along gravity
    CHECK(cx::similar(slicer.getPlane(), toolSidePlane));
    std::cout << "TOOLSIDE slicerplan, Case 3 \n" << slicer.getPlane();


    //Case 4: The tool is pointing down 45 degrees towards the nose and rotated 45 degrees to the left.
    //        This will start to turn the plane slightly as we approach the singularity case
    cx::SlicePlane singularityTestPlane(c_tool, cx::Vector3D(0.281085, 0, -0.959683), cx::Vector3D(0, -1, 0));
    R = cx::createTransformRotateX(M_PI_4) * cx::createTransformRotateZ(M_PI_4) * cx::createTransformRotateY(M_PI) * cx::createTransformRotateZ(M_PI_2);
    rMt = T*R;
    slicer.setToolPosition(rMt);
    CHECK(cx::similar(rMt.vector(cx::Vector3D(0, 0, 1)), cx::Vector3D(0, 0.707107, -0.707107))); // tip down and towards the feet
    CHECK(cx::similar(slicer.getPlane(), singularityTestPlane));
    std::cout << "TOOLSIDE slicerplan, Case 4 \n" << slicer.getPlane();






/*


    cx::SlicePlane toolSidePlane2(c_tool, cx::Vector3D( 0, -0.707107, -0.707107), cx::Vector3D( 0, -0.707107, 0.707107));
    std::cout << "toolSidePlane \n" << toolSidePlane;
    std::cout << "slicerplan FØR rotasjon \n" << slicer.getPlane();
    //Transform tool so that the handle tilts 45 degrees towards the top of the head of the patient.
    //cx::Transform3D rotateTowardsHead = cx::createTransformRotateX(M_PI_4);
    cx::Transform3D rotateTowardsHead = cx::createTransformRotateX(M_PI_4);
    //rMt = rMt * rotateTowardsHead;
    //rMt = rotateTowardsHead * rMt;
    rMt = cx::createTransformRotateX(M_PI_4) * cx::createTransformRotateY(M_PI) * cx::createTransformRotateZ(M_PI_2);
    cx::Transform3D rMt1 = cx::createTransformRotateX(M_PI_4) * cx::createTransformRotateY(M_PI) * cx::createTransformRotateZ(M_PI_2);
    cx::Transform3D rMt11 = T*rMt1;
    CHECK(cx::similar(rMt11.vector(cx::Vector3D(0, 0, 1)), cx::Vector3D(0, 0.707107, -0.707107))); // tip right
    CHECK(cx::similar(rMt11.vector(cx::Vector3D(0, 1, 0)), cx::Vector3D(1, 0, 0))); // leftprobe to leftpatient
    rMt = T*rMt;
    //slicer.setToolPosition(rMt);

//    cx::SliceComputer slicer3;
//    slicer3.setOrientationType(cx::otOBLIQUE);
//    slicer3.setFollowType(cx::ftFOLLOW_TOOL);
    slicer.setToolPosition(rMt);
//    slicer3.setFixedCenter(center);
//    slicer3.setPlaneType(cx::ptTOOLSIDEPLANE);
    // BRUKTE DENNE CHECK(cx::similar(slicer.getPlane(), toolSidePlane2));
    std::cout << "slicerplan ENDELIG \n" << slicer.getPlane();
//    CHECK(cx::similar(slicer3.getPlane(), toolSidePlane));
//    std::cout << "slicerplan ENDELIG \n" << slicer3.getPlane();



    cx::SliceComputer slicer2;
    slicer2.setOrientationType(cx::otOBLIQUE);
    slicer2.setFollowType(cx::ftFOLLOW_TOOL);
    cx::Transform3D R2 = cx::createTransformRotateX(M_PI_4) * cx::createTransformRotateY(M_PI) * cx::createTransformRotateZ(M_PI_2);
    cx::Transform3D T2 = cx::createTransformTranslate(c_tool);
    cx::Transform3D rMt2 = T2*R2;
    CHECK(cx::similar(rMt2.vector(cx::Vector3D(0, 0, 1)), cx::Vector3D(0, 0.707107, -0.707107))); // tip right
    CHECK(cx::similar(rMt2.vector(cx::Vector3D(0, 1, 0)), cx::Vector3D(1, 0, 0))); // leftprobe to leftpatient
    slicer2.setToolPosition(rMt2);
    slicer2.setFixedCenter(center);
    slicer2.setPlaneType(cx::ptTOOLSIDEPLANE);
    cx::SlicePlane sidePlane3(c_tool, cx::Vector3D( 0, -0.707107, -0.707107), cx::Vector3D( 0, -0.707107, 0.707107));
    //CHECK(cx::similar(slicer2.getPlane(), sidePlane3));
    std::cout << "slicer2 \n" << slicer2.getPlane();

*/
}
