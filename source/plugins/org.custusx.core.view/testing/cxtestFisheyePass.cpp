/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkWindowToImageFilter.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>

#include "cxFisheyePass.h"
#include "cxMaterialOverridePass.h"
#include <vtkRenderState.h>
#include <vtkProp.h>

namespace cxtest
{

namespace
{
/** Render a small white sphere straight ahead of a wide angle camera, return the number of lit pixels. */
int renderSphereAndCountLitPixels(bool useFisheye)
{
	vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
	sphere->SetRadius(3);
	sphere->SetThetaResolution(32);
	sphere->SetPhiResolution(32);
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(sphere->GetOutputPort());
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->GetProperty()->SetColor(1, 1, 1);
	actor->GetProperty()->SetAmbient(1);
	actor->GetProperty()->SetDiffuse(0);

	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->SetBackground(0, 0, 0);
	renderer->AddActor(actor);
	renderer->GetActiveCamera()->SetPosition(0, 0, 20);
	renderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
	renderer->GetActiveCamera()->SetViewAngle(110);
	renderer->ResetCameraClippingRange();
	if (useFisheye)
		renderer->SetPass(vtkSmartPointer<cx::FisheyePass>::New());

	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->SetOffScreenRendering(true);
	renderWindow->SetSize(320, 200);
	renderWindow->AddRenderer(renderer);
	renderWindow->Render();

	vtkSmartPointer<vtkWindowToImageFilter> grabber = vtkSmartPointer<vtkWindowToImageFilter>::New();
	grabber->SetInput(renderWindow);
	grabber->Update();
	vtkImageData* image = grabber->GetOutput();

	int dims[3];
	image->GetDimensions(dims);
	int lit = 0;
	for (int y = 0; y < dims[1]; ++y)
		for (int x = 0; x < dims[0]; ++x)
			if (image->GetScalarComponentAsDouble(x, y, 0, 0) > 128)
				++lit;

	renderer->SetPass(nullptr);
	renderWindow->Finalize();
	return lit;
}
}

TEST_CASE("FisheyePass: Magnifies the center of the view", "[integration][org.custusx.core.view][visualization]")
{
	int litRectilinear = renderSphereAndCountLitPixels(false);
	int litFisheye = renderSphereAndCountLitPixels(true);

	REQUIRE(litRectilinear > 0);
	// With a 110 degree view angle and aspect 1.6, the center is magnified about 2.2 times, the area about 4.8 times
	CHECK(litFisheye > 3*litRectilinear);
}

TEST_CASE("MaterialOverridePass: Overrides and restores the material of lit actors", "[unit][org.custusx.core.view]")
{
	vtkSmartPointer<vtkActor> lit = vtkSmartPointer<vtkActor>::New();
	lit->GetProperty()->SetAmbient(0.2);
	lit->GetProperty()->SetSpecular(0.3);
	vtkSmartPointer<vtkActor> unlit = vtkSmartPointer<vtkActor>::New();
	unlit->GetProperty()->LightingOff();
	unlit->GetProperty()->SetAmbient(0.2);

	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkProp* props[2] = { lit, unlit };
	vtkRenderState state(renderer);
	state.SetPropArrayAndCount(props, 2);

	vtkSmartPointer<cx::MaterialOverridePass> pass = vtkSmartPointer<cx::MaterialOverridePass>::New();
	pass->setMaterial(0.02, 0.85, 0.6, 60);
	pass->Render(&state);

	CHECK(lit->GetProperty()->GetAmbient() == Approx(0.02));
	CHECK(lit->GetProperty()->GetSpecularPower() == Approx(60));
	CHECK(unlit->GetProperty()->GetAmbient() == Approx(0.2));

	// The owner resets the material: override again, and restore to the new original
	lit->GetProperty()->SetAmbient(0.4);
	pass->Render(&state);
	CHECK(lit->GetProperty()->GetAmbient() == Approx(0.02));

	pass->restoreMaterials();
	CHECK(lit->GetProperty()->GetAmbient() == Approx(0.4));
	CHECK(lit->GetProperty()->GetSpecular() == Approx(0.3));
}

}//namespace cxtest
