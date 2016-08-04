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



