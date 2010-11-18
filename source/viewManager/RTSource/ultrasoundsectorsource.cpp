#include "ultrasoundsectorsource.h"

#include "vtkCellArray.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include <vtkPointData.h>
#include "vtkPolyData.h"
#include <vtkFloatArray.h>

#include "sscVector3D.h"
#include "sscTransform3D.h"
#include "sscBoundingBox3D.h"

using namespace ssc;

vtkCxxRevisionMacro(UltrasoundSectorSource, "$Revision: 3.0 $")
;
vtkStandardNewMacro(UltrasoundSectorSource)
;

UltrasoundSectorSource::UltrasoundSectorSource()
{
}

void UltrasoundSectorSource::Execute()
{
    this->updateSector();
}

void UltrasoundSectorSource::setProbeData(const ssc::ProbeSector& data)
{
  mData = data;
  this->Modified();
}

Transform3D UltrasoundSectorSource::get_tMu() const
{
  Transform3D Rx = ssc::createTransformRotateX(-M_PI/2.0);
  Transform3D Rz = ssc::createTransformRotateY(M_PI/2.0);
  ssc::Transform3D R = (Rx*Rz);
  ssc::Transform3D T = ssc::createTransformTranslate(-mData.mImage.mOrigin_u);

  ssc::Transform3D tMu = R*T;
  return tMu;
}

void UltrasoundSectorSource::updateSector()
{
  vtkPolyData *mPolyData = this->GetOutput();
//  double mDepthStart = 30;
//  double mDepthEnd = 150;
//  double mWidth = M_PI_2;
  //ssc::Vector3D tex(255.5,255.5,1);
  ssc::Vector3D bounds = ssc::Vector3D(mData.mImage.mSize.width(), mData.mImage.mSize.height(), 1);
  bounds = multiply_elems(bounds, mData.mImage.mSpacing);


  vtkFloatArray *newTCoords;

  newTCoords = vtkFloatArray::New();
  newTCoords->SetNumberOfComponents(2);


  //TODO: Merge with USProbe2D in sscVtkHelperClasses?
  ssc::Vector3D p(0,0,0); // tool position in local space
  // first define the shape of the probe in a xy-plane.
  // after that, transform into yz-plane because thats the tool plane (probe point towards positive z)
  // then transform to global space.
  Transform3D tMl = createTransformIJC(Vector3D(0,1,0), Vector3D(0,0,1), Vector3D(0,0,0));
  Transform3D texMu = createTransformNormalize(DoubleBoundingBox3D(0, bounds[0], 0, bounds[1], 0, 1), DoubleBoundingBox3D(0,1,0,1,0,1));
  Transform3D uMt = this->get_tMu().inv();
  Transform3D texMl = texMu * uMt * tMl;
  Transform3D uMl = uMt * tMl;

  Transform3D M = tMl;
  Vector3D e_x = unitVector(0);
  Vector3D e_y = unitVector(M_PI_2);
  Vector3D e_z(0,0,1);

  vtkPointsPtr points = vtkPointsPtr::New();
  vtkCellArrayPtr sides = vtkCellArrayPtr::New();
  vtkCellArrayPtr strips = vtkCellArrayPtr::New();
  vtkCellArrayPtr polys = vtkCellArrayPtr::New();

  if (0)
  {
    Vector3D cr = mData.mDepthStart * e_y + mData.mWidth/2 * e_x;
    Vector3D cl = mData.mDepthStart * e_y - mData.mWidth/2 * e_x;

    Vector3D pr = mData.mDepthEnd * e_y + mData.mWidth/2 * e_x;
    Vector3D pl = mData.mDepthEnd * e_y - mData.mWidth/2 * e_x;

    cl = M.coord(cl);
    cr = M.coord(cr);
    pl = M.coord(pl);
    pr = M.coord(pr);

    points->Allocate(4);
    points->InsertNextPoint(cl.begin());
    points->InsertNextPoint(cr.begin());
    points->InsertNextPoint(pr.begin());
    points->InsertNextPoint(pl.begin());

//    newTCoords->Allocate(2 * 4);
//    newTCoords->InsertTuple((i + j), tc);

    vtkIdType cells[5] = { 0,1,2,3,0};
    sides->InsertNextCell(5, cells);
  }
  else if (1)
  {
    Vector3D c = - mData.mDepthStart * e_y;  // arc center point

    int arcRes = 20;//Number of points in arc
    double angleIncrement = mData.mWidth/arcRes;
    double startAngle = M_PI_2 - mData.mWidth/2.0;
    double stopAngle = M_PI_2 + mData.mWidth/2.0;

    points->Allocate(arcRes*2);//TODO: Don't use the same number of points in top as in bottom?
    newTCoords->Allocate(2 * arcRes*2);

    for(int i = 0; i <= arcRes; i++)
    {
      double theta = startAngle + i*angleIncrement;
      Vector3D startTheta = c + mData.mDepthStart * unitVector(theta);
      newTCoords->InsertNextTuple(texMl.coord(startTheta).begin());
      //startTheta = M.coord(startTheta);
      points->InsertNextPoint(uMl.coord(startTheta).begin());
    }
    for(int i = 0; i <= arcRes; i++)
    {
      double theta = stopAngle - i*angleIncrement;
      Vector3D endTheta = c + mData.mDepthEnd * unitVector(theta);
      newTCoords->InsertNextTuple(texMl.coord(endTheta).begin());
//      endTheta = M.coord(endTheta);
      points->InsertNextPoint(uMl.coord(endTheta).begin());
    }

    sides->InsertNextCell(arcRes*2+2+1);
    for(int i = 0; i < arcRes*2+2; i++)
      sides->InsertCellPoint(i);
    sides->InsertCellPoint(0);

    polys->InsertNextCell(arcRes*2+2+1);
        for(int i = 0; i < arcRes*2+2; i++)
          polys->InsertCellPoint(i);
        polys->InsertCellPoint(0);

  }

  mPolyData->SetPoints(points);
//  mPolyData->SetStrips(strips);
  mPolyData->GetPointData()->SetTCoords(newTCoords);
  mPolyData->SetLines(sides);
  mPolyData->SetPolys(polys);
 // mPolyData->Update();
}

void UltrasoundSectorSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
//  os << indent << "InnerRadius: " << this->InnerRadius << "\n";
}

/*
 * $Log: ultrasoundsectorsource.cpp,v $
 * Revision 1.1  2004/10/12 06:32:23  geirat
 * File created
 *
 */

