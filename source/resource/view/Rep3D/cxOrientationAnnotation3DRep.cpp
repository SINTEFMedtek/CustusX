/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

/*
 * sscOrientationAnnotation3DRep.cpp
 *
 *  \date Mar 24, 2011
 *      \author christiana
 */

#include "cxOrientationAnnotation3DRep.h"

#include <vtkOrientationMarkerWidget.h>
#include <vtkAnnotatedCubeActor.h>
#include <vtkProperty.h>
#include <vtkAxesActor.h>
#include <vtkTextProperty.h>
#include <vtkCaptionActor2D.h>
#include <vtkPropAssembly.h>
#include <vtkRenderWindow.h>
#include <QFileInfo>
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

#include "cxVector3D.h"
#include "cxView.h"
#include "cxTypeConversions.h"
#include "cxForwardDeclarations.h"
#include <vtkRenderWindowInteractor.h>

typedef vtkSmartPointer<vtkAxesActor> vtkAxesActorPtr;
typedef vtkSmartPointer<vtkTextProperty> vtkTextPropertyPtr;
typedef vtkSmartPointer<vtkPropAssembly> vtkPropAssemblyPtr;
typedef vtkSmartPointer<vtkSTLWriter> vtkSTLWriterPtr;
typedef vtkSmartPointer<vtkQuadricDecimation> vtkQuadricDecimationPtr;

namespace cx
{

////---------------------------------------------------------
//std::pair<QString, vtkPropPtr> OrientationAnnotation3DRep::mMarkerCache;
////---------------------------------------------------------

OrientationAnnotation3DRep::OrientationAnnotation3DRep() :
				RepImpl(), mSize(0.2), mColor(1, 0.5, 0.5)
{
	this->rebuild(NULL);
}

OrientationAnnotation3DRepPtr OrientationAnnotation3DRep::New(const QString& uid)
{
	return wrap_new(new OrientationAnnotation3DRep(), uid);
}

OrientationAnnotation3DRep::~OrientationAnnotation3DRep()
{

}

void OrientationAnnotation3DRep::addRepActorsToViewRenderer(ViewPtr view)
{
	this->rebuild(view->getRenderWindow()->GetInteractor());
}

void OrientationAnnotation3DRep::removeRepActorsFromViewRenderer(ViewPtr view)
{
	mMarker->SetInteractor(NULL);
}

bool OrientationAnnotation3DRep::getVisible() const
{
	return mMarker->GetEnabled();
}

void OrientationAnnotation3DRep::setVisible(bool on)
{
	if (!this->getView())
		return;
	mMarker->SetEnabled(on);
}

void OrientationAnnotation3DRep::setSize(double size)
{
	mSize = size;
	this->rebuild(mMarker->GetInteractor());
}

void OrientationAnnotation3DRep::rebuild(vtkRenderWindowInteractorPtr interactor)
{
	bool enable = true;
	if (mMarker)
	{
		enable = mMarker->GetEnabled();
		mMarker->SetInteractor(NULL);
	}

	mMarker = vtkOrientationMarkerWidgetPtr::New();
	mMarker->SetOutlineColor(mColor[0], mColor[1], mColor[2]);
	mMarker->SetViewport(0.0, 1.0 - mSize, mSize, 1.0);
	mMarker->SetOrientationMarker(mMarkerCache.second);

	if (interactor)
	{
		mMarker->KeyPressActivationOff();
		mMarker->SetInteractor(interactor);
		mMarker->SetEnabled(true);
		mMarker->InteractiveOff();//This line prints a VTK warning if enabled is false
		mMarker->SetEnabled(enable);
	}
}

void OrientationAnnotation3DRep::setMarkerFilename(const QString filename)
{

	if (!mMarkerCache.second || (mMarkerCache.first != filename))
	{
		mMarkerCache.first = filename;
		;
		mMarkerCache.second = this->readMarkerFromFile(filename);
	}

	this->rebuild(mMarker->GetInteractor());
}

vtkPropPtr OrientationAnnotation3DRep::readMarkerFromFile(const QString filename)
{
	if (filename.isEmpty() || !QFileInfo(filename).exists() || QFileInfo(filename).isDir())
	{
		return this->createCube();
	}

//	std::cout << "OrientationAnnotation3DRep::readMarkerFromFile " << filename << std::endl;

	vtkSTLReaderPtr STLReader = vtkSTLReaderPtr::New();
	STLReader->SetFileName(cstring_cast(filename));

//	vtkPolyDataPtr person = STLReader->GetOutput();

	vtkPolyDataNormalsPtr normals = vtkPolyDataNormalsPtr::New();
	normals->SetInputConnection(STLReader->GetOutputPort());
	normals->Update();
//	person = normals->GetOutput();

	vtkPolyDataMapperPtr polyDataMapper = vtkPolyDataMapperPtr::New();
	polyDataMapper->SetInputConnection(normals->GetOutputPort()); //read a 3D model file of the tool
	polyDataMapper->Update();

	vtkActorPtr actor = vtkActorPtr::New();
	actor->SetMapper(polyDataMapper);
	actor->GetProperty()->SetColor(0.5, 1, 1);
	actor->GetProperty()->SetSpecularPower(15);
	actor->GetProperty()->SetSpecular(0.3);

	return actor;
}

vtkAnnotatedCubeActorPtr OrientationAnnotation3DRep::createCube()
{
	vtkAnnotatedCubeActorPtr cube = vtkAnnotatedCubeActorPtr::New();

	cube->SetXPlusFaceText("L");
	cube->SetXMinusFaceText("R");
	cube->SetYPlusFaceText("P");
	cube->SetYMinusFaceText("A");
	cube->SetZPlusFaceText("S");
	cube->SetZMinusFaceText("I");
	cube->SetZFaceTextRotation(-90);
	cube->SetFaceTextScale(0.65);

	vtkPropertyPtr property;

	Vector3D red(1, 0, 0);
	Vector3D green(0, 1, 0);
	Vector3D blue(0, 0, 1);

	property = cube->GetCubeProperty();
	property->SetColor(0.5, 1, 1);
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
