#define _USE_MATH_DEFINES
#include "sscTestUtilityClasses.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <math.h>

#include <cppunit/extensions/TestFactoryRegistry.h> 
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "sscTestUtilities.h"
#include "sscSliceComputer.h"
#include "sscVector3D.h"
#include "sscTransform3D.h"
#include "sscFrame3D.h"

using namespace ssc::utils;

void TestUtilities::setUp()
{
}

void TestUtilities::tearDown()
{
}

void TestUtilities::testTransform3DAccess()
{
	Transform3D t = createTransformRotateY(M_PI/4)*createTransformRotateX(M_PI/3)*createTransformTranslate(Vector3D(3,4,5));
	//const Transform3D ct = t;
	
	for (unsigned i=0; i<4; ++i)
	{
		for (unsigned j=0; j<4; ++j)
		{
			double val = i*4+j;
			t[i][j] = val;
			//ct[i][j] = val; // does not compile: ok
			//double temp = ct[i][j];  // does compile: ok
			CPPUNIT_ASSERT(similar(val, t[i][j]));
		}
	}
}

void TestUtilities::singleTestFrame(const Transform3D& transform)
{
	Frame3D frame = Frame3D::create(transform);
	Transform3D restored = frame.transform();
	
	if (!similar(transform, restored))
	{
		std::cout << "--------------------------------------" << std::endl;
		std::cout << "mismatch:" << std::endl;
		std::cout << "original:" << std::endl;
		std::cout << transform << std::endl;
		std::cout << "frame: " << frame << std::endl;
		std::cout << "axis: " << frame.rotationAxis() << std::endl;
		std::cout << std::endl;		
		std::cout << "restored:" << std::endl;
		std::cout << restored << std::endl;
		std::cout << "--------------------------------------" << std::endl;
	}
	else if (similar(transform, restored))
	{
//		std::cout << "success" << std::endl;
//		std::cout << transform << std::endl;
//		std::cout << "frame: " << frame << std::endl;
//		std::cout << "axis: " << frame.rotationAxis() << std::endl;
	}
	
	CPPUNIT_ASSERT(similar(transform, restored));
}

void TestUtilities::singleTestFrameRotationAxis(const Vector3D& k)
{
	Frame3D frame;
	frame.setRotationAxis(k);
	CPPUNIT_ASSERT(similar(k, frame.rotationAxis()));
	CPPUNIT_ASSERT(similar(frame.rotationAxis().normal(), frame.rotationAxis()));	
}

//#define SINGLE_TEST_FRAME(expr) std::cout << "testing: " << # expr << std::endl; singleTestFrame(expr);
#define SINGLE_TEST_FRAME(expr) singleTestFrame(expr);

void TestUtilities::testFrame()
{
	singleTestFrameRotationAxis((ssc::createTransformRotateZ(M_PI/4)).vector(Vector3D(1,0,0)));
	singleTestFrameRotationAxis((ssc::createTransformRotateY(M_PI/4)).vector(Vector3D(1,0,0)));
	singleTestFrameRotationAxis((ssc::createTransformRotateZ(M_PI/4)*ssc::createTransformRotateY(M_PI/4)).vector(Vector3D(1,0,0)));
	
	SINGLE_TEST_FRAME( Transform3D() );
	SINGLE_TEST_FRAME( createTransformTranslate(Vector3D(1,0,0)) );
	SINGLE_TEST_FRAME( createTransformTranslate(Vector3D(4,3,2)) );
	SINGLE_TEST_FRAME( ssc::createTransformRotateX(M_PI_2) );
	SINGLE_TEST_FRAME( ssc::createTransformRotateX(M_PI_4) );
	SINGLE_TEST_FRAME( ssc::createTransformRotateY(0/4)*createTransformRotateX(M_PI/4)*createTransformTranslate(Vector3D(0,0,0)) );
	SINGLE_TEST_FRAME( createTransformRotateX(M_PI/4) );
	SINGLE_TEST_FRAME( ssc::createTransformRotateY(4*M_PI/4) * createTransformRotateX(M_PI/4) );
	SINGLE_TEST_FRAME( createTransformRotateY(M_PI/4)*createTransformRotateX(M_PI/3)*createTransformTranslate(Vector3D(3,4,5)) );
	SINGLE_TEST_FRAME( createTransformRotateY(M_PI/4.1)*createTransformRotateX(M_PI/3.3)*createTransformTranslate(Vector3D(3,4,5))*createTransformRotateZ(M_PI/4) );
	SINGLE_TEST_FRAME( createTransformRotateY(M_PI/2)*createTransformRotateX(M_PI/2)*createTransformTranslate(Vector3D(3,4,5)) );
	SINGLE_TEST_FRAME( createTransformRotateX(M_PI)*createTransformTranslate(Vector3D(3,4,5)) );
	SINGLE_TEST_FRAME( createTransformRotateY(M_PI)*createTransformTranslate(Vector3D(3,4,5)) );
	SINGLE_TEST_FRAME( createTransformRotateZ(M_PI)*createTransformTranslate(Vector3D(3,4,5)) );
}
