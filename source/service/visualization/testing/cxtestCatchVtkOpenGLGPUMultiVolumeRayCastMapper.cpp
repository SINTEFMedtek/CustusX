
#include "cxConfig.h"
#ifdef CX_BUILD_MEHDI_VTKMULTIVOLUME

#include "catch.hpp"
#include <vtkTransform.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRendererCollection.h>
#include <vtkImageData.h>
#include "vtkOpenGLGPUMultiVolumeRayCastMapper.h"
#include "cxVolumeProperty.h"
#include "sscImage.h"
#include "cxtestUtilities.h"
#include "cxMehdiGPURayCastMultiVolumeRep.h"

namespace cxtest
{

// Test that tests vtkOpenGLGPUMultiVolumeRayCastMapper directly, without using MehdiGPURayCastMultiVolumeRep
TEST_CASE("vtkOpenGLGPUMultiVolumeRayCastMapper can render 3 small volumes", "[integration][gui][not_apple]")
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

} // namespace cxtest


#endif //CX_BUILD_MEHDI_VTKMULTIVOLUME
