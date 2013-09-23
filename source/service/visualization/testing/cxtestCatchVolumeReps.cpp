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

#include "cxConfig.h"
#ifdef CX_BUILD_MEHDI_VTKMULTIVOLUME

#include "catch.hpp"
#include "cxMehdiGPURayCastMultiVolumeRep.h"
#include "sscVolumetricRep.h"

#include <vtkImageData.h>
#include "sscImage.h"
#include "cxtestRenderTester.h"
#include "cxtestUtilities.h"

//Needed for vtkOpenGLGPUMultiVolumeRayCastMapper test
#include <vtkTransform.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRendererCollection.h>
#include "vtkOpenGLGPUMultiVolumeRayCastMapper.h"
#include "cxVolumeProperty.h"

typedef vtkSmartPointer<class vtkWindowToImageFilter> vtkWindowToImageFilterPtr;

namespace cxtest
{

void simpleVolumeRepTest(cx::RepPtr rep, unsigned int viewAxisSize = 30)
{
	RenderTesterPtr renderTester = cxtest::RenderTester::create(rep, viewAxisSize);

	vtkImageDataPtr output = renderTester->renderToImage();
//	REQUIRE(cx::similar(Eigen::Array3i(output->GetDimensions()), Eigen::Array3i(viewAxisSize,viewAxisSize,1)));
	REQUIRE(output->GetDimensions()[0] == viewAxisSize);
	REQUIRE(output->GetDimensions()[1] == viewAxisSize);

	unsigned int numNonZeroPixels = renderTester->getNumberOfNonZeroPixels(output);
	REQUIRE(numNonZeroPixels > 0);
	REQUIRE(numNonZeroPixels < viewAxisSize*viewAxisSize);
}

// Test that tests vtkOpenGLGPUMultiVolumeRayCastMapper directly, without using MehdiGPURayCastMultiVolumeRep
TEST_CASE("vtkOpenGLGPUMultiVolumeRayCastMapper can render 3 small volumes", "[integration][gui][notmac]")
{
	unsigned int imageCount = 3;
	std::vector<cx::ImagePtr> images = cxtest::Utilities::create3DImages(imageCount, Eigen::Array3i(3,3,3), 200);


	vtkOpenGLGPUMultiVolumeRayCastMapperPtr mapper;
	mapper = vtkOpenGLGPUMultiVolumeRayCastMapperPtr::New();
	mapper->setNumberOfAdditionalVolumes(images.size()-1);

	vtkVolumePtr volume;
	volume = vtkVolumePtr::New();
	volume->SetMapper(mapper);

	//Set transforms
	cx::Transform3D rMd0 = images[0]->get_rMd(); // use on first volume
	for (unsigned i=0; i<images.size(); ++i)
	{
		cx::Transform3D rMdi = images[i]->get_rMd();
		cx::Transform3D d0Mdi = rMd0.inv() * rMdi; // use on additional volumescd
		if (i==0)
		{
			volume->SetUserMatrix(rMd0.getVtkMatrix());
		}
		else if (i>0)
		{
			vtkTransformPtr transform = vtkTransform::New();
			transform->SetMatrix(d0Mdi.getVtkMatrix());
			transform->Update();
			mapper->SetAdditionalInputUserTransform(i-1,transform);
		}
	}

	//Set volume properties
	std::vector<cx::VolumePropertyPtr> volumeProperties;
	for (unsigned i=0; i<images.size(); ++i)
	{
		cx::VolumePropertyPtr property = cx::VolumeProperty::create();
		property->setImage(images[i]);
		volumeProperties.push_back(property);
		mapper->SetInput(i, images[i]->getBaseVtkImageData());

		if (i==0)
			volume->SetProperty( property->getVolumeProperty() );
		else if (i>0)
			mapper->SetAdditionalProperty(i-1, property->getVolumeProperty() );
	}


	vtkRenderWindowPtr renderWindow = vtkRenderWindowPtr::New();
	vtkRendererPtr renderer = vtkRendererPtr::New();
	renderer->AddVolume(volume);
	renderWindow->AddRenderer(renderer);
	renderWindow->Render();
	renderWindow->Render();
	REQUIRE(true);
}


TEST_CASE("MehdiGPURayCastMultiVolumeRep can render 3 small volumes.", "[rep][integration][gui][notmac]")
{
	unsigned int imageCount = 3;
	std::vector<cx::ImagePtr> images = cxtest::Utilities::create3DImages(imageCount, Eigen::Array3i(3,3,3), 200);

	cx::MehdiGPURayCastMultiVolumeRepPtr rep = cx::MehdiGPURayCastMultiVolumeRep::New("");
	REQUIRE(rep);
	rep->setImages(images);

	simpleVolumeRepTest(rep);
}

TEST_CASE("MehdiGPURayCastMultiVolumeRep can render 1 small volume.", "[rep][integration][gui][notmac]")
{
	unsigned int imageCount = 1;
	std::vector<cx::ImagePtr> images = cxtest::Utilities::create3DImages(imageCount, Eigen::Array3i(3,3,3), 200);

	cx::MehdiGPURayCastMultiVolumeRepPtr rep = cx::MehdiGPURayCastMultiVolumeRep::New("");
	REQUIRE(rep);
	rep->setImages(images);

	simpleVolumeRepTest(rep);
}

TEST_CASE("MehdiGPURayCastMultiVolumeRep can render 3 large volumes.", "[rep][integration][gui][notmac]")
{
	unsigned int imageCount = 3;
	std::vector<cx::ImagePtr> images = cxtest::Utilities::create3DImages(imageCount, Eigen::Array3i(300,300,300), 200);

	cx::MehdiGPURayCastMultiVolumeRepPtr rep = cx::MehdiGPURayCastMultiVolumeRep::New("");
	REQUIRE(rep);
	rep->setImages(images);

	simpleVolumeRepTest(rep, 1000);
}

TEST_CASE("VolumetricRep using vtkVolumeTextureMapper3D can render 1 small volume.", "[rep][unit][gui]")
{
	cx::ImagePtr image = cxtest::Utilities::create3DImage(Eigen::Array3i(3,3,3), 200);

	cx::VolumetricRepPtr rep = cx::VolumetricRep::New("");
	REQUIRE(rep);
	rep->setImage(image);

	simpleVolumeRepTest(rep);
}

TEST_CASE("VolumetricRep using vtkVolumeTextureMapper3D can render 1 large volume.", "[rep][integration][gui]")
{
	cx::ImagePtr image = cxtest::Utilities::create3DImage(Eigen::Array3i(300,300,300), 200);

	cx::VolumetricRepPtr rep = cx::VolumetricRep::New("");
	REQUIRE(rep);
	rep->setImage(image);

	simpleVolumeRepTest(rep, 1000);
}

TEST_CASE("VolumetricRep using vtkGPUVolumeRayCastMapper can render 1 small volume.", "[rep][integration][gui][notmac]")
{
	cx::ImagePtr image = cxtest::Utilities::create3DImage(Eigen::Array3i(3,3,3), 200);

	cx::VolumetricRepPtr rep = cx::VolumetricRep::New("");
	REQUIRE(rep);
	rep->setUseGPUVolumeRayCastMapper();
	rep->setImage(image);

	simpleVolumeRepTest(rep);
}

} // namespace cxtest


#endif //CX_BUILD_MEHDI_VTKMULTIVOLUME
