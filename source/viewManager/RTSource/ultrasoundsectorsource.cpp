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
using ssc::Vector3D;
using ssc::Transform3D;
using ssc::unitVector;

//#include "../datahandling/ultrasoundSector.h"
//#include "../toolbox.h"
//#include "../math/suvecmat.h"

vtkCxxRevisionMacro(UltrasoundSectorSource, "$Revision: 1.1 $")
;
vtkStandardNewMacro(UltrasoundSectorSource)
;

UltrasoundSectorSource::UltrasoundSectorSource()
{
  this->InnerRadius = 10;
  this->OuterRadius = 30;
  this->RadialResolution = 1;
  this->CircumferentialResolution = 4;
}

void UltrasoundSectorSource::Execute()
{
  vtkIdType numPolys, numPts;
  double x[3], tc[2];
  int i, j;
  //vtkIdType pts[4];
  double theta, deltaRadius;
  double cosTheta, sinTheta;
  vtkPoints *newPoints;
  vtkFloatArray *newNormals;
  vtkFloatArray *newTCoords;
  vtkCellArray *newPolys;
  vtkPolyData *output = this->GetOutput();
  vtkCellArray* strips = vtkCellArray::New();

  //printf("***UltrasoundSectorSource::Execute()\n");
  // Ultrasound sector data
  float openingAngle, startAngle, incrementAngle, depthCm, resolution;
  int sectorStart;
  /// values hardcoded:
  // usSectorPixel sectorOrigin;
  int probeCalibrated = true;
  openingAngle = 90;
  depthCm = 25;
  resolution = 20;
  sectorStart = 50;
  double sectorOrigin_x = 250;
  double sectorOrigin_y = 0;
  //  sectorOrigin = ssc::Vector3D(,,);
  ///


  //  if(Toolbox::GetBookkeeper()->GetUSsectorData() == NULL || Toolbox::GetBookkeeper() == NULL)
  //    goto returnLabel;

  if (probeCalibrated)
  {
    this->InnerRadius = ((double) sectorStart - sectorOrigin_y) * (double) resolution;
    this->OuterRadius = this->InnerRadius + (double) depthCm * 10.0;

    startAngle = 90.0 - (openingAngle / 2.0);
    incrementAngle = openingAngle / this->CircumferentialResolution;
  }
  // Set things up; allocate memory
  //
  double normal[3];
  normal[0] = -1.0;
  normal[1] = 0;
  normal[2] = 0;

  numPts = (this->RadialResolution + 1) * (this->CircumferentialResolution + 1);
  newPoints = vtkPoints::New();
  newPoints->Allocate(numPts);
  //newNormals = vtkFloatArray::New();
  //newNormals->SetNumberOfComponents(3);
  //newNormals->Allocate(3*numPts);
  newTCoords = vtkFloatArray::New();
  newTCoords->SetNumberOfComponents(2);
  newTCoords->Allocate(2 * numPts);

  bool useCAWay = true;
  if (useCAWay)
  {
    updateSector();
    return;
  }
  abort();

  if (probeCalibrated)
  {
    for (i = 0; i < (this->CircumferentialResolution); i++)
    {
      theta = startAngle + i * incrementAngle;
      theta = theta * vtkMath::DegreesToRadians();
      deltaRadius = (this->OuterRadius - this->InnerRadius) / ((double) this->RadialResolution);
      //printf("UltrasoundSectorSource i : %d - theta : %f\n",i,(theta*vtkMath::RadiansToDegrees()));

      cosTheta = cos((double) theta);
      sinTheta = sin((double) theta);
      for (j = 0; j <= this->RadialResolution; j++)
      {
        // Vertex coordinates
        x[0] = ((this->InnerRadius + j * deltaRadius) * cosTheta);// + horizontalOffset;
        x[1] = (this->InnerRadius + j * deltaRadius) * sinTheta;
        x[2] = 0.0;
        // Texture coordinates

        if (j == 0)
        {
          tc[0] = (sectorOrigin_x - cosTheta * this->InnerRadius) / TEXTURESIZE;
          tc[1] = (sectorOrigin_x - sinTheta * this->InnerRadius) / TEXTURESIZE;
        }
        else
        {
          tc[0] = (sectorOrigin_x - cosTheta * this->OuterRadius);// / TEXTURESIZE;
          tc[1] = (sectorOrigin_x - sinTheta * this->OuterRadius);// / TEXTURESIZE;
        }

        newPoints->InsertNextPoint(x);
        newTCoords->InsertTuple((i + j), tc);
        //newNormals->InsertTuple(i++,normal);
      }
    }

    //  Create connectivity
    //
    // VTK_TRIANGLE_STRIP
    strips->InsertNextCell(this->CircumferentialResolution * 2);
    for (i = 0; i < (this->CircumferentialResolution * 2); i++)
    {
      strips->InsertCellPoint(i);
    }
  }
  // Update ourselves and release memory
  //
  output->SetPoints(newPoints);
  output->SetStrips(strips);
  newPoints->Delete();

  //output->GetPointData()->SetNormals(newNormals);
  //newNormals->Delete();

  output->GetPointData()->SetTCoords(newTCoords);
  newTCoords->Delete();

  returnLabel: i = 0;
}

void UltrasoundSectorSource::setProbeData(const ssc::ProbeSector& data)
{
  mData = data;
  this->Modified();
}

void UltrasoundSectorSource::updateSector()
{
  vtkPolyData *mPolyData = this->GetOutput();
//  double mDepthStart = 30;
//  double mDepthEnd = 150;
//  double mWidth = M_PI_2;
  ssc::Vector3D tex(100,100,1);

  vtkFloatArray *newTCoords;

  newTCoords = vtkFloatArray::New();
  newTCoords->SetNumberOfComponents(2);


  //TODO: Merge with USProbe2D in sscVtkHelperClasses?
  ssc::Vector3D p(0,0,0); // tool position in local space
  // first define the shape of the probe in a xy-plane.
  // after that, transform into yz-plane because thats the tool plane (probe point towards positive z)
  // then transform to global space.
  Transform3D tMl = createTransformIJC(Vector3D(0,1,0), Vector3D(0,0,1), Vector3D(0,0,0));

  Transform3D M;// = tMl;
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
      newTCoords->InsertNextTuple(divide_elems(startTheta, tex).begin());
      startTheta = M.coord(startTheta);
      points->InsertNextPoint(startTheta.begin());
    }
    for(int i = 0; i <= arcRes; i++)
    {
      double theta = stopAngle - i*angleIncrement;
      Vector3D endTheta = c + mData.mDepthEnd * unitVector(theta);
      newTCoords->InsertNextTuple(divide_elems(endTheta, tex).begin());
      endTheta = M.coord(endTheta);
      points->InsertNextPoint(endTheta.begin());
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

  os << indent << "InnerRadius: " << this->InnerRadius << "\n";
  os << indent << "OuterRadius: " << this->OuterRadius << "\n";
  os << indent << "RadialResolution: " << this->RadialResolution << "\n";
  os << indent << "CircumferentialResolution: " << this->CircumferentialResolution << "\n";
}

/*
 * $Log: ultrasoundsectorsource.cpp,v $
 * Revision 1.1  2004/10/12 06:32:23  geirat
 * File created
 *
 */

