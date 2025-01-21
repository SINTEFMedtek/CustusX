/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxTransform3D.h"
#include "vtkMatrix4x4.h"
#include "cxTypeConversions.h"
#include "catch.hpp"

using namespace cx;

TEST_CASE("Transform3D: Element access", "[unit][resource][core]")
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
			CHECK(similar(val, t(i,j)));
		}
	}
}

TEST_CASE("Transform3D: Identity transform has no effect on vector", "[unit][resource][core]")
{
  Transform3D M = Transform3D::Identity();
  Vector3D v(0,0,0);
  Vector3D w(1,1,1);

  CHECK( similar(M.coord(v), v) );
  CHECK( similar(M.coord(w), w) );

//  std::cout << M.matrix().format(Eigen::IOFormat(6, 0, " ", " ", "","","","")) << std::endl;
//  std::cout << M.matrix() << std::endl;
//  std::cout << M << std::endl;
}

TEST_CASE("Transform3D: Inserted elements equals accessed elements", "[unit][resource][core]")
{
  double data[16] = {1, 0, 0, M_PI,
					 0, 1, 0, 4,
					 0, 0, 1, 5,
					 0, 0, 0, 1 };
  Transform3D M(data);

//  std::cout << "M:\n" << M << std::endl;
//  std::cout << "M.matrix():\n" << M.matrix() << std::endl;

  CHECK( M(0,0)==1.0 ); CHECK( M(0,1)==0.0 ); CHECK( M(0,2)==0.0 ); CHECK( M(0,3)==M_PI );
  CHECK( M(1,0)==0.0 ); CHECK( M(1,1)==1.0 ); CHECK( M(1,2)==0.0 ); CHECK( M(1,3)==4.0 );
  CHECK( M(2,0)==0.0 ); CHECK( M(2,1)==0.0 ); CHECK( M(2,2)==1.0 ); CHECK( M(2,3)==5.0 );
  CHECK( M(3,0)==0.0 ); CHECK( M(3,1)==0.0 ); CHECK( M(3,2)==0.0 ); CHECK( M(3,3)==1.0 );
}

TEST_CASE("Transform3D: Equal transforms are similar", "[unit][resource][core]")
{
  Transform3D M0 = createTransformRotateX(M_PI/5) * createTransformTranslate(Vector3D(3,4,5));

  CHECK( similar(M0, M0) );
}

TEST_CASE("Transform3D: Different transforms are not similar", "[unit][resource][core]")
{
  Transform3D M0 = createTransformRotateX(M_PI/5) * createTransformTranslate(Vector3D(3,4,5));
  Transform3D M1 = createTransformRotateY(M_PI/6) * createTransformTranslate(Vector3D(6,7,8));

  CHECK( !similar(M0, M1) );
}

TEST_CASE("Transform3D: createTransformTranslate", "[unit][resource][core]")
{
  Transform3D M1 = createTransformTranslate(Vector3D(3,4,5));

  double data[16] = {1, 0, 0, 3,
					 0, 1, 0, 4,
					 0, 0, 1, 5,
					 0, 0, 0, 1 };
  Transform3D M2(data);

  CHECK( similar(M1, M2) );
}

TEST_CASE("Transform3D: createTransformRotate", "[unit][resource][core]")
{
  Transform3D M1 = createTransformRotateX(M_PI/2.0);
//  std::cout << "\n" << M << std::endl;

  double data[16] = {1, 0, 0, 0,
					 0, 0,-1, 0,
					 0, 1, 0, 0,
					 0, 0, 0, 1 };
  Transform3D M2(data);

  CHECK( similar(M1, M2) );
}

TEST_CASE("Transform3D: Convert to vtkMatrix4x4 and back", "[unit][resource][core]")
{
  Transform3D M1 = createTransformRotateX(M_PI/2.0)*createTransformTranslate(Vector3D(3,4,5));
  vtkMatrix4x4Ptr Mvtk = M1.getVtkMatrix();

  for (int r=0; r<4; ++r)
	for (int c=0; c<4; ++c)
	  CHECK( similar(M1(r,c), Mvtk->GetElement(r,c)) );

  Transform3D M2 = Transform3D::fromVtkMatrix(Mvtk);
  CHECK( similar(M1, M2) );
}

