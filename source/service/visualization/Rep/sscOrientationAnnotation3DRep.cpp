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

#include "sscVector3D.h"
#include "sscView.h"

typedef vtkSmartPointer<vtkAxesActor> vtkAxesActorPtr;
typedef vtkSmartPointer<vtkTextProperty> vtkTextPropertyPtr;
typedef vtkSmartPointer<vtkPropAssembly> vtkPropAssemblyPtr;


namespace ssc
{


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
  vtkAnnotatedCubeActorPtr cube = vtkAnnotatedCubeActorPtr::New();
  mCube = cube;

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

  // Combine the two actors into one with vtkPropAssembly ...
  vtkPropAssemblyPtr assembly = vtkPropAssemblyPtr::New();
//  assembly->AddPart(axes);
  assembly->AddPart(cube);


  vtkOrientationMarkerWidgetPtr marker = vtkOrientationMarkerWidgetPtr::New();

//  marker->SetOutlineColor( 0.93, 0.57, 0.13);
  marker->SetOutlineColor( 1, 0.5, 0.5);
  marker->SetOrientationMarker(assembly);
  double size = 0.1;
  marker->SetViewport( 0.0, 1.0-size, size, 1.0);

//  marker->SetInteractor(mView->getRenderWindow()->GetInteractor());
//  marker->SetEnabled(1);
//  marker->InteractiveOff();
  mMarker = marker;
}


}
