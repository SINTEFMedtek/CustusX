#include "sscProbeSector.h"


#include "vtkImageData.h"
#include <vtkPointData.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include <vtkFloatArray.h>
#include <vtkPolyLine.h>
#include <vtkClipPolyData.h>
#include <vtkBox.h>
#include <vtkPlane.h>
#include <vtkPlanes.h>
#include <vtkCutter.h>
#include <vtkAppendPolyData.h>
//#include <vtkPolyPlane.h>
#include "sscBoundingBox3D.h"
#include "sscVolumeHelpers.h"


typedef vtkSmartPointer<class vtkPlanes> vtkPlanesPtr;
typedef vtkSmartPointer<class vtkPlane> vtkPlanePtr;
typedef vtkSmartPointer<class vtkBox> vtkBoxPtr;
typedef vtkSmartPointer<class vtkCutter> vtkCutterPtr;
typedef vtkSmartPointer<class vtkAppendPolyData> vtkAppendPolyDataPtr;
typedef vtkSmartPointer<class vtkFloatArray> vtkFloatArrayPtr;


namespace ssc
{

ProbeSector::ProbeSector()// : mType(ProbeSector::tNONE)
{

//  // testdata:
//  mData.mType = ProbeSector::tSECTOR;
//  mData.mDepthStart = 0;
////  mDepthEnd = 350;
//  mData.mDepthEnd = 300;
//  mData.mWidth = M_PI/4;
//  mData.mImage.mSpacing = Vector3D(0.928,0.928,1);
////  mSpacing = Vector3D(1,1,1); // using this spacing gives correct image - investigate!
//  mData.mImage.mSpacing = Vector3D(0.5,0.5,0);
//  mData.mImage.mSize = QSize(512,512);
//
//  //mOrigin_u = multiply_elems(Vector3D(mSize.width()/2, mSize.height()*0.75, 0), mSpacing);
//  mData.mImage.mOrigin_u = multiply_elems(Vector3D(mData.mImage.mSize.width()/2, mData.mImage.mSize.height()*1.0, 0), mData.mImage.mSpacing);

  //mCachedCenter_v = this->get_uMv().inv().coord(mOrigin_u) - mDepthStart * Vector3D(0,1,0);
  mPolyData = vtkPolyDataPtr::New();
}

void ProbeSector::setData(ProbeData data)
{
  mData = data;
}

/**Function object for evaluating whether a pixel is inside the
 * us mask.
 */
class InsideMaskFunctor
{
public:
  InsideMaskFunctor(ProbeData data, Transform3D uMv) :
    mData(data),
    m_vMu(uMv.inv())
  {
    mCachedCenter_v = m_vMu.coord(mData.mImage.getOrigin_u());
    mClipRect_v = transform(m_vMu, mData.mImage.getClipRect_u());
    mClipRect_v[4] = -1;
    mClipRect_v[5] =  1;
  }
  bool operator ()(int x, int y) const
  {
    Vector3D p_v = multiply_elems(Vector3D(x,y,0), mData.mImage.mSpacing);

    return this->insideClipRect(p_v)
        && this->insideSector(p_v);
  }

private:
  /**return true if p_v, given in the upper-left space v,
   * is inside the us beam sector
   *
   * Prerequisite: mCachedCenter_v is updated!
   */
  bool insideClipRect(const Vector3D& p_v) const
  {
    return mClipRect_v.contains(p_v);
  }

  /**return true if p_v, given in the upper-left space v,
   * is inside the us beam sector
   *
   * Prerequisite: mCachedCenter_v is updated!
   */
  bool insideSector(const Vector3D& p_v) const
  {
    Vector3D d = p_v - mCachedCenter_v;

    if (mData.mType==ProbeData::tSECTOR)
    {
      double angle = atan2(d[1], d[0]);
      angle -= M_PI_2; // center angle on us probe axis at 90*.
      if (angle < -M_PI)
        angle += 2.0 * M_PI;

      if (fabs(angle) > mData.mWidth/2.0)
        return false;
      if (d.length() < mData.mDepthStart)
        return false;
      if (d.length() > mData.mDepthEnd)
        return false;
      return true;
    }
    else // tLINEAR
    {
      if (fabs(d[0]) > mData.mWidth/2.0)
        return false;
      if (d[1] < mData.mDepthStart)
        return false;
      if (d[1] > mData.mDepthEnd)
        return false;
      return true;
    }
  }

