// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxVector3D.h"
#include "catch.hpp"

using namespace cx;

TEST_CASE("Vector3D works", "[unit][resource][core]")
{
  Vector3D a(1,2,3);
  Vector3D b(4,5,6);
  Vector3D e_x(1,0,0);
  Vector3D e_y(0,1,0);
  Vector3D e_z(0,0,1);

  CHECK( similar(a,a) );
  CHECK( !similar(a,b) );

  CHECK( similar( cross(e_x,e_y), e_z ) );
  CHECK( similar( dot(e_x,e_y), 0 ) );
  CHECK( similar( dot(e_x,e_x), 1 ) );

  CHECK( a[0]==1 );
  CHECK( a[1]==2 );
  CHECK( a[2]==3 );
}



