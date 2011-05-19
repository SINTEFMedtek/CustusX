#define _USE_MATH_DEFINES
#include "sscTestSliceComputer.h"
#include "sscSliceComputer.h"

//#include <cppunit/extensions/TestFactoryRegistry.h> 
//#include <cppunit/ui/text/TestRunner.h>
//#include <cppunit/extensions/HelperMacros.h>
#include <math.h>
#include "sscTypeConversions.h"

using ssc::Vector3D;
using ssc::Transform3D;

void testResultText(const QString& msg)
{
	std::cout << msg;
}

void TestSliceComputer::setUp()
{
}

void TestSliceComputer::tearDown()
{
}

void TestSliceComputer::testInitialize()
{
	ssc::SliceComputer obj;
}
void TestSliceComputer::testACS()
{
	//testResultText("testing ACS\n");
	// test acs for the std case: a fixed center and a moving tool

	ssc::SliceComputer slicer;
	slicer.setOrientationType(ssc::otORTHOGONAL);
	slicer.setFollowType(ssc::ftFIXED_CENTER);

	Vector3D c_tool(5, 40, -50);
	Vector3D center(33, 66, -3);
	Transform3D R = ssc::createTransformRotateY(M_PI) * ssc::createTransformRotateZ(M_PI_2);
	Transform3D T = ssc::createTransformTranslate(c_tool);
	Transform3D rMt = T*R;
	slicer.setToolPosition(rMt);
	slicer.setFixedCenter(center);

	slicer.setPlaneType(ssc::ptSAGITTAL);
	ssc::SlicePlane sagittalPlane(Vector3D(c_tool[0], center[1], center[2]), Vector3D( 0, 1, 0), Vector3D(0, 0, 1));
	CPPUNIT_ASSERT(similar(slicer.getPlane(), sagittalPlane));

	slicer.setPlaneType(ssc::ptCORONAL);
	ssc::SlicePlane coronalPlane(Vector3D(center[0], c_tool[1], center[2]), Vector3D(-1, 0, 0), Vector3D(0, 0, 1));
	CPPUNIT_ASSERT(similar(slicer.getPlane(), coronalPlane));

	slicer.setPlaneType(ssc::ptAXIAL);
	ssc::SlicePlane axialPlane(Vector3D(center[0], center[1], c_tool[2]), Vector3D(-1, 0, 0), Vector3D(0, -1, 0));
	CPPUNIT_ASSERT(similar(slicer.getPlane(), axialPlane));
}

void TestSliceComputer::testACS_FollowTool()
{
	//testResultText("testing ACS-FollowTool\n");
	ssc::SliceComputer slicer;
	slicer.setOrientationType(ssc::otORTHOGONAL);
	slicer.setFollowType(ssc::ftFOLLOW_TOOL);

	Vector3D c_tool(5, 40, -50);
	Vector3D center(44, 55, 66);
	Transform3D R = ssc::createTransformRotateY(M_PI) * ssc::createTransformRotateZ(M_PI_2);
	Transform3D T = ssc::createTransformTranslate(c_tool);
	Transform3D rMt = T*R;
	slicer.setToolPosition(rMt);
	slicer.setFixedCenter(center);

	slicer.setPlaneType(ssc::ptSAGITTAL);
	ssc::SlicePlane sagittalPlane(c_tool, Vector3D( 0, 1, 0), Vector3D(0, 0, 1));
	CPPUNIT_ASSERT(similar(slicer.getPlane(), sagittalPlane));

	slicer.setPlaneType(ssc::ptCORONAL);
	ssc::SlicePlane coronalPlane(c_tool, Vector3D(-1, 0, 0), Vector3D(0, 0, 1));
	CPPUNIT_ASSERT(similar(slicer.getPlane(), coronalPlane));

	slicer.setPlaneType(ssc::ptAXIAL);
	ssc::SlicePlane axialPlane(c_tool, Vector3D(-1, 0, 0), Vector3D(0, -1, 0));
	CPPUNIT_ASSERT(similar(slicer.getPlane(), axialPlane));
}

/** anyplanes with no gravity, tool in standard position with 
 *  tip down and from towards nose
 */
