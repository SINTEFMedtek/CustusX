/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "vtkMatrix4x4.h"
#include <QStringList>

#include "cxVector3D.h"
#include "cxTransform3D.h"
#include "cxTypeConversions.h"
#include "cxBoundingBox3D.h"

#include "catch.hpp"

using namespace cx;

TEST_CASE("DoubleBoundingBox3D: Constructors OK", "[unit][resource][core]")
{
  double data0[6] = { 0, 1, 0, 1, 0, 1 };
  DoubleBoundingBox3D bb0(data0);
  DoubleBoundingBox3D bb1(data0[0], data0[1], data0[2], data0[3], data0[4], data0[5]);
  DoubleBoundingBox3D bb2(Vector3D(data0[0], data0[2], data0[4]), Vector3D(data0[1], data0[3], data0[5]));

  for (int i=0; i<6; ++i)
	CHECK( similar(bb0[i], data0[i]) );

  CHECK( similar(bb0, bb1) );
  CHECK( similar(bb0, bb2) );
}

TEST_CASE("DoubleBoundingBox3D: Getters OK", "[unit][resource][core]")
{
  double data0[6] = { 0, 1, 0, 1, 0, 1 };
  DoubleBoundingBox3D bb(data0);

  CHECK( similar(bb.center(), Vector3D(0.5,0.5,0.5)) );
  CHECK( similar(bb.range(), Vector3D(1,1,1)) );

  for (int x=0; x<2; ++x)
	for (int y=0; y<2; ++y)
	  for (int z=0; z<2; ++z)
		CHECK( similar(bb.corner(x,y,z), Vector3D(x,y,z)) );

  CHECK( similar(bb.bottomLeft(), Vector3D(0,0,0)) );
  CHECK( similar(bb.topRight(), Vector3D(1,1,1)) );
}

TEST_CASE("DoubleBoundingBox3D: Similar OK", "[unit][resource][core]")
{
  double data0[6] = { 0, 1, 0, 1, 0, 1 };
  double data1[6] = { -1, 1, -1, 1, -1, 1 };

  DoubleBoundingBox3D bb0(data0);
  DoubleBoundingBox3D bb1(data1);

  CHECK( similar(bb0, bb0) );
  CHECK( !similar(bb0, bb1) );
}

TEST_CASE("DoubleBoundingBox3D: Can be created from a point cloud", "[unit][resource][core]")
{
  std::vector<Vector3D> cloud;
  for (unsigned i=0; i<10; ++i)
	cloud.push_back(Vector3D(i, 2*i, 3*i));

  DoubleBoundingBox3D bb0 = DoubleBoundingBox3D::fromCloud(cloud);
  DoubleBoundingBox3D bb1(0, 9, 0, 18, 0, 27);

  CHECK( similar(bb0, bb1) );
}

TEST_CASE("DoubleBoundingBox3D: Contains OK", "[unit][resource][core]")
{
  double data0[6] = { 0, 1, 0, 1, 0, 1 };
  DoubleBoundingBox3D bb0(data0);

  CHECK( bb0.contains(Vector3D( 0.5,  0.5,  0.5)) );
  CHECK( bb0.contains(Vector3D( 0,  0.5,  0.5)) );
  CHECK( bb0.contains(Vector3D( 0.5,  0,  0.5)) );
  CHECK( bb0.contains(Vector3D( 0.5,  0,  0)) );
  CHECK( bb0.contains(Vector3D( 1,  1,  1)) );
  CHECK( bb0.contains(Vector3D( 1,  0,  0)) );
  CHECK( bb0.contains(Vector3D( 0,  1,  0)) );
  CHECK( bb0.contains(Vector3D( 0,  0,  1)) );

  CHECK( !bb0.contains(Vector3D(-0.5,  0.5,  0.5)) );
  CHECK( !bb0.contains(Vector3D( 0.5, -0.5,  0.5)) );
  CHECK( !bb0.contains(Vector3D( 0.5,  0.5, -0.5)) );
  CHECK( !bb0.contains(Vector3D(1.5, 1.5, 1.5)) );
}

TEST_CASE("DoubleBoundingBox3D: Convert to QString and back", "[unit][resource][core]")
{
  QString inText("0 1 0 1 0 1");
  DoubleBoundingBox3D bb0 = DoubleBoundingBox3D::fromString(inText);

  double data[6] = {0, 1, 0, 1, 0, 1};
  DoubleBoundingBox3D bb1(data);

  CHECK( similar(bb0, bb1) );

  QString outText = qstring_cast(bb0);

  QStringList inTextList = inText.split(QRegExp("\\s+"), QString::SkipEmptyParts);
  QStringList outTextList = outText.split(QRegExp("\\s+"), QString::SkipEmptyParts);

  CHECK(inTextList.size()==outTextList.size());

  for (int i=0; i<6; ++i)
  {
	CHECK(inTextList[i].toDouble()==outTextList[i].toDouble());
  }
}

TEST_CASE("DoubleBoundingBox3D: intersection", "[unit][resource][core]")
{
  DoubleBoundingBox3D bb0(0, 1, 0, 1, 0, 1);
  DoubleBoundingBox3D bb1(0.5, 1.5, -0.5, 0.5, -5, 5);
  DoubleBoundingBox3D bb2(2, 3, 2, 3, 0, 1);

  DoubleBoundingBox3D bb01 = intersection(bb0, bb1);
  CHECK(similar(bb01, DoubleBoundingBox3D(0.5, 1, 0, 0.5, 0, 1)));

  DoubleBoundingBox3D bb02 = intersection(bb0, bb2);
  CHECK(similar(bb02, DoubleBoundingBox3D::zero()));
}



