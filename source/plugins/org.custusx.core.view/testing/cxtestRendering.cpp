/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
//#include <vtkVolumeTextureMapper3D.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkVolumeProperty.h>
#include <vtkPiecewiseFunction.h>

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
//	REQUIRE(volumeMapper->IsRenderSupported(volume->GetProperty(), mRenderer));

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
