#include "sscSliceComputer.h"

#include <cppunit/extensions/TestFactoryRegistry.h> 
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <math.h>

using ssc::Vector3D;
using ssc::Transform3D;

namespace ssctest
{

/**Unit tests for class ssc::SliceComputer
 */
class TestSliceComputer : public CppUnit::TestFixture
{
public:
	void setUp()
	{
	}

	void tearDown()
	{
	}

	void testInitialize()
	{
		ssc::SliceComputer obj;
	}
	void testACS()
	{
		// test acs for the std case: a fixed center and a moving tool
		
		ssc::SliceComputer slicer;
		slicer.setOrientationType(ssc::SliceComputer::otORTHOGONAL);	
		slicer.setFollowType(ssc::SliceComputer::ftFIXED_CENTER);
		
		Vector3D c_tool(5,40,-50);
		Vector3D center(33,66,-3);
		Transform3D R = ssc::createTransformRotateY(M_PI) * ssc::createTransformRotateZ(M_PI_2);
		Transform3D T = ssc::createTransformTranslate(c_tool);		
		Transform3D rMt = T*R;
		slicer.setToolPosition(rMt);	
		slicer.setFixedCenter(center);
		
		slicer.setPlaneType(ssc::SliceComputer::ptSAGITTAL);
		ssc::SlicePlane sagittalPlane(Vector3D(c_tool[0], center[1], center[2]), Vector3D( 0, 1, 0), Vector3D(0,  0, 1));
		CPPUNIT_ASSERT(similar(slicer.getPlane(), sagittalPlane));		

		slicer.setPlaneType(ssc::SliceComputer::ptCORONAL);
		ssc::SlicePlane  coronalPlane(Vector3D(center[0], c_tool[1], center[2]), Vector3D(-1, 0, 0), Vector3D(0,  0, 1));
		CPPUNIT_ASSERT(similar(slicer.getPlane(), coronalPlane));		
		
		slicer.setPlaneType(ssc::SliceComputer::ptAXIAL);
		ssc::SlicePlane    axialPlane(Vector3D(center[0], center[1], c_tool[2]), Vector3D(-1, 0, 0), Vector3D(0, -1, 0));
		CPPUNIT_ASSERT(similar(slicer.getPlane(), axialPlane));		
	}
	
	void testACS_FollowTool()
	{
		ssc::SliceComputer slicer;
		slicer.setOrientationType(ssc::SliceComputer::otORTHOGONAL);	
		slicer.setFollowType(ssc::SliceComputer::ftFOLLOW_TOOL);
		
		Vector3D c_tool(5,40,-50);
		Vector3D center(44,55,66);
		Transform3D R = ssc::createTransformRotateY(M_PI) * ssc::createTransformRotateZ(M_PI_2);
		Transform3D T = ssc::createTransformTranslate(c_tool);		
		Transform3D rMt = T*R;
		slicer.setToolPosition(rMt);	
		slicer.setFixedCenter(center);
		
		slicer.setPlaneType(ssc::SliceComputer::ptSAGITTAL);
		ssc::SlicePlane sagittalPlane(c_tool, Vector3D( 0, 1, 0), Vector3D(0,  0, 1));
		CPPUNIT_ASSERT(similar(slicer.getPlane(), sagittalPlane));		

		slicer.setPlaneType(ssc::SliceComputer::ptCORONAL);
		ssc::SlicePlane  coronalPlane(c_tool, Vector3D(-1, 0, 0), Vector3D(0,  0, 1));
		CPPUNIT_ASSERT(similar(slicer.getPlane(), coronalPlane));		
		
		slicer.setPlaneType(ssc::SliceComputer::ptAXIAL);
		ssc::SlicePlane    axialPlane(c_tool, Vector3D(-1, 0, 0), Vector3D(0, -1, 0));
		CPPUNIT_ASSERT(similar(slicer.getPlane(), axialPlane));				
	}
	
	void testAnyPlanes()
	{
//		ssc::SliceComputer slicer;
//		slicer.setOrientationType(ssc::SliceComputer::otOBLIQUE);	
//		slicer.setFollowType(ssc::SliceComputer::ftFOLLOW_TOOL);
//		
//		Vector3D c_tool(5,40,-50);
//		Vector3D center(44,55,66);
//		Transform3D R = ssc::createTransformRotateY(M_PI) * ssc::createTransformRotateZ(M_PI_2);
//		Transform3D T = ssc::createTransformTranslate(c_tool);		
//		Transform3D rMt = T*R;
//		slicer.setToolPosition(rMt);	
//		slicer.setFixedCenter(center);
//		
//	case ptANYPLANE:    return std::make_pair(Vector3D( 0,-1, 0), Vector3D( 0, 0,-1)); 
//	case ptSIDEPLANE:   return std::make_pair(Vector3D(-1, 0, 0), Vector3D( 0, 0,-1)); 
//	case ptRADIALPLANE: return std::make_pair(Vector3D( 0,-1, 0), Vector3D(-1, 0, 0));
//
//		slicer.setPlaneType(ssc::SliceComputer::ptANYPLANE);
//		ssc::SlicePlane sagittalPlane(c_tool, Vector3D( 0,-1, 0), Vector3D( 0, 0,-1));
//		CPPUNIT_ASSERT(similar(slicer.getPlane(), sagittalPlane));		
//
//		slicer.setPlaneType(ssc::SliceComputer::ptSIDEPLANE);
//		ssc::SlicePlane  coronalPlane(c_tool, Vector3D(-1, 0, 0), Vector3D( 0, 0,-1));
//		CPPUNIT_ASSERT(similar(slicer.getPlane(), coronalPlane));		
//		
//		slicer.setPlaneType(ssc::SliceComputer::ptRADIALPLANE);
//		ssc::SlicePlane    axialPlane(c_tool, Vector3D( 0,-1, 0), Vector3D(-1, 0, 0));
//		CPPUNIT_ASSERT(similar(slicer.getPlane(), axialPlane));				
	}

	void testMore()
	{
		
	}
	
public:
	CPPUNIT_TEST_SUITE( TestSliceComputer );
		CPPUNIT_TEST( testInitialize );			
		CPPUNIT_TEST( testACS );			
		//CPPUNIT_TEST( testMore );			
		CPPUNIT_TEST( testACS_FollowTool );
	CPPUNIT_TEST_SUITE_END();
private:

};
CPPUNIT_TEST_SUITE_REGISTRATION( TestSliceComputer );

} // ssctest