  ProbeData mData;
  Transform3D m_vMu;
  Vector3D mCachedCenter_v; ///< center of beam sector for sector probes.
  ssc::DoubleBoundingBox3D mClipRect_v;
};

/** Return a 2D mask image identifying the US beam inside the image
 *  data stream.
 */
vtkImageDataPtr ProbeSector::getMask()
{
  InsideMaskFunctor checkInside(mData, this->get_uMv());
  vtkImageDataPtr retval;
  retval = generateVtkImageData(Vector3D(mData.mImage.mSize.width(),mData.mImage.mSize.height(),1), mData.mImage.mSpacing, 0);

  int* dim(retval->GetDimensions());
  unsigned char* dataPtr = static_cast<unsigned char*>(retval->GetScalarPointer());
  for(int x = 0; x < dim[0]; x++)
    for(int y = 0; y < dim[1]; y++)
    {
      dataPtr[x + y*dim[0]] = checkInside(x,y) ? 1 : 0;
    }

  return retval;
}

void ProbeSector::test()
{
  Transform3D tMu = this->get_tMu();
  Vector3D e_x(1,0,0);
  Vector3D e_y(0,1,0);
  Vector3D e_z(0,0,1);

  // zero = tMu * mOrigin_u
  std::cout << "zero = tMu * mOrigin_u, zero: " << tMu.coord(mData.mImage.getOrigin_u()) << ", mOrigin_u: " << mData.mImage.getOrigin_u() << std::endl;

  // e_z = tMu * -e_y
  std::cout << "e_z = tMu * -e_y " << tMu.vector(-e_y) <<std::endl;

  // e_y = tMu * -e_x
  std::cout << "e_y = tMu * -e_x " << tMu.vector(-e_x) <<std::endl;


  // tMu * e_x
  std::cout << "tMu * e_x " << tMu.vector(e_x) <<std::endl;
  // tMu * e_y
  std::cout << "tMu * e_y " << tMu.vector(e_y) <<std::endl;


}

Transform3D ProbeSector::get_tMu() const
{
  Transform3D Rx = ssc::createTransformRotateX(-M_PI/2.0);
  Transform3D Rz = ssc::createTransformRotateY(-M_PI/2.0);
  ssc::Transform3D R = (Rx*Rz);
  ssc::Transform3D T = ssc::createTransformTranslate(-mData.mImage.getOrigin_u());

  ssc::Transform3D tMu = R*T;
  return tMu;
}

Transform3D ProbeSector::get_uMv() const
{
  // use H-1 because we count between pixel centers.
  double H = (mData.mImage.mSize.height()-1) * mData.mImage.mSpacing[1];
  return createTransformRotateX(M_PI) * createTransformTranslate(Vector3D(0,-H,0));
}

vtkPolyDataPtr ProbeSector::getSector()
{
  this->updateSector();
  return mPolyData;
}

/**Return true is cliprect has any effect on the sector,
 * i.e if the intersection between sector and cliprect is
 * different from sector.
 */
bool ProbeSector::clipRectIntersectsSector() const
{
  DoubleBoundingBox3D s(mPolyData->GetPoints()->GetBounds());
  DoubleBoundingBox3D c = mData.mImage.getClipRect_u();

  bool outside = ( c[0]<s[0] )&&( s[1]<c[1] )&&( c[2]<s[2] )&&( s[3]<c[3] );
  return !outside;
}

vtkPolyDataPtr ProbeSector::getSectorLinesOnly()
{
  if (mData.mType == ProbeData::tNONE)
    return mPolyData;

  this->updateSector();
//  return mPolyData;

  vtkPolyDataPtr output = vtkPolyDataPtr::New();

  output->SetPoints(mPolyData->GetPoints());
  output->SetLines(mPolyData->GetLines());
//  output->SetStrips(mPolyData->GetStrips());
//  return output;

  // also display the cliprect
  vtkAppendPolyDataPtr retval = vtkAppendPolyDataPtr::New();
  retval->AddInput(output);

  if (this->clipRectIntersectsSector())
    retval->AddInput(this->getClipRectPolyData());

  retval->Update();
  return retval->GetOutput();
}


vtkPolyDataPtr clipPlane(vtkPolyDataPtr input, Vector3D p, Vector3D n)
{
  vtkPlanePtr plane = vtkPlanePtr::New();
  plane->SetOrigin(p.begin());
  plane->SetNormal(n.begin());

  vtkClipPolyDataPtr clipper = vtkClipPolyDataPtr::New();
  clipper->SetInput(input);
  clipper->SetClipFunction(plane);
  clipper->SetInsideOut(true);
  clipper->Update();
  return clipper->GetOutput();
}

vtkPolyDataPtr ProbeSector::generateClipper(vtkPolyDataPtr input)
{
//  return input;

//  vtkBoxPtr box = vtkBoxPtr::New();
//  DoubleBoundingBox3D bb_p = mData.mImage.mClipRect_p;
//  std::cout << "box_p: " << bb_p << std::endl;
//  std::cout << "box_p_p0: " << bb_p.corner(0,0,0) << std::endl;
//  std::cout << "box_p_p1: " << bb_p.corner(1,1,1) << std::endl;
  DoubleBoundingBox3D bb = mData.mImage.getClipRect_u();
  bb[4] = -1;
  bb[5] = +1;
//  std::cout << "box_u: " << bb << std::endl;
//  std::cout << "box_u_p0: " << bb.corner(0,0,0) << std::endl;
//  std::cout << "box_u_p1: " << bb.corner(1,1,1) << std::endl;
//  box->SetBounds(bb.begin());

  vtkPlanesPtr planes = vtkPlanesPtr::New();
  planes->SetBounds(bb.begin());

  vtkPolyDataPtr retval = input;
  retval = clipPlane(retval, bb.corner(0,0,0), Vector3D(-1, 0,0));
  retval = clipPlane(retval, bb.corner(1,1,0), Vector3D( 1, 0,0));
  retval = clipPlane(retval, bb.corner(0,0,0), Vector3D( 0,-1,0));
  retval = clipPlane(retval, bb.corner(1,1,0), Vector3D( 0, 1,0));
  return retval;

//  vtkPlanePtr plane = vtkPlanePtr::New();
//  plane->SetOrigin(bb.corner(0,0,0).begin());
//  plane->SetNormal(ssc::Vector3D(-1,0,0).begin());

//  vtkPolyLinePtr clipRect = this->createClipRectPolyLine();
//  vtkPolyPlanePtr clipPlanes = vtkPolyPlanePtr::New();
//  clipPlanes->SetPolyLine(clipRect);
//  clipPlanes->Update();

//  vtkClipPolyDataPtr clipper = vtkClipPolyDataPtr::New();
//  clipper->SetInput(input);
////  clipper->SetClipFunction(box);
//  clipper->SetClipFunction(planes);
//  clipper->SetInsideOut(true);
//  clipper->Update();
//  return clipper->GetOutput();

  // vtkPolyData in space u from box or corners
  // vtkPolyPlane as extrusion of polydata
  // vtkClipPolyData with sector and polyplane
}

/**generate a polydata containing only a polygon representing the sector cliprect.
 *
 */
vtkPolyDataPtr ProbeSector::getClipRectPolyData()
{
  vtkPointsPtr points = vtkPointsPtr::New();
  vtkCellArrayPtr sides = vtkCellArrayPtr::New();

  //points->Allocate(N+M);
  vtkIdType cells[5] = { 0,1,2,3,0};
  sides->InsertNextCell(5, cells);

  DoubleBoundingBox3D bb = mData.mImage.getClipRect_u();
  points->InsertNextPoint(bb.corner(0,0,0).begin());
  points->InsertNextPoint(bb.corner(1,0,0).begin());
  points->InsertNextPoint(bb.corner(1,1,0).begin());
  points->InsertNextPoint(bb.corner(0,1,0).begin());

  vtkPolyDataPtr polydata = vtkPolyDataPtr::New();
  polydata->SetPoints(points);
  polydata->SetLines(sides);

//  polydata = this->generateClipper(polydata);
  return polydata;
}

void ProbeSector::updateSector()
{
  if (mData.mType == ProbeData::tNONE)
    return;

  ssc::Vector3D bounds = ssc::Vector3D(mData.mImage.mSize.width()-1, mData.mImage.mSize.height()-1, 1);
  bounds = multiply_elems(bounds, mData.mImage.mSpacing);

  vtkFloatArrayPtr newTCoords = vtkFloatArrayPtr::New();
  newTCoords->SetNumberOfComponents(2);

  ssc::Vector3D p(0,0,0); // tool position in local space
  // first define the shape of the probe in a xy-plane.
  // after that, transform into yz-plane because thats the tool plane (probe point towards positive z)
  // then transform to global space.
  Transform3D tMl = createTransformIJC(Vector3D(0,1,0), Vector3D(0,0,1), Vector3D(0,0,0));
  Transform3D texMu = createTransformNormalize(DoubleBoundingBox3D(0, bounds[0], 0, bounds[1], 0, 1), DoubleBoundingBox3D(0,1,0,1,0,1));
  Transform3D uMt = this->get_tMu().inv();
  Transform3D texMl = texMu * uMt * tMl;
  Transform3D uMl = uMt * tMl;

  //Transform3D M = tMl;
  Vector3D e_x = unitVector(0);
  Vector3D e_y = unitVector(M_PI_2);
  Vector3D e_z(0,0,1);

  vtkPointsPtr points = vtkPointsPtr::New();
  vtkCellArrayPtr sides = vtkCellArrayPtr::New();
  vtkCellArrayPtr strips = vtkCellArrayPtr::New();
  vtkCellArrayPtr polys = vtkCellArrayPtr::New();

  DoubleBoundingBox3D bb_u;

  if (mData.mType == ProbeData::tLINEAR)
  {
    Vector3D cr = mData.mDepthStart * e_y + mData.mWidth/2 * e_x;
    Vector3D cl = mData.mDepthStart * e_y - mData.mWidth/2 * e_x;
    Vector3D pr = mData.mDepthEnd * e_y + mData.mWidth/2 * e_x;
    Vector3D pl = mData.mDepthEnd * e_y - mData.mWidth/2 * e_x;

    points->Allocate(4);
    points->InsertNextPoint(uMl.coord(cl).begin());
    points->InsertNextPoint(uMl.coord(cr).begin());
    points->InsertNextPoint(uMl.coord(pr).begin());
    points->InsertNextPoint(uMl.coord(pl).begin());

    newTCoords->Allocate(4);
    newTCoords->InsertNextTuple(texMl.coord(cl).begin());
    newTCoords->InsertNextTuple(texMl.coord(cr).begin());
    newTCoords->InsertNextTuple(texMl.coord(pr).begin());
    newTCoords->InsertNextTuple(texMl.coord(pl).begin());

    vtkIdType cells[5] = { 0,1,2,3,0};
    sides->InsertNextCell(5, cells);
    polys->InsertNextCell(5, cells);
    vtkIdType s_cells[5] = { 0,3,1,2};
    strips->InsertNextCell(4, s_cells);
  }
  else if (mData.mType == ProbeData::tSECTOR)
  {
    Vector3D c(0,0,0);  // arc center point
//    Vector3D c = - mData.mDepthStart * e_y;  // arc center point

//    std::cout << "c_local " << c << std::endl;
//    std::cout << "c_u " << uMl.coord(c) << std::endl;
//    Vector3D c_e = c + mData.mDepthStart * unitVector(M_PI_2);
//    std::cout << "c_e_local " << c_e << std::endl;
//    std::cout << "c_e_u " << uMl.coord(c_e) << std::endl;

//    int arcRes = 20;//Number of points in arc
    int arcRes = 20;//Number of points in arc
    double angleIncrement = mData.mWidth/arcRes;
    double startAngle = M_PI_2 - mData.mWidth/2.0;
    double stopAngle = M_PI_2 + mData.mWidth/2.0;
    int N = 2*(arcRes+1); // total number of points

    points->Allocate(N);
    newTCoords->Allocate(2*N);

    for(int i = 0; i <= arcRes; i++)
    {
      double theta = startAngle + i*angleIncrement;
      Vector3D startTheta = c + mData.mDepthStart * unitVector(theta);
      newTCoords->InsertNextTuple(texMl.coord(startTheta).begin());
      points->InsertNextPoint(uMl.coord(startTheta).begin());
      //std::cout << "p_arc " << uMl.coord(startTheta) << std::endl;
    }
    for(int i = 0; i <= arcRes; i++)
    {
      double theta = stopAngle - i*angleIncrement;
      Vector3D endTheta = c + mData.mDepthEnd * unitVector(theta);
      newTCoords->InsertNextTuple(texMl.coord(endTheta).begin());
      points->InsertNextPoint(uMl.coord(endTheta).begin());
      //std::cout << "p_arc " << uMl.coord(endTheta) << std::endl;
    }

    sides->InsertNextCell(N + 1);
    for (int i = 0; i < N; i++)
      sides->InsertCellPoint(i);
    sides->InsertCellPoint(0);

    polys->InsertNextCell(N + 1);
    for (int i = 0; i < arcRes * 2 + 2; i++)
      polys->InsertCellPoint(i);
    polys->InsertCellPoint(0);

    strips->InsertNextCell(N);
    for (int i = 0; i <= arcRes; ++i)
    {
      strips->InsertCellPoint(i);
      strips->InsertCellPoint(N - 1 - i);
    }
  }

  vtkPolyDataPtr polydata = vtkPolyDataPtr::New();
  polydata->SetPoints(points);
  polydata->SetStrips(strips);
  polydata->GetPointData()->SetTCoords(newTCoords);
  polydata->SetLines(sides);
//  polydata->SetPolys(polys);
  mPolyData = polydata;
//  mPolyData = this->generateClipper(polydata);
}


}
