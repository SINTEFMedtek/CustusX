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
#include <vtkPlane.h>
#include "sscImage.h"
#include "cxtestRenderTester.h"
#include "cxtestUtilities.h"
#include "sscBoundingBox3D.h"
#include "sscMessageManager.h"
#include "cxImageEnveloper.h"

typedef vtkSmartPointer<class vtkWindowToImageFilter> vtkWindowToImageFilterPtr;

namespace cxtest
{

namespace
{
	double repRenderAndGetNonZeroFraction(cx::RepPtr rep, unsigned int viewAxisSize = 30)
	{
		RenderTesterPtr renderTester = cxtest::RenderTester::create(rep, viewAxisSize);

		vtkImageDataPtr output = renderTester->renderToImage();
	//	REQUIRE(cx::similar(Eigen::Array3i(output->GetDimensions()), Eigen::Array3i(viewAxisSize,viewAxisSize,1)));
		REQUIRE(output->GetDimensions()[0] == viewAxisSize);
		REQUIRE(output->GetDimensions()[1] == viewAxisSize);

		double retval = cxtest::Utilities::getFractionOfVoxelsAboveThreshold(output, 0);
		return retval;
	}

	void requireRepRender(cx::RepPtr rep, unsigned int viewAxisSize = 30)
	{
		double fraction = repRenderAndGetNonZeroFraction(rep, viewAxisSize);
		REQUIRE(fraction > 0);
		REQUIRE(fraction < 1);
	}
}// namespace

TEST_CASE("MehdiGPURayCastMultiVolumeRep can render 3 small volumes.", "[rep][unit][gui][not_apple][hide]")
{
	unsigned int imageCount = 3;
	std::vector<cx::ImagePtr> images = cxtest::Utilities::create3DImages(imageCount, Eigen::Array3i(3,3,3), 200);

	cx::MehdiGPURayCastMultiVolumeRepPtr rep = cx::MehdiGPURayCastMultiVolumeRep::New("");
	REQUIRE(rep);
	rep->setImages(images);

	requireRepRender(rep);
}

TEST_CASE("MehdiGPURayCastMultiVolumeRep can render 1 small volume.", "[rep][unit][gui][not_apple][hide]")
{
	unsigned int imageCount = 1;
	std::vector<cx::ImagePtr> images = cxtest::Utilities::create3DImages(imageCount, Eigen::Array3i(3,3,3), 200);

	cx::MehdiGPURayCastMultiVolumeRepPtr rep = cx::MehdiGPURayCastMultiVolumeRep::New("");
	rep->setBoundingBoxGenerator(cx::ImageEnveloper::create());
	REQUIRE(rep);
	rep->setImages(images);

	requireRepRender(rep);
}

TEST_CASE("MehdiGPURayCastMultiVolumeRep can render 3 large volumes.", "[rep][integration][gui][not_apple]")
{
	unsigned int imageCount = 3;
	std::vector<cx::ImagePtr> images = cxtest::Utilities::create3DImages(imageCount, Eigen::Array3i(300,300,300), 200);

	cx::MehdiGPURayCastMultiVolumeRepPtr rep = cx::MehdiGPURayCastMultiVolumeRep::New("");
	REQUIRE(rep);
	rep->setImages(images);

	requireRepRender(rep, 1000);
}

// vtk emits PreRender end ERROR (x502) Invalid operation when this is run after another mehdi test.
TEST_CASE("MehdiGPURayCastMultiVolumeRep can crop 1 small volume.", "[rep][unit][gui][not_apple][hide]")
{
	unsigned int size = 10;
	cx::ImagePtr image = cxtest::Utilities::create3DImage(Eigen::Array3i(size,size,size), 200);

	cx::MehdiGPURayCastMultiVolumeRepPtr rep = cx::MehdiGPURayCastMultiVolumeRep::New("");
	REQUIRE(rep);

	cx::DoubleBoundingBox3D bb = image->boundingBox();
	bb[5] = bb[4] + bb.range()[2]/2.0;

	image->setCroppingBox(bb);
	image->setCropping(true);
	rep->setImages(std::vector<cx::ImagePtr>(1, image));

	requireRepRender(rep);

	bb = cx::DoubleBoundingBox3D(0,0,0,0,0,0);
	image->setCroppingBox(bb);
	REQUIRE(repRenderAndGetNonZeroFraction(rep) == Approx(0.0));
}

// vtk emits PreRender end ERROR (x502) Invalid operation when this is run after another mehdi test.
TEST_CASE("MehdiGPURayCastMultiVolumeRep can clip 1 small volume.", "[rep][unit][gui][not_apple][hide]")
{
	unsigned int size = 10;
	cx::ImagePtr image = cxtest::Utilities::create3DImage(Eigen::Array3i(size,size,size), 200);

	cx::MehdiGPURayCastMultiVolumeRepPtr rep = cx::MehdiGPURayCastMultiVolumeRep::New("");
	REQUIRE(rep);

	vtkPlanePtr plane = vtkPlanePtr::New();
	plane->SetNormal(0,0,-1);
	plane->SetOrigin(image->boundingBox().center().data());

	image->setInteractiveClipPlane(plane);

	rep->setImages(std::vector<cx::ImagePtr>(1, image));

	requireRepRender(rep);
}

// vtk emits PreRender end ERROR (x502) Invalid operation when this is run after another mehdi test.
TEST_CASE("MehdiGPURayCastMultiVolumeRep can remove 1 small volume with clipper.", "[rep][unit][gui][not_apple][hide]")
{
	unsigned int size = 10;
	cx::ImagePtr image = cxtest::Utilities::create3DImage(Eigen::Array3i(size,size,size), 200);

	cx::MehdiGPURayCastMultiVolumeRepPtr rep = cx::MehdiGPURayCastMultiVolumeRep::New("");
	REQUIRE(rep);

	vtkPlanePtr plane = vtkPlanePtr::New();
	plane->SetNormal(0,0,-1);
	plane->SetOrigin(0,0,0);
	image->setInteractiveClipPlane(plane);
	rep->setImages(std::vector<cx::ImagePtr>(1, image));
	REQUIRE(repRenderAndGetNonZeroFraction(rep) == Approx(0.0));
}

// vtk emits PreRender end ERROR (x502) Invalid operation when this is run - in second part.
TEST_CASE("MehdiGPURayCastMultiVolumeRep can clip partially and fully 1 small volume.", "[rep][unit][gui][not_apple][hide]")
{
	cx::MessageManager::initialize();
	unsigned int size = 10;
	std::vector<cx::ImagePtr> images = cxtest::Utilities::create3DImages(2, Eigen::Array3i(size,size,size), 200);

	cx::MehdiGPURayCastMultiVolumeRepPtr rep = cx::MehdiGPURayCastMultiVolumeRep::New("");
	REQUIRE(rep);

	vtkPlanePtr plane = vtkPlanePtr::New();
	plane->SetNormal(0,0,-1);
	plane->SetOrigin(images[0]->boundingBox().center().data());

	images[0]->setInteractiveClipPlane(plane);
	images[1]->setInteractiveClipPlane(plane);

	rep->setImages(images);

	requireRepRender(rep);

	plane->SetOrigin(0,0,0);
	images[0]->setInteractiveClipPlane(plane);
	images[1]->setInteractiveClipPlane(plane);
	rep->setImages(images);
	// vtk emits PreRender end ERROR (x502) Invalid operation here
	// caused by clipping/cropping code not properly cleaned up in previous render???
	double nonZero = repRenderAndGetNonZeroFraction(rep, 500);
	REQUIRE(nonZero == Approx(0.0));
	usleep(1000000);
	cx::MessageManager::shutdown();
}

TEST_CASE("VolumetricRep using vtkVolumeTextureMapper3D can render 1 small volume.", "[rep][unit][gui]")
{
	cx::ImagePtr image = cxtest::Utilities::create3DImage(Eigen::Array3i(3,3,3), 200);

	cx::VolumetricRepPtr rep = cx::VolumetricRep::New("");
	REQUIRE(rep);
	rep->setImage(image);

	requireRepRender(rep);
}

TEST_CASE("VolumetricRep using vtkVolumeTextureMapper3D can render 1 large volume.", "[rep][integration][gui]")
{
	cx::ImagePtr image = cxtest::Utilities::create3DImage(Eigen::Array3i(300,300,300), 200);

	cx::VolumetricRepPtr rep = cx::VolumetricRep::New("");
	REQUIRE(rep);
	rep->setImage(image);

	requireRepRender(rep, 1000);
}

TEST_CASE("VolumetricRep using vtkGPUVolumeRayCastMapper can render 1 small volume.", "[rep][unit][gui][not_apple]")
{
	cx::ImagePtr image = cxtest::Utilities::create3DImage(Eigen::Array3i(3,3,3), 200);

	cx::VolumetricRepPtr rep = cx::VolumetricRep::New("");
	REQUIRE(rep);
	rep->setUseGPUVolumeRayCastMapper();
	rep->setImage(image);

	requireRepRender(rep);
}

} // namespace cxtest


#endif //CX_BUILD_MEHDI_VTKMULTIVOLUME
