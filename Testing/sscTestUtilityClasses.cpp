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
#include "sscSharedMemory.h"

using namespace ssc;

void TestUtilityClasses::setUp()
{
}

void TestUtilityClasses::tearDown()
{
}

void TestUtilityClasses::testTransform3DAccess()
{
	Transform3D t = createTransformRotateY(M_PI/4)*createTransformRotateX(M_PI/3)*createTransformTranslate(Vector3D(3,4,5));
	//const Transform3D ct = t;
	
	for (unsigned i=0; i<4; ++i)
	{
		for (unsigned j=0; j<4; ++j)
		{
			double val = i*4+j;
			t(i,j) = val;
			//ct[i][j] = val; // does not compile: ok
			//double temp = ct[i][j];  // does compile: ok
			CPPUNIT_ASSERT(similar(val, t(i,j)));
		}
	}
}

void TestUtilityClasses::singleTestFrame(const Transform3D& transform)
{
	Frame3D frame = Frame3D::create(transform);
	Transform3D restored = frame.transform();
	
	boost::array<double, 6> rep = frame.getCompactAxisAngleRep();
	Transform3D restored_rep = Frame3D::fromCompactAxisAngleRep(rep).transform();

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
  CPPUNIT_ASSERT(similar(transform, restored_rep));
}

void TestUtilityClasses::singleTestFrameRotationAxis(const Vector3D& k)
{
	Frame3D frame;
	frame.setRotationAxis(k);
	CPPUNIT_ASSERT(similar(k, frame.rotationAxis()));
	CPPUNIT_ASSERT(similar(frame.rotationAxis().normal(), frame.rotationAxis()));	
}

//#define SINGLE_TEST_FRAME(expr) std::cout << "testing: " << # expr << std::endl; singleTestFrame(expr);
#define SINGLE_TEST_FRAME(expr) singleTestFrame(expr);

void TestUtilityClasses::testFrame()
{
	singleTestFrameRotationAxis((ssc::createTransformRotateZ(M_PI/4)).vector(Vector3D(1,0,0)));
	singleTestFrameRotationAxis((ssc::createTransformRotateY(M_PI/4)).vector(Vector3D(1,0,0)));
	singleTestFrameRotationAxis((ssc::createTransformRotateZ(M_PI/4)*ssc::createTransformRotateY(M_PI/4)).vector(Vector3D(1,0,0)));
	
	SINGLE_TEST_FRAME( Transform3D::Identity() );
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

void TestUtilityClasses::testVector3D()
{
  Vector3D a(1,2,3);
  Vector3D b(4,5,6);
  Vector3D e_x(1,0,0);
  Vector3D e_y(0,1,0);
  Vector3D e_z(0,0,1);

  CPPUNIT_ASSERT( similar(a,a) );
  CPPUNIT_ASSERT( !similar(a,b) );

  CPPUNIT_ASSERT( similar( cross(e_x,e_y), e_z ) );
  CPPUNIT_ASSERT( similar( dot(e_x,e_y), 0 ) );
  CPPUNIT_ASSERT( similar( dot(e_x,e_x), 1 ) );

  CPPUNIT_ASSERT( a[0]==1 && a[1]==2 && a[2]==3 );
}

void TestUtilityClasses::testSharedMemory()
{
	for (int i = 1; i < 10; i++)
	{
		SharedMemoryServer srv("test_", i, 100 * i);
		SharedMemoryClient cli;

		bool result = cli.attach(srv.key());
		CPPUNIT_ASSERT( result );
		CPPUNIT_ASSERT( cli.key() == srv.key() );
		CPPUNIT_ASSERT( cli.size() == srv.size() );
		CPPUNIT_ASSERT( cli.buffers() == srv.buffers() );
		void *dst = srv.buffer();
		CPPUNIT_ASSERT( dst != NULL );
		strcpy((char *)dst, "text");
		srv.release();
		srv.release();
		const void *src = cli.buffer();
		CPPUNIT_ASSERT( src != NULL );
		CPPUNIT_ASSERT( strncmp((char *)src, "text", 4) == 0 );
		cli.release();
		cli.release();
	}
}
