#include "cxTest_Example_CppUnit.h"

#include <math.h>
#include "sscTransform3D.h"

void cxTestExamples::setUp()
{
	// this stuff will be performed just before all tests in this class
}

void cxTestExamples::tearDown()
{
	// this stuff will be performed just after all tests in this class
}

/** Test the default constructor of a ssc::Transform3D
 */
void cxTestExamples::testInitialize()
{
	ssc::Transform3D M;
}

/** Test some properties of the identity transform
 */
void cxTestExamples::testIdentityTransform()
{
	ssc::Transform3D M_identity;

	CPPUNIT_ASSERT(similar(M_identity, M_identity.inv()));

	for (unsigned r=0; r<4; ++r)
	{
		for (unsigned c=0; c<4; ++c)
		{
			if (r==c)
				CPPUNIT_ASSERT(similar(M_identity[r][c], 1.0));
			else
				CPPUNIT_ASSERT(similar(M_identity[r][c], 0.0));
		}
	}
}

/** Test the ssc::createTransformTranslate() function
 */
void cxTestExamples::testTransformTranslation()
{
	ssc::Vector3D origin(0,0,0);
	ssc::Vector3D p(3,4,5);
	ssc::Vector3D e_x(1, 0, 0);
	ssc::Vector3D e_y(0, 1, 0);
	ssc::Vector3D e_z(0, 0, 1);

	ssc::Transform3D T = ssc::createTransformTranslate(p);

	CPPUNIT_ASSERT(similar(T.coord(origin), p));
	CPPUNIT_ASSERT(similar(e_x, T.vector(e_x)));
	CPPUNIT_ASSERT(similar(e_y, T.vector(e_y)));
	CPPUNIT_ASSERT(similar(e_z, T.vector(e_z)));
}

/** Test the ssc::createTransformRotateX() function
 */
void cxTestExamples::testTransformXRotation()
{
	ssc::Vector3D origin(0,0,0);
	ssc::Vector3D e_x(1, 0, 0);
	ssc::Vector3D e_y(0, 1, 0);
	ssc::Vector3D e_z(0, 0, 1);

	ssc::Vector3D k_x(1, 0, 0);
	ssc::Vector3D k_y(0, 0, 1);
	ssc::Vector3D k_z(0,-1, 0);

	ssc::Transform3D RX = ssc::createTransformRotateX(M_PI_2);

	CPPUNIT_ASSERT(similar(origin, RX.coord(origin)));
	CPPUNIT_ASSERT(similar(k_x, RX.vector(e_x)));
	CPPUNIT_ASSERT(similar(k_y, RX.vector(e_y)));
	CPPUNIT_ASSERT(similar(k_z, RX.vector(e_z)));
}


void cxTestExamples::testMore()
{

}


