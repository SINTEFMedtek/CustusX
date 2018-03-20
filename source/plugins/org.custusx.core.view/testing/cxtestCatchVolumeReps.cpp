/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxConfig.h"
#ifdef CX_BUILD_MEHDI_VTKMULTIVOLUME

#include "catch.hpp"
#include "cxMehdiGPURayCastMultiVolumeRep.h"
#include "cxVolumetricRep.h"

#include <vtkImageData.h>
#include <vtkPlane.h>
#include "cxImage.h"
#include "cxtestRenderTester.h"
#include "cxtestUtilities.h"
#include "cxBoundingBox3D.h"

#include "cxImageEnveloper.h"

typedef vtkSmartPointer<class vtkWindowToImageFilter> vtkWindowToImageFilterPtr;

namespace cxtest
{

namespace {
double repRenderAndGetNonZeroFraction(cx::RepPtr rep, int viewAxisSize=30)
{
	RenderTesterPtr renderTester = cxtest::RenderTester::create(rep, viewAxisSize);

	vtkImageDataPtr output = renderTester->renderToImage();
//	REQUIRE(cx::similar(Eigen::Array3i(output->GetDimensions()), Eigen::Array3i(viewAxisSize,viewAxisSize,1)));
//	REQUIRE(output->GetDimensions()[0] == viewAxisSize);
//	REQUIRE(output->GetDimensions()[1] == viewAxisSize);

	double retval = cxtest::Utilities::getFractionOfVoxelsAboveThreshold(output, 0);
	renderTester->writeToPNG(output, "output.png");
	return retval;
}
} // namespace

class MehdiGPURayCastMultiVolumeRepFixture
{
public:

	MehdiGPURayCastMultiVolumeRepFixture() :
		mImageCount(1),
		mAxisSize(3),
		mViewAxisSize(30)
	{
		cx::Reporter::initialize();
	}
	~MehdiGPURayCastMultiVolumeRepFixture()
	{
		cx::Reporter::shutdown();
	}

	double renderAndGetNonZeroFraction()
	{
		return repRenderAndGetNonZeroFraction(mRep, mViewAxisSize);
	}

	void requireRepRender()
	{
		double fraction = renderAndGetNonZeroFraction();
		REQUIRE(fraction > 0);
		REQUIRE(fraction < 1);
	}

	void setNumberOfImages(int count)
	{
		mImageCount = count;
	}

	void setImageAxisSize(int size)
	{
		mAxisSize = size;
	}

	void setViewAxisSize(int size)
	{
		mViewAxisSize = size;
	}

