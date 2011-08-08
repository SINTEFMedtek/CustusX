/*
 * sscOrientationAnnotation3DRep.cpp
 *
 *  Created on: Mar 24, 2011
 *      Author: christiana
 */

#include <sscOrientationAnnotation3DRep.h>

#include <vtkOrientationMarkerWidget.h>
#include <vtkAnnotatedCubeActor.h>
#include <vtkProperty.h>
#include <vtkAxesActor.h>
#include <vtkTextProperty.h>
#include <vtkCaptionActor2D.h>
#include <vtkPropAssembly.h>
#include <vtkRenderWindow.h>

#include "vtkSTLReader.h"
#include "vtkSTLWriter.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include <vtkImageShrink3D.h>
#include <vtkMarchingCubes.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkDecimatePro.h>
#include <vtkPolyDataNormals.h>
#include <vtkQuadricDecimation.h>

#include "sscVector3D.h"
#include "sscView.h"
#include "sscTypeConversions.h"

typedef vtkSmartPointer<vtkAxesActor> vtkAxesActorPtr;
typedef vtkSmartPointer<vtkTextProperty> vtkTextPropertyPtr;
typedef vtkSmartPointer<vtkPropAssembly> vtkPropAssemblyPtr;
typedef vtkSmartPointer<vtkSTLWriter> vtkSTLWriterPtr;
typedef vtkSmartPointer<vtkQuadricDecimation> vtkQuadricDecimationPtr;


