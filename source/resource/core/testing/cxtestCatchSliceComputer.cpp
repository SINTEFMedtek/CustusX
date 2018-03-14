/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
    slicer.setGravity(true, cx::Vector3D(0, 1, 0)); //Following the definition of the operating table
    slicer.setPlaneType(cx::ptTOOLSIDEPLANE);
    cx::Vector3D c_tool(5, 40, -50);
    cx::Vector3D center(44, 55, 66);    
    slicer.setFixedCenter(center);
    cx::Transform3D T = cx::createTransformTranslate(c_tool);

    cx::SlicePlane toolSidePlane(c_tool, cx::Vector3D( 0, 0, -1), cx::Vector3D( 0, -1, 0));

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
	//        The plane is still attached fully to the tool side plane.
	cx::SlicePlane singularityTestPlane(c_tool, cx::Vector3D(0.281085, 0, -0.959683), cx::Vector3D(0, -1, 0));
	R = cx::createTransformRotateX(M_PI_4) * cx::createTransformRotateZ(M_PI_4) * cx::createTransformRotateY(M_PI) * cx::createTransformRotateZ(M_PI_2);
	rMt = T*R;
	slicer.setToolPosition(rMt);
	CHECK(cx::similar(rMt.vector(cx::Vector3D(0, 0, 1)), cx::Vector3D(0, 0.707107, -0.707107))); // tip down and towards the feet
	CHECK(cx::similar(slicer.getPlane(), toolSidePlane));
	std::cout << "TOOLSIDE slicerplan, Case 4 \n" << slicer.getPlane();
}