	void createRep()
	{
		mImages = cxtest::Utilities::create3DImages(mImageCount, Eigen::Array3i(mAxisSize,mAxisSize,mAxisSize), 200);

		mRep = cx::MehdiGPURayCastMultiVolumeRep::New();
		mRep->setImages(mImages);
	}

public:
	std::vector<cx::ImagePtr> mImages;
	cx::MehdiGPURayCastMultiVolumeRepPtr mRep;
private:
	int mImageCount;
	int mAxisSize;
	unsigned int mViewAxisSize;
};

TEST_CASE("MehdiGPURayCastMultiVolumeRep can render 1 small volume.", "[rep][unit][visualization][unstable]")
{
	MehdiGPURayCastMultiVolumeRepFixture fixture;
	fixture.setNumberOfImages(1);
	fixture.setImageAxisSize(3);
	fixture.createRep();

	fixture.requireRepRender();
}

TEST_CASE("MehdiGPURayCastMultiVolumeRep can render 3 small volumes.", "[rep][unit][visualization][unstable]")//not stable as unit/integration
{
	MehdiGPURayCastMultiVolumeRepFixture fixture;
	fixture.setNumberOfImages(3);
	fixture.setImageAxisSize(3);
	fixture.createRep();

	fixture.requireRepRender();
}

TEST_CASE("MehdiGPURayCastMultiVolumeRep can render 3 large volumes.", "[rep][unit][visualization][unstable]")
{
	MehdiGPURayCastMultiVolumeRepFixture fixture;
	fixture.setNumberOfImages(3);
	fixture.setImageAxisSize(300);
	fixture.setViewAxisSize(1000);
	fixture.createRep();

	fixture.requireRepRender();
}

// vtk emits PreRender end ERROR (x502) Invalid operation when this is run after another mehdi test.
TEST_CASE("MehdiGPURayCastMultiVolumeRep can crop 1 small volume.", "[rep][unit][visualization][unstable]")
{
	MehdiGPURayCastMultiVolumeRepFixture fixture;
	fixture.setNumberOfImages(1);
	fixture.setImageAxisSize(10);
	fixture.createRep();

	cx::ImagePtr image = fixture.mImages[0];
	cx::DoubleBoundingBox3D bb = image->boundingBox();
	bb[5] = bb[4] + bb.range()[2]/2.0;
	image->setCroppingBox(bb);
	image->setCropping(true);

	fixture.requireRepRender();
}

// vtk emits PreRender end ERROR (x502) Invalid operation when this is run after another mehdi test.
TEST_CASE("MehdiGPURayCastMultiVolumeRep can crop completely 1 small volume.", "[rep][unit][visualization][unstable]")
{
	MehdiGPURayCastMultiVolumeRepFixture fixture;
	fixture.setNumberOfImages(1);
	fixture.setImageAxisSize(10);
	fixture.createRep();

	cx::DoubleBoundingBox3D bb = cx::DoubleBoundingBox3D(0,0,0,0,0,0);

	cx::ImagePtr image = fixture.mImages[0];
	image->setCropping(true);
	image->setCroppingBox(bb);

	REQUIRE(fixture.renderAndGetNonZeroFraction() == Approx(0.0));
}

// vtk emits PreRender end ERROR (x502) Invalid operation when this is run after another mehdi test.
TEST_CASE("MehdiGPURayCastMultiVolumeRep can clip 1 small volume.", "[rep][unit][visualization][unstable]")
{
	MehdiGPURayCastMultiVolumeRepFixture fixture;
	fixture.setNumberOfImages(1);
	fixture.setImageAxisSize(10);
	fixture.createRep();

	cx::ImagePtr image = fixture.mImages[0];
	vtkPlanePtr plane = vtkPlanePtr::New();
	plane->SetNormal(0,0,-1);
	plane->SetOrigin(image->boundingBox().center().data());
	image->setInteractiveClipPlane(plane);

	fixture.requireRepRender();
}

// vtk emits PreRender end ERROR (x502) Invalid operation when this is run after another mehdi test.
TEST_CASE("MehdiGPURayCastMultiVolumeRep can remove 1 small volume with clipper.", "[rep][unit][visualization][unstable]")
{
	MehdiGPURayCastMultiVolumeRepFixture fixture;
	fixture.setNumberOfImages(1);
	fixture.setImageAxisSize(10);
	fixture.createRep();

	cx::ImagePtr image = fixture.mImages[0];
	vtkPlanePtr plane = vtkPlanePtr::New();
	plane->SetNormal(0,0,-1);
	plane->SetOrigin(0,0,0);
	image->setInteractiveClipPlane(plane);

	REQUIRE(fixture.renderAndGetNonZeroFraction() == Approx(0.0));
}

// vtk emits PreRender end ERROR (x502) Invalid operation when this is run - in second part.
TEST_CASE("MehdiGPURayCastMultiVolumeRep can clip partially and fully 1 small volume.", "[rep][unit][visualization][not_apple][broken][unstable]")
{
	MehdiGPURayCastMultiVolumeRepFixture fixture;
	fixture.setNumberOfImages(2);
	fixture.setImageAxisSize(10);
	fixture.setViewAxisSize(500);
	fixture.createRep();

	vtkPlanePtr plane = vtkPlanePtr::New();
	plane->SetNormal(0,0,-1);
	plane->SetOrigin(fixture.mImages[0]->boundingBox().center().data());

	fixture.mImages[0]->setInteractiveClipPlane(plane);
	fixture.mImages[1]->setInteractiveClipPlane(plane);

	fixture.requireRepRender();

	plane->SetOrigin(0,0,0);
	fixture.mImages[0]->setInteractiveClipPlane(plane);
	fixture.mImages[1]->setInteractiveClipPlane(plane);
	// vtk emits PreRender end ERROR (x502) Invalid operation here
	// caused by clipping/cropping code not properly cleaned up in previous render???
	double nonZero = fixture.renderAndGetNonZeroFraction();
	REQUIRE(nonZero == Approx(0.0));
}

TEST_CASE("VolumetricRep using vtkVolumeTextureMapper3D can render 1 small volume.", "[rep][integration][visualization]")//not stable as unit
{
	cx::ImagePtr image = cxtest::Utilities::create3DImage(Eigen::Array3i(3,3,3), 200);

	cx::VolumetricRepPtr rep = cx::VolumetricRep::New();
	REQUIRE(rep);
	rep->setImage(image);

	double fraction = repRenderAndGetNonZeroFraction(rep);
	REQUIRE(fraction > 0);
	REQUIRE(fraction < 1);
}

TEST_CASE("VolumetricRep using vtkVolumeTextureMapper3D can render 1 large volume.", "[rep][integration][visualization]")
{
	cx::ImagePtr image = cxtest::Utilities::create3DImage(Eigen::Array3i(300,300,300), 200);

	cx::VolumetricRepPtr rep = cx::VolumetricRep::New();
	REQUIRE(rep);
	rep->setImage(image);

	double fraction = repRenderAndGetNonZeroFraction(rep, 1000);
	REQUIRE(fraction > 0);
	REQUIRE(fraction < 1);
}

TEST_CASE("VolumetricRep using vtkGPUVolumeRayCastMapper can render 1 small volume.", "[rep][integration][visualization]")//not stable as unit
{
	cx::ImagePtr image = cxtest::Utilities::create3DImage(Eigen::Array3i(3,3,3), 200);

	cx::VolumetricRepPtr rep = cx::VolumetricRep::New();
	REQUIRE(rep);
	rep->setUseGPUVolumeRayCastMapper();
	rep->setImage(image);

	double fraction = repRenderAndGetNonZeroFraction(rep);
	REQUIRE(fraction > 0);
	REQUIRE(fraction < 1);
}

} // namespace cxtest


#endif //CX_BUILD_MEHDI_VTKMULTIVOLUME