TEST_CASE("Transform3D: Convert to QString and back", "[unit][resource][core]")
{
  QString inText("1 0 0 3 "
				 "0 1 0 4 "
				 "0 0 1 5 "
				 "0 0 0 1 ");
  Transform3D M1 = Transform3D::fromString(inText);

  double data[16] = {1, 0, 0, 3,
					 0, 1, 0, 4,
					 0, 0, 1, 5,
					 0, 0, 0, 1 };
  Transform3D M2(data);

//  std::cout << "M1\n" << M1 << std::endl;
//  std::cout << "M2\n" << M2 << std::endl;
//  std::cout << inText << std::endl;

  CHECK( similar(M1, M2) );

//  QString outText = M1.toString();
  QString outText = qstring_cast(M1);
//  std::cout << outText << std::endl;

  QStringList inTextList = inText.split(QRegExp("\\s+"), QString::SkipEmptyParts);
  QStringList outTextList = outText.split(QRegExp("\\s+"), QString::SkipEmptyParts);

  CHECK(inTextList.size()==outTextList.size());

  for (int i=0; i<16; ++i)
  {
//    std::cout << "g " << inTextList[i] << " - " << outTextList[i] << std::endl;
	CHECK(inTextList[i].toDouble()==outTextList[i].toDouble());
  }
}

TEST_CASE("Transform3D: Free vectors and points are correctly transformed", "[unit][resource][core]")
{
  Vector3D offset = Vector3D(3,4,5);

  Transform3D T = createTransformTranslate(offset);
  Transform3D R = createTransformRotateX(M_PI/2.0);
  Transform3D S = createTransformScale(Vector3D(2,2,2));
  Transform3D M = T*R;

  Vector3D zero(0,0,0);
  Vector3D e_z(0,0,1);
  Vector3D e_z_rotx90(0,-1,0);

  // transformation of coordinate points [x y z 1] homogenous
  CHECK( similar(T.coord(zero), offset) );
  CHECK( similar(R.coord(zero), zero) );
  CHECK( similar(M.coord(zero), offset) );

  // transformation of free vectors [x y z 0] homogenous
  CHECK( similar(T.vector(e_z), e_z) );
  CHECK( similar(R.vector(e_z), e_z_rotx90) );
  CHECK( similar(M.vector(e_z), e_z_rotx90) );

//  std::cout << "(S*T).unitVector(e_z) " << (S*T).unitVector(e_z) << std::endl;
//  std::cout << "e_z " << e_z << std::endl;
//  std::cout << "T " << T << std::endl;
//  std::cout << "S " << S << std::endl;
//  std::cout << "S*T " << S*T << std::endl;
//  std::cout << "similar((S*R).unitVector(e_z) " << (S*R).unitVector(e_z) << std::endl;
//  std::cout << "e_z_rotx90 " << e_z_rotx90 << std::endl;

  // transformation of free vectors [x y z 0] homogenous, but force |n|=1
  CHECK( similar((S*T).unitVector(e_z), e_z) );
  CHECK( similar((S*R).unitVector(e_z), e_z_rotx90) );
  CHECK( similar((S*M).unitVector(e_z), e_z_rotx90) );
}

TEST_CASE("Transform3D: Can be inverted", "[unit][resource][core]")
{
  Vector3D p = Vector3D(1,2,3);
  Transform3D M0 = createTransformRotateX(M_PI/5) * createTransformTranslate(Vector3D(3,4,5));
  Vector3D q = M0.coord(p);

  CHECK( !similar(p, q) );
  CHECK( similar(p, M0.inv().coord(q)) );
}

TEST_CASE("Transform3D: Can be multiplied", "[unit][resource][core]")
{
  Vector3D p = Vector3D(1,2,3);

  Transform3D M0 = createTransformRotateX(M_PI/5) * createTransformTranslate(Vector3D(3,4,5));
  Transform3D M1 = createTransformRotateY(M_PI/6) * createTransformTranslate(Vector3D(6,7,8));

  // simple test: chaining transforms equals transforming with the product
  CHECK( similar( (M1*M0).coord(p), M1.coord(M0.coord(p)) ) );
}


