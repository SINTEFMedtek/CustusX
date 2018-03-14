/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

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



