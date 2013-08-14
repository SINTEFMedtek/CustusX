//#define _USE_MATH_DEFINES
#include "sscTestBoundingBox3D.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <math.h>

#include <cppunit/extensions/TestFactoryRegistry.h> 
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "vtkMatrix4x4.h"
#include <QStringList>

#include "sscTestUtilities.h"
#include "sscSliceComputer.h"
#include "sscVector3D.h"
#include "sscTransform3D.h"
#include "sscTypeConversions.h"

using namespace ssc;

void TestBoundingBox3D::testCreate()
{
  double data0[6] = { 0, 1, 0, 1, 0, 1 };
  DoubleBoundingBox3D bb0(data0);
  DoubleBoundingBox3D bb1(data0[0], data0[1], data0[2], data0[3], data0[4], data0[5]);
  DoubleBoundingBox3D bb2(Vector3D(data0[0], data0[2], data0[4]), Vector3D(data0[1], data0[3], data0[5]));

  for (int i=0; i<6; ++i)
    CPPUNIT_ASSERT( similar(bb0[i], data0[i]) );

  CPPUNIT_ASSERT( similar(bb0, bb1) );
  CPPUNIT_ASSERT( similar(bb0, bb2) );
}

void TestBoundingBox3D::testGetters()
{
  double data0[6] = { 0, 1, 0, 1, 0, 1 };
  DoubleBoundingBox3D bb(data0);

  CPPUNIT_ASSERT( similar(bb.center(), Vector3D(0.5,0.5,0.5)) );
  CPPUNIT_ASSERT( similar(bb.range(), Vector3D(1,1,1)) );

  for (int x=0; x<2; ++x)
    for (int y=0; y<2; ++y)
      for (int z=0; z<2; ++z)
        CPPUNIT_ASSERT( similar(bb.corner(x,y,z), Vector3D(x,y,z)) );

  CPPUNIT_ASSERT( similar(bb.bottomLeft(), Vector3D(0,0,0)) );
  CPPUNIT_ASSERT( similar(bb.topRight(), Vector3D(1,1,1)) );
}

void TestBoundingBox3D::testSimilar()
{
  double data0[6] = { 0, 1, 0, 1, 0, 1 };
  double data1[6] = { -1, 1, -1, 1, -1, 1 };

  DoubleBoundingBox3D bb0(data0);
  DoubleBoundingBox3D bb1(data1);

  CPPUNIT_ASSERT( similar(bb0, bb0) );
  CPPUNIT_ASSERT( !similar(bb0, bb1) );
}

void TestBoundingBox3D::testCloud()
{
  std::vector<Vector3D> cloud;
  for (unsigned i=0; i<10; ++i)
    cloud.push_back(Vector3D(i, 2*i, 3*i));

  DoubleBoundingBox3D bb0 = DoubleBoundingBox3D::fromCloud(cloud);
  DoubleBoundingBox3D bb1(0, 9, 0, 18, 0, 27);

  CPPUNIT_ASSERT( similar(bb0, bb1) );
}

void TestBoundingBox3D::testContains()
{
  double data0[6] = { 0, 1, 0, 1, 0, 1 };
  DoubleBoundingBox3D bb0(data0);

  CPPUNIT_ASSERT( bb0.contains(Vector3D( 0.5,  0.5,  0.5)) );
  CPPUNIT_ASSERT( bb0.contains(Vector3D( 0,  0.5,  0.5)) );
  CPPUNIT_ASSERT( bb0.contains(Vector3D( 0.5,  0,  0.5)) );
  CPPUNIT_ASSERT( bb0.contains(Vector3D( 0.5,  0,  0)) );
  CPPUNIT_ASSERT( bb0.contains(Vector3D( 1,  1,  1)) );
  CPPUNIT_ASSERT( bb0.contains(Vector3D( 1,  0,  0)) );
  CPPUNIT_ASSERT( bb0.contains(Vector3D( 0,  1,  0)) );
  CPPUNIT_ASSERT( bb0.contains(Vector3D( 0,  0,  1)) );

  CPPUNIT_ASSERT( !bb0.contains(Vector3D(-0.5,  0.5,  0.5)) );
  CPPUNIT_ASSERT( !bb0.contains(Vector3D( 0.5, -0.5,  0.5)) );
  CPPUNIT_ASSERT( !bb0.contains(Vector3D( 0.5,  0.5, -0.5)) );
  CPPUNIT_ASSERT( !bb0.contains(Vector3D(1.5, 1.5, 1.5)) );
}

void TestBoundingBox3D::testStringConvert()
{
  QString inText("0 1 0 1 0 1");
  DoubleBoundingBox3D bb0 = DoubleBoundingBox3D::fromString(inText);

  double data[6] = {0, 1, 0, 1, 0, 1};
  DoubleBoundingBox3D bb1(data);

  CPPUNIT_ASSERT( similar(bb0, bb1) );

  QString outText = qstring_cast(bb0);

  QStringList inTextList = inText.split(QRegExp("\\s+"), QString::SkipEmptyParts);
  QStringList outTextList = outText.split(QRegExp("\\s+"), QString::SkipEmptyParts);

  CPPUNIT_ASSERT(inTextList.size()==outTextList.size());

  for (int i=0; i<6; ++i)
  {
    CPPUNIT_ASSERT(inTextList[i].toDouble()==outTextList[i].toDouble());
  }
}


