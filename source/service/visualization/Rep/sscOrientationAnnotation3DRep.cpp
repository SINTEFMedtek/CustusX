// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

/*
 * sscOrientationAnnotation3DRep.cpp
 *
 *  \date Mar 24, 2011
 *      \author christiana
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

////---------------------------------------------------------
//std::pair<QString, vtkPropPtr> OrientationAnnotation3DRep::mMarkerCache;
////---------------------------------------------------------

OrientationAnnotation3DRep::OrientationAnnotation3DRep(const QString& uid, const QString& name) :
				RepImpl(uid, name), mSize(0.2), mColor(1, 0.5, 0.5)
{
	this->rebuild(NULL);
}

OrientationAnnotation3DRepPtr OrientationAnnotation3DRep::New(const QString& uid, const QString& name)
{
	OrientationAnnotation3DRepPtr retval(new OrientationAnnotation3DRep(uid, name));
	retval->mSelf = retval;
	return retval;
}

OrientationAnnotation3DRep::~OrientationAnnotation3DRep()
{

}

void OrientationAnnotation3DRep::addRepActorsToViewRenderer(ssc::View* view)
{
	this->rebuild(view->getRenderWindow()->GetInteractor());
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

void OrientationAnnotation3DRep::setSize(double size)
{
	mSize = size;
	this->rebuild(mMarker->GetInteractor());
}

void OrientationAnnotation3DRep::rebuild(vtkRenderWindowInteractorPtr interactor)
{
	if (mMarker)
	{
		mMarker->SetInteractor(NULL);
	}

	mMarker = vtkOrientationMarkerWidgetPtr::New();
	mMarker->SetOutlineColor(mColor[0], mColor[1], mColor[2]);
	mMarker->SetViewport(0.0, 1.0 - mSize, mSize, 1.0);
	mMarker->SetOrientationMarker(mMarkerCache.second);

	if (interactor)
	{
		mMarker->SetInteractor(interactor);
		mMarker->SetEnabled(1);
		mMarker->InteractiveOff();
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
	if (true)
	{
		smoother->SetInput(person);
		smoother->SetPassBand(1);
		std::cout << "passband " << smoother->GetPassBand() << std::endl;
		smoother->Update();
		person = smoother->GetOutput();
		std::cout << "smoo cells=" << person->GetNumberOfCells() << ", mem=" << person->GetActualMemorySize()
						<< std::endl;
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

	cube->SetXPlusFaceText("L");
	cube->SetXMinusFaceText("R");
	cube->SetYPlusFaceText("P");
	cube->SetYMinusFaceText("A");
	cube->SetZPlusFaceText("S");
	cube->SetZMinusFaceText("I");
	cube->SetZFaceTextRotation(-90);
	cube->SetFaceTextScale(0.65);

	vtkPropertyPtr property;

	ssc::Vector3D red(1, 0, 0);
	ssc::Vector3D green(0, 1, 0);
	ssc::Vector3D blue(0, 0, 1);

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