void TestSliceComputer::testAnyPlanes()
{
	ssc::SliceComputer slicer;
	slicer.setOrientationType(ssc::otOBLIQUE);
	slicer.setFollowType(ssc::ftFOLLOW_TOOL);

	Vector3D c_tool(5, 40, -50);
	Vector3D center(44, 55, 66);
	// position the tool with the tip pointing down
	// and the front towards the nose.
	// this is the case when the surgeon stands behind the upright head
	// and aligns the tool with his left hand with the tip towards the feet.
	Transform3D R = ssc::createTransformRotateY(M_PI) * ssc::createTransformRotateZ(M_PI_2);
	Transform3D T = ssc::createTransformTranslate(c_tool);
	Transform3D rMt = T*R;
	CPPUNIT_ASSERT(ssc::similar(rMt.vector(ssc::Vector3D(0, 0, 1)), ssc::Vector3D(0, 0, -1))); // tip down
	CPPUNIT_ASSERT(ssc::similar(rMt.vector(ssc::Vector3D(0, 1, 0)), ssc::Vector3D(1, 0, 0))); // leftprobe to leftpatient
	slicer.setToolPosition(rMt);
	slicer.setFixedCenter(center);

	// looking from behind
	slicer.setPlaneType(ssc::ptANYPLANE);
	ssc::SlicePlane anyPlane(c_tool, Vector3D( -1, 0, 0), Vector3D( 0, 0, 1));
	CPPUNIT_ASSERT(similar(slicer.getPlane(), anyPlane));

	// looking from the right
	slicer.setPlaneType(ssc::ptSIDEPLANE);
	ssc::SlicePlane sidePlane(c_tool, Vector3D( 0, -1, 0), Vector3D( 0, 0, 1));
	//		std::cout << "side template: \n" << sidePlane << std::endl;
	//		std::cout << "side result: " << slicer.getPlane() << std::endl;
	CPPUNIT_ASSERT(similar(slicer.getPlane(), sidePlane));

	// looking from above
	slicer.setPlaneType(ssc::ptRADIALPLANE);
	ssc::SlicePlane radialPlane(c_tool, Vector3D(-1, 0, 0), Vector3D( 0, -1, 0));
	//std::cout << "radial: \n" << slicer.getPlane() << std::endl;
	CPPUNIT_ASSERT(similar(slicer.getPlane(), radialPlane));

	// perform test with a default gravity vector.
	// This has no effect
	slicer.setGravity(true, Vector3D(0, 0, -1));

	// looking from behind
	slicer.setPlaneType(ssc::ptANYPLANE);
	//std::cout << "any: \n" << slicer.getPlane() << std::endl;
	CPPUNIT_ASSERT(similar(slicer.getPlane(), anyPlane));
	// looking from the right
	slicer.setPlaneType(ssc::ptSIDEPLANE);
	CPPUNIT_ASSERT(similar(slicer.getPlane(), sidePlane));
	// looking from above
	slicer.setPlaneType(ssc::ptRADIALPLANE);
	CPPUNIT_ASSERT(similar(slicer.getPlane(), radialPlane));

	// test viewOffset
	slicer.setToolViewOffset(true, 40, 0.25);
	slicer.setPlaneType(ssc::ptANYPLANE);
	//std::cout << "viewoffset: \n" << slicer.getPlane() << std::endl;
	CPPUNIT_ASSERT(ssc::similar(slicer.getPlane().c, c_tool-10.0*Vector3D(0, 0, 1)));
	slicer.setToolViewOffset(false, 40, 0.25);

	// perform test with gravity vector normal to any direction.
	// this is a singularity and should be handled by using the standard vectors
	slicer.setGravity(true, Vector3D(0, 1, 0));

	// looking from behind
	slicer.setPlaneType(ssc::ptANYPLANE);
	//std::cout << "any: \n" << slicer.getPlane() << std::endl;
	CPPUNIT_ASSERT(similar(slicer.getPlane(), anyPlane));
	// looking from the right
	slicer.setPlaneType(ssc::ptSIDEPLANE);
	ssc::SlicePlane sidePlaneG(c_tool, Vector3D( 0, 0, -1), Vector3D( 0, -1, 0));
	CPPUNIT_ASSERT(similar(slicer.getPlane(), sidePlaneG));
	// looking from above
	slicer.setPlaneType(ssc::ptRADIALPLANE);
	//		std::cout << "r template: \n" << radialPlane << std::endl;
	//		std::cout << "r result: " << slicer.getPlane() << std::endl;
	ssc::SlicePlane radialPlaneG(c_tool, Vector3D(-1, 0, 0), Vector3D( 0, -1, 0));
	CPPUNIT_ASSERT(similar(slicer.getPlane(), radialPlaneG));
}

void TestSliceComputer::testMore()
{

}


