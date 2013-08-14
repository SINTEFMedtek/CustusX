//#define _USE_MATH_DEFINES
#include "sscTestTransform3D.h"

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
#include "sscFrame3D.h"
#include "sscTypeConversions.h"

using namespace ssc;

void TestTransform3D::testIdentityTransformHasNoEffectOnVector()
{
  Transform3D M = Transform3D::Identity();
  Vector3D v(0,0,0);
  Vector3D w(1,1,1);

  CPPUNIT_ASSERT( similar(M.coord(v), v) );
  CPPUNIT_ASSERT( similar(M.coord(w), w) );

//  std::cout << M.matrix().format(Eigen::IOFormat(6, 0, " ", " ", "","","","")) << std::endl;
//  std::cout << M.matrix() << std::endl;
//  std::cout << M << std::endl;
}


void TestTransform3D::testInsertedElementsEqualsAccessedElements()
{
  double data[16] = {1, 0, 0, M_PI,
                     0, 1, 0, 4,
                     0, 0, 1, 5,
                     0, 0, 0, 1 };
  Transform3D M(data);

//  std::cout << "M:\n" << M << std::endl;
//  std::cout << "M.matrix():\n" << M.matrix() << std::endl;

  CPPUNIT_ASSERT( M(0,0)==1.0 ); CPPUNIT_ASSERT( M(0,1)==0.0 ); CPPUNIT_ASSERT( M(0,2)==0.0 ); CPPUNIT_ASSERT( M(0,3)==M_PI );
  CPPUNIT_ASSERT( M(1,0)==0.0 ); CPPUNIT_ASSERT( M(1,1)==1.0 ); CPPUNIT_ASSERT( M(1,2)==0.0 ); CPPUNIT_ASSERT( M(1,3)==4.0 );
  CPPUNIT_ASSERT( M(2,0)==0.0 ); CPPUNIT_ASSERT( M(2,1)==0.0 ); CPPUNIT_ASSERT( M(2,2)==1.0 ); CPPUNIT_ASSERT( M(2,3)==5.0 );
  CPPUNIT_ASSERT( M(3,0)==0.0 ); CPPUNIT_ASSERT( M(3,1)==0.0 ); CPPUNIT_ASSERT( M(3,2)==0.0 ); CPPUNIT_ASSERT( M(3,3)==1.0 );
}

void TestTransform3D::testEqualTransformsAreSimilar()
{
  Transform3D M0 = createTransformRotateX(M_PI/5) * createTransformTranslate(Vector3D(3,4,5));

  CPPUNIT_ASSERT( similar(M0, M0) );
}
void TestTransform3D::testDifferentTransformsAreNotSimilar()
{
  Transform3D M0 = createTransformRotateX(M_PI/5) * createTransformTranslate(Vector3D(3,4,5));
  Transform3D M1 = createTransformRotateY(M_PI/6) * createTransformTranslate(Vector3D(6,7,8));

  CPPUNIT_ASSERT( !similar(M0, M1) );
}

void TestTransform3D::testTranslate()
{
  Transform3D M1 = createTransformTranslate(Vector3D(3,4,5));

  double data[16] = {1, 0, 0, 3,
                     0, 1, 0, 4,
                     0, 0, 1, 5,
                     0, 0, 0, 1 };
  Transform3D M2(data);

  CPPUNIT_ASSERT( similar(M1, M2) );
}

void TestTransform3D::testRotate()
{
  Transform3D M1 = createTransformRotateX(M_PI/2.0);
//  std::cout << "\n" << M << std::endl;

  double data[16] = {1, 0, 0, 0,
                     0, 0,-1, 0,
                     0, 1, 0, 0,
                     0, 0, 0, 1 };
  Transform3D M2(data);

  CPPUNIT_ASSERT( similar(M1, M2) );
}

void TestTransform3D::testVtkConvert()
{
  Transform3D M1 = createTransformRotateX(M_PI/2.0)*createTransformTranslate(Vector3D(3,4,5));
  vtkMatrix4x4Ptr Mvtk = M1.getVtkMatrix();

  for (int r=0; r<4; ++r)
    for (int c=0; c<4; ++c)
      CPPUNIT_ASSERT( similar(M1(r,c), Mvtk->GetElement(r,c)) );

  Transform3D M2 = Transform3D::fromVtkMatrix(Mvtk);
  CPPUNIT_ASSERT( similar(M1, M2) );

}

void TestTransform3D::testStringConvert()
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

  CPPUNIT_ASSERT( similar(M1, M2) );

//  QString outText = M1.toString();
  QString outText = qstring_cast(M1);
//  std::cout << outText << std::endl;

  QStringList inTextList = inText.split(QRegExp("\\s+"), QString::SkipEmptyParts);
  QStringList outTextList = outText.split(QRegExp("\\s+"), QString::SkipEmptyParts);

  CPPUNIT_ASSERT(inTextList.size()==outTextList.size());

  for (int i=0; i<16; ++i)
  {
//    std::cout << "g " << inTextList[i] << " - " << outTextList[i] << std::endl;
    CPPUNIT_ASSERT(inTextList[i].toDouble()==outTextList[i].toDouble());
  }
}

void TestTransform3D::testDoTransform()
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
  CPPUNIT_ASSERT( similar(T.coord(zero), offset) );
  CPPUNIT_ASSERT( similar(R.coord(zero), zero) );
  CPPUNIT_ASSERT( similar(M.coord(zero), offset) );

  // transformation of free vectors [x y z 0] homogenous
  CPPUNIT_ASSERT( similar(T.vector(e_z), e_z) );
  CPPUNIT_ASSERT( similar(R.vector(e_z), e_z_rotx90) );
  CPPUNIT_ASSERT( similar(M.vector(e_z), e_z_rotx90) );

//  std::cout << "(S*T).unitVector(e_z) " << (S*T).unitVector(e_z) << std::endl;
//  std::cout << "e_z " << e_z << std::endl;
//  std::cout << "T " << T << std::endl;
//  std::cout << "S " << S << std::endl;
//  std::cout << "S*T " << S*T << std::endl;
//  std::cout << "similar((S*R).unitVector(e_z) " << (S*R).unitVector(e_z) << std::endl;
//  std::cout << "e_z_rotx90 " << e_z_rotx90 << std::endl;

  // transformation of free vectors [x y z 0] homogenous, but force |n|=1
  CPPUNIT_ASSERT( similar((S*T).unitVector(e_z), e_z) );
  CPPUNIT_ASSERT( similar((S*R).unitVector(e_z), e_z_rotx90) );
  CPPUNIT_ASSERT( similar((S*M).unitVector(e_z), e_z_rotx90) );
}

void TestTransform3D::testInverse()
{
  Vector3D p = Vector3D(1,2,3);
  Transform3D M0 = createTransformRotateX(M_PI/5) * createTransformTranslate(Vector3D(3,4,5));
  Vector3D q = M0.coord(p);

  CPPUNIT_ASSERT( !similar(p, q) );
  CPPUNIT_ASSERT( similar(p, M0.inv().coord(q)) );
}

void TestTransform3D::testMultiply()
{
  Vector3D p = Vector3D(1,2,3);

  Transform3D M0 = createTransformRotateX(M_PI/5) * createTransformTranslate(Vector3D(3,4,5));
  Transform3D M1 = createTransformRotateY(M_PI/6) * createTransformTranslate(Vector3D(6,7,8));

  // simple test: chaining transforms equals transforming with the product
  CPPUNIT_ASSERT( similar( (M1*M0).coord(p), M1.coord(M0.coord(p)) ) );
}
