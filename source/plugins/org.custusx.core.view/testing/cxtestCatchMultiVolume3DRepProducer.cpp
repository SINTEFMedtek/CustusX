/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkVolumeTextureMapper3D.h>
#include <vtkVolume.h>

#include "catch.hpp"

#include "cxVolumeHelpers.h"
#include "cxVolumetricRep.h"
#include "cxtestUtilities.h"

#include "cxtestDirectSignalListener.h"
#include "cxImage2DRep3D.h"
#include "cxConfig.h"

#include "cxMultiVolume3DRepProducer.h"
#include "cxtestMultiVolume3DRepProducerFixture.h"

#include "cxMehdiGPURayCastMultiVolumeRep.h"
//#include "vtkOpenGLGPUMultiVolumeRayCastMapper.h"

namespace cxtest
{

TEST_CASE("MultiVolume3DRepProducer emits imageChanged and repChanged signals for 1 added image.", "[unit]")
{
	MultiVolume3DRepProducerFixture fixture;

	cxtest::DirectSignalListener imagesChanged(&fixture.mBase, SIGNAL(imagesChanged()));

	fixture.initializeVisualizerAndImages("vtkVolumeTextureMapper3D", 1);

	CHECK(imagesChanged.isReceived());
}

TEST_CASE("MultiVolume3DRepProducer creates 1 rep using vtkVolumeTextureMapper3D for 1 added image.", "[unit]")
{
	MultiVolume3DRepProducerFixture fixture;

	fixture.initializeVisualizerAndImages("vtkVolumeTextureMapper3D", 1);

	REQUIRE(fixture.mBase.getAllReps().size() == 1);

	cx::VolumetricRepPtr rep = fixture.downcastRep<cx::VolumetricRep>(0);
	REQUIRE(rep);
	CHECK(dynamic_cast<vtkVolumeMapper*>(rep->getVtkVolume()->GetMapper()));
}

TEST_CASE("MultiVolume3DRepProducer creates 0 reps using invalid type for 1 added image.", "[unit]")
{
	MultiVolume3DRepProducerFixture fixture;
	fixture.initializeVisualizerAndImages("invalid type", 1);

	REQUIRE(fixture.mBase.getAllReps().size() == 0);
}

TEST_CASE("MultiVolume3DRepProducer creates 1 rep using vtkGPUVolumeRayCastMapper for 1 added image.", "[unit]")
{
	MultiVolume3DRepProducerFixture fixture;
	fixture.initializeVisualizerAndImages("vtkGPUVolumeRayCastMapper", 1);

	REQUIRE(fixture.mBase.getAllReps().size() == 1);

	cx::VolumetricRepPtr rep = fixture.downcastRep<cx::VolumetricRep>(0);
	REQUIRE(rep);
	CHECK(dynamic_cast<vtkGPUVolumeRayCastMapper*>(rep->getVtkVolume()->GetMapper()));
}

TEST_CASE("MultiVolume3DRepProducer creates 1 rep using Image2DRep3D for 1 added 2D image, for any visualizer type.", "[unit]")
{
	MultiVolume3DRepProducerFixture fixture;

	fixture.mBase.setVisualizerType("vtkVolumeTextureMapper3D");
	cx::ImagePtr image = cxtest::Utilities::create3DImage(Eigen::Array3i(5,5,1));
	fixture.mBase.addImage(image);
	fixture.mImages.push_back(image);


	REQUIRE(fixture.mBase.getAllReps().size() == 1);

	cx::Image2DRep3DPtr rep = fixture.downcastRep<cx::Image2DRep3D>(0);
	REQUIRE(rep);
}

TEST_CASE("MultiVolume3DRepProducer creates 3 reps using vtkVolumeTextureMapper3D for 3 added images.", "[unit]")
{
	MultiVolume3DRepProducerFixture fixture;

	unsigned imageCount = 3;
	fixture.initializeVisualizerAndImages("vtkVolumeTextureMapper3D", imageCount);

	REQUIRE(fixture.mBase.getAllReps().size() == imageCount);

	for (unsigned i=0; i<imageCount; ++i)
	{
		cx::VolumetricRepPtr rep = fixture.downcastRep<cx::VolumetricRep>(i);
		REQUIRE(rep);
		CHECK(dynamic_cast<vtkVolumeMapper*>(rep->getVtkVolume()->GetMapper()));
	}
}

TEST_CASE("MultiVolume3DRepProducer creates 2 reps using vtkVolumeTextureMapper3D for 3 added and 1 removed image(s).", "[unit]")
{
	MultiVolume3DRepProducerFixture fixture;

	unsigned imageCount = 3;
	fixture.initializeVisualizerAndImages("vtkVolumeTextureMapper3D", imageCount);

	fixture.mBase.removeImage(fixture.mImages[0]->getUid());
	imageCount--;

	REQUIRE(fixture.mBase.getAllReps().size() == imageCount);

	for (unsigned i=0; i<imageCount; ++i)
	{
		cx::VolumetricRepPtr rep = fixture.downcastRep<cx::VolumetricRep>(i);
		REQUIRE(rep);
		CHECK(dynamic_cast<vtkVolumeMapper*>(rep->getVtkVolume()->GetMapper()));
	}
}

#ifdef CX_BUILD_MEHDI_VTKMULTIVOLUME
TEST_CASE("MultiVolume3DRepProducer creates 1 rep using vtkOpenGLGPUMultiVolumeRayCastMapper for 3 added images.", "[unit]")
{
	MultiVolume3DRepProducerFixture fixture;

	unsigned imageCount = 3;
	fixture.initializeVisualizerAndImages("vtkOpenGLGPUMultiVolumeRayCastMapper", imageCount);

	REQUIRE(fixture.mBase.getAllReps().size() == 1);

	cx::MehdiGPURayCastMultiVolumeRepPtr rep = fixture.downcastRep<cx::MehdiGPURayCastMultiVolumeRep>(0);
	REQUIRE(rep);
//		CHECK(dynamic_cast<vtkOpenGLGPUMultiVolumeRayCastMapper*>(rep->getVtkVolume()->GetMapper()));
}
#endif //CX_BUILD_MEHDI_VTKMULTIVOLUME

} // namespace cxtest
