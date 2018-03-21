/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxtestUtilities.h"
#include "cxImage.h"
#include "cxVtkHelperClasses.h"
#include "cxVolumeHelpers.h"
#include "cxTypeConversions.h"

#include <vtkRenderWindowInteractor.h>
#include <vtkTextMapper.h>
#include <vtkRenderWindow.h>
#include <vtkTextProperty.h>
#include <vtkActor2D.h>
#include <vtkRendererCollection.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkCamera.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkVolumeProperty.h>
#include <vtkPiecewiseFunction.h>

#ifndef CX_VTK_OPENGL2
#include <vtkVolumeTextureMapper3D.h>
#endif

namespace cxtest
{

TEST_CASE("Render volume with texture mapper and text overlay", "[integration][resource][visualization][textrender]")
{
	vtkImageDataPtr image = cx::generateVtkImageData(Eigen::Array3i(3,3,3), Eigen::Vector3d(1, 1, 1), 100);

	vtkRenderWindowPtr renderWindow = vtkRenderWindowPtr::New();
	vtkRenderWindowInteractorPtr interactor = vtkRenderWindowInteractorPtr::New();
	interactor->SetRenderWindow(renderWindow);


	vtkRendererPtr mRenderer = vtkRendererPtr::New();
	mRenderer->SetBackground(0.1,0.4,0.2);
	mRenderer->GetActiveCamera()->SetParallelProjection(true);

	renderWindow->AddRenderer(mRenderer);
	renderWindow->SetSize(120,120);

	//Text
//	cx::TextDisplay::forceUseVtkTextMapper();
	vtkTextMapperPtr mapper = vtkTextMapperPtr::New();
	mapper->SetInput("Test");
	mapper->GetTextProperty()->SetColor(1, 0, 0);
	mapper->GetTextProperty()->SetFontSize(20);
	mapper->GetTextProperty()->SetFontFamilyToArial();

	vtkActor2DPtr textActor = vtkActor2DPtr::New();
	textActor->SetMapper(mapper);

	mRenderer->AddActor(textActor);

	//Volume
	vtkVolumeTextureMapper3DPtr volumeMapper = vtkVolumeTextureMapper3DPtr::New();
	volumeMapper->SetInputData(image);

	vtkVolumePtr volume = vtkVolumePtr::New();
	volume->SetMapper(volumeMapper);
	mRenderer->AddVolume(volume);

	mRenderer->ResetCamera();
	renderWindow->Render();

#ifndef CX_VTK_OPENGL2
	REQUIRE(volumeMapper->IsRenderSupported(volume->GetProperty(), mRenderer));
#endif

	int numRenders = 3000;
	for (int i = 0; i < numRenders; ++i)
	{
		QString text(qstring_cast("Test: ")+qstring_cast(i));
		mapper->SetInput(cstring_cast(text));
		renderWindow->Render();
	}
	REQUIRE(true);

}

} //namespace cxtest