namespace ssc
{

std::pair<QString, vtkActorPtr> OrientationAnnotation3DRep::mMarkerCache;



OrientationAnnotation3DRep::OrientationAnnotation3DRep( const QString& uid, const QString& name) :
RepImpl(uid, name)
{
  this->createAnnotation();
}

OrientationAnnotation3DRepPtr OrientationAnnotation3DRep::New(const QString& uid,const QString& name)
{
  OrientationAnnotation3DRepPtr retval(new OrientationAnnotation3DRep(uid,name));
  retval->mSelf = retval;
  return retval;
}

OrientationAnnotation3DRep::~OrientationAnnotation3DRep()
{

}

void OrientationAnnotation3DRep::addRepActorsToViewRenderer(ssc::View* view)
{
  mMarker->SetInteractor(view->getRenderWindow()->GetInteractor());
  mMarker->SetEnabled(1);
  mMarker->InteractiveOff();
}

void OrientationAnnotation3DRep::removeRepActorsFromViewRenderer(ssc::View* view)
{
  mMarker->SetInteractor(NULL);
}

bool OrientationAnnotation3DRep::getVisible() const
{
  return mMarker->GetEnabled();
}

void OrientationAnnotation3DRep::setVisible(bool on)
{
  mMarker->SetEnabled(on);
}


void OrientationAnnotation3DRep::createAnnotation()
{
//  vtkAxesActorPtr axes = this->createAxes();
//  mCube = this-> createCube();

  // Combine the two actors into one with vtkPropAssembly ...
//  vtkPropAssemblyPtr assembly = vtkPropAssemblyPtr::New();
//  assembly->AddPart(axes);
//  assembly->AddPart(cube);

  mMarker = vtkOrientationMarkerWidgetPtr::New();

//  marker->SetOutlineColor( 0.93, 0.57, 0.13);
  mMarker->SetOutlineColor( 1, 0.5, 0.5);
//  marker->SetOrientationMarker(assembly);
  double size = 0.25;
//  size = 1;
  mMarker->SetViewport( 0.0, 1.0-size, size, 1.0);

  this->setMarkerFilename(mMarkerCache.first);
//  marker->SetInteractor(mView->getRenderWindow()->GetInteractor());
//  marker->SetEnabled(1);
//  marker->InteractiveOff();
//  std::cout << "marker: " << mMarker->GetOrientationMarker() << std::endl;
}

void OrientationAnnotation3DRep::setSize(double size)
{
  mMarker->SetViewport( 0.0, 1.0-size, size, 1.0);
}

void OrientationAnnotation3DRep::setMarkerFilename(const QString filename)
{
//  std::cout << "beg OrientationAnnotation3DRep::setMarkerFilename " << mMarkerCache.first << " " << filename  << " " << mMarker->GetOrientationMarker() << std::endl;

  if (!mMarkerCache.second || (mMarkerCache.first != filename))
  {
    mMarkerCache.first = filename;;

    if (filename.isEmpty())
    {
      mMarkerCache.second = 0;
    }
    else
    {
      mMarkerCache.second = this->readMarkerFromFile(filename);
//      std::cout << "  OrientationAnnotation3DRep::readMarkerFromFile " << filename << std::endl;
    }
  }

  if (mMarker)
  {
    if (mMarkerCache.second)
      mMarker->SetOrientationMarker(mMarkerCache.second);
    else
      mMarker->SetOrientationMarker(this->createCube());
  }

//  std::cout << "end OrientationAnnotation3DRep::setMarkerFilename " << filename  << " " << mMarker->GetOrientationMarker() << std::endl;
}

vtkActorPtr OrientationAnnotation3DRep::readMarkerFromFile(const QString filename)
{
  vtkSTLReaderPtr STLReader = vtkSTLReaderPtr::New();
  STLReader->SetFileName(cstring_cast(filename));
//  STLReader->SetFileName("/home/christiana/Dropbox/woman.stl");
//    STLReader->SetFileName("/home/christiana/Dropbox/man.stl");

  vtkPolyDataPtr person = STLReader->GetOutput();

  vtkPolyDataNormalsPtr normals = vtkPolyDataNormalsPtr::New();
  normals->SetInput(person);
  normals->Update();
  person = normals->GetOutput();

  vtkPolyDataMapperPtr polyDataMapper = vtkPolyDataMapperPtr::New();
  polyDataMapper->SetInput(person); //read a 3D model file of the tool
  polyDataMapper->Update();

  vtkActorPtr actor = vtkActorPtr::New();
  actor->SetMapper(polyDataMapper);
  actor->GetProperty()->SetColor(0.5, 1, 1);
  actor->GetProperty()->SetSpecularPower(15);
  actor->GetProperty()->SetSpecular(0.3);

  return actor;
}

void OrientationAnnotation3DRep::reduceSTLFile(const QString source, const QString dest, double reduction)
{
  vtkSTLReaderPtr STLReader = vtkSTLReaderPtr::New();
  STLReader->SetFileName(cstring_cast(source));

  vtkPolyDataPtr person = STLReader->GetOutput();
  person->Update();
  std::cout << "base cells=" << person->GetNumberOfCells() << ", mem=" << person->GetActualMemorySize() << std::endl;

  // Smooth surface model
  vtkWindowedSincPolyDataFilterPtr smoother = vtkWindowedSincPolyDataFilterPtr::New();
  if(true)
  {
    smoother->SetInput(person);
    smoother->SetPassBand(1);
    std::cout << "passband " << smoother->GetPassBand() << std::endl;
    smoother->Update();
    person = smoother->GetOutput();
    std::cout << "smoo cells=" << person->GetNumberOfCells() << ", mem=" << person->GetActualMemorySize() << std::endl;
  }

  //Decimate surface model (remove a percentage of the polygons)
  vtkTriangleFilterPtr trifilt = vtkTriangleFilterPtr::New();
  trifilt->SetInput(person);
  trifilt->Update();
  person = trifilt->GetOutput();
  std::cout << "trif cells=" << person->GetNumberOfCells() << ", mem=" << person->GetActualMemorySize() << std::endl;

  //    vtkDecimateProPtr deci = vtkDecimateProPtr::New();
  vtkQuadricDecimationPtr deci = vtkQuadricDecimationPtr::New();

  deci->SetInput(person);
  deci->SetTargetReduction(reduction);
  deci->Update();
  person = deci->GetOutput();
  std::cout << "deci cells=" << person->GetNumberOfCells() << ", mem=" << person->GetActualMemorySize() << std::endl;

  vtkSTLWriterPtr writer = vtkSTLWriterPtr::New();
  writer->SetInput(person);
  writer->SetFileName(cstring_cast(dest));
  writer->SetFileTypeToASCII();
  //    writer->SetFileTypeToBinary();
  writer->Update();
  writer->Write();
}


vtkAnnotatedCubeActorPtr OrientationAnnotation3DRep::createCube()
{
  vtkAnnotatedCubeActorPtr cube = vtkAnnotatedCubeActorPtr::New();
//  mCube = cube;

  cube->SetXPlusFaceText("L");
  cube->SetXMinusFaceText("R");
  cube->SetYPlusFaceText("P");
  cube->SetYMinusFaceText("A");
  cube->SetZPlusFaceText("S");
  cube->SetZMinusFaceText("I");
  cube->SetZFaceTextRotation(-90);
  cube->SetFaceTextScale(0.65);

  vtkPropertyPtr property;

  ssc::Vector3D red(1,0,0);
  ssc::Vector3D green(0,1,0);
  ssc::Vector3D blue(0,0,1);

  property = cube->GetCubeProperty();
  property->SetColor( 0.5, 1, 1);
  property = cube->GetTextEdgesProperty();
  property->SetLineWidth(1);
  property->SetDiffuse(0);
  property->SetAmbient(1);
  property->SetColor(0.18, 0.28, 0.23);

  property = cube->GetXPlusFaceProperty();
  property->SetColor(red.begin());
  property->SetInterpolationToFlat();
  property = cube->GetXMinusFaceProperty();
  property->SetColor(red.begin());
  property->SetInterpolationToFlat();

  property = cube->GetYPlusFaceProperty();
  property->SetColor(green.begin());
  property->SetInterpolationToFlat();
  property = cube->GetYMinusFaceProperty();
  property->SetColor(green.begin());
  property->SetInterpolationToFlat();

  property = cube->GetZPlusFaceProperty();
  property->SetColor(blue.begin());
  property->SetInterpolationToFlat();
  property = cube->GetZMinusFaceProperty();
  property->SetColor(blue.begin());
  property->SetInterpolationToFlat();

  return cube;
}

vtkAxesActorPtr OrientationAnnotation3DRep::createAxes()
{
  vtkAxesActorPtr axes = vtkAxesActorPtr::New();
  axes->SetShaftTypeToCylinder();
  axes->SetXAxisLabelText("x");
  axes->SetYAxisLabelText("y");
  axes->SetZAxisLabelText("z");
  axes->SetTotalLength(1.5, 1.5, 1.5);
  vtkTextPropertyPtr tprop = vtkTextPropertyPtr::New();
  tprop->ItalicOn();
  tprop->ShadowOn();
  tprop->SetFontFamilyToTimes();
  axes->GetXAxisCaptionActor2D()->SetCaptionTextProperty(tprop);
  vtkTextPropertyPtr tprop2 = vtkTextPropertyPtr::New();
  tprop2->ShallowCopy(tprop);
  axes->GetYAxisCaptionActor2D()->SetCaptionTextProperty(tprop2);
  vtkTextPropertyPtr tprop3 = vtkTextPropertyPtr::New();
  tprop3->ShallowCopy(tprop);
  axes->GetZAxisCaptionActor2D()->SetCaptionTextProperty(tprop3);
  return axes;
}

}
