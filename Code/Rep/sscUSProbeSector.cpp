/*
 * sscUSProbeSector.cpp
 *
 *  Created on: Sep 17, 2010
 *      Author: Ole Vegard Solberg
 */

#include "sscUSProbeSector.h""
#include <vtkPolyData.h>
#include <vtkCellArray.h>

namespace ssc
{

USProbeSector::USProbeSector():
    mPolyData(vtkPolyDataPtr::New())
    //mSector(ssc::ProbeSector())
{
}

USProbeSector::~USProbeSector()
{}

void USProbeSector::setSector(ProbeSector sector)
{
  //mSector = sector;
  mType = sector.mType;
  m_d_start = sector.mDepthStart;
  m_d_end = sector.mDepthEnd;
  mWidth = sector.mWidth;
  this->createSector();
}

void USProbeSector::setPosition(const Transform3D& wMt)
{
  m_wMt = wMt;
  this->createSector();
}

namespace // unnamed
{
  /**compute the planar unit vector for a given angle
   */
  Vector3D unitVector(double angle)
  {
    return Vector3D(cos(angle), sin(angle), 0.0);
  }
} // unnamed

void USProbeSector::createSector()
{
  //TODO: Merge with USProbe2D in sscVtkHelperClasses?
  Vector3D p(0,0,0); // tool position in local space
  // first define the shape of the probe in a xy-plane.
  // after that, transform into yz-plane because thats the tool plane (probe point towards positive z)
  // then transform to global space.
  Transform3D tMl = createTransformIJC(Vector3D(0,1,0), Vector3D(0,0,1), Vector3D(0,0,0));

  Transform3D M = m_wMt*tMl;
  Vector3D e_x = unitVector(0);
  Vector3D e_y = unitVector(M_PI_2);
  Vector3D e_z(0,0,1);


  mPoints = vtkPointsPtr::New();
  mSides = vtkCellArrayPtr::New();
  //mPoints->InsertPoint(0, x, y, z);//example
  if (mType == ProbeSector::tLINEAR)
  {
    Vector3D cr = m_d_start * e_y + mWidth/2 * e_x;
    Vector3D cl = m_d_start * e_y - mWidth/2 * e_x;

    Vector3D pr = m_d_end * e_y + mWidth/2 * e_x;
    Vector3D pl = m_d_end * e_y - mWidth/2 * e_x;

    cl = M.coord(cl);
    cr = M.coord(cr);
    pl = M.coord(pl);
    pr = M.coord(pr);

    mPoints->Allocate(4);
    mPoints->InsertNextPoint(cl.begin());
    mPoints->InsertNextPoint(cr.begin());
    mPoints->InsertNextPoint(pr.begin());
    mPoints->InsertNextPoint(pl.begin());

    //vtkIdType cells[5] = { 0,1,2,3,0 };
    vtkIdType cells[5] = { 0,1,2,3,4 };//TODO: check
    mSides->InsertNextCell(5, cells);
//    mStrips->InsertCellPoint(0);
//    mStrips->InsertCellPoint(1);
//    mStrips->InsertCellPoint(2);
//    mStrips->InsertCellPoint(3);
  }
  else if (mType == ProbeSector::tSECTOR)
  {

    Vector3D c = - m_d_start * e_y;  // arc center point
    c = M.coord(c);
    /*Vector3D sr = c + m_d_start * unitVector(M_PI_2 - mWidth/2.0); // left startpoint
    Vector3D sl = c + m_d_start * unitVector(M_PI_2 + mWidth/2.0); // right startpoint
    Vector3D pr = c + m_d_end * unitVector(M_PI_2 - mWidth/2.0); // left endpoint
    Vector3D pl = c + m_d_end * unitVector(M_PI_2 + mWidth/2.0); // right endpoint
//    double re = - m_d_end / (pl-pr).length();   // normalize on distance between endpoints (vtk spec)
//    double rs = - m_d_start / (sl-sr).length(); // normalize on distance between endpoints (vtk spec)

    c = M.coord(c);
    pr = M.coord(pr);
    pl = M.coord(pl);
    sr = M.coord(sr);
    sl = M.coord(sl);*/

    int arcRes = 20;//Number of points in arc
    double angleIncrement = mWidth/arcRes;
    double startAngle = M_PI_2 - mWidth/2.0;

    mPoints->Allocate(arcRes*2);//TODO: Don't use the same number of points in top as in bottom?
    for(int i = 0; i <= arcRes; i++)
    {
      double theta = startAngle + i*angleIncrement;
      Vector3D startTheta = c + m_d_start * unitVector(theta);
      Vector3D endTheta = c + m_d_end * unitVector(theta);

      startTheta = M.coord(startTheta);
      endTheta = M.coord(endTheta);

      mPoints->InsertNextPoint(startTheta.begin());
      mPoints->InsertNextPoint(endTheta.begin());
    }

    mSides->InsertNextCell(arcRes*2+2);
    for(int i = 0; i < arcRes*2+2; i++)
      mSides->InsertCellPoint(i);
  }

  mPolyData->SetPoints(mPoints);
  mPolyData->SetStrips(mSides);
  mPolyData->Update();
}

vtkPolyDataPtr USProbeSector::getPolyData()
{
  return mPolyData;
}

}//namespace ssc
