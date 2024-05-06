/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxSlider2D.h"
#include "cxtestVisServices.h"
#include "cxImage.h"
#include "cxLogger.h"

namespace
{
typedef boost::shared_ptr<class Slider2DTest> Slider2DTestPtr;
class Slider2DTest : public cx::Slider2D
{
public:
	cx::PLANE_TYPE mPlane;
	cx::ImagePtr mImage;
	int mImageAxisSize;
	double mToolposition;

	Slider2DTest() :
	cx::Slider2D(cxtest::TestVisServices::create(), nullptr, new ctkDoubleSlider(Qt::Vertical)),
		mPlane(cx::ptAXIAL),
		mImage(nullptr),
		mImageAxisSize(10),
		mToolposition(5)
	{
	}

	//Override functions
	cx::PLANE_TYPE getPlaneType()
	{
		return mPlane;
	}
	cx::Transform3D get_sMr()
	{
		return cx::Transform3D::Identity();
	}
	virtual cx::ImagePtr getImage()
	{
		return mImage;
	}
	cx::Vector3D get_tool_d()
	{
		return cx::Vector3D(mToolposition, mToolposition, mToolposition);
	}

	// Allow using protected functions
	int getDimension()
	{
		return cx::Slider2D::getDimension();
	}
	double getOutOfPlaneVoxels()
	{
		return cx::Slider2D::getOutOfPlaneVoxels();
	}
	cx::Vector3D protected_get_tool_d()
	{
		return cx::Slider2D::get_tool_d();
	}
	bool correctSliderValue(double &sliderValue)
	{
		return cx::Slider2D::correctSliderValue(sliderValue);
	}
	void updateSliderDiffIfOutOfRange(double &sliderValueDiff)
	{
		return cx::Slider2D::updateSliderDiffIfOutOfRange(sliderValueDiff);
	}
	void shiftPosOutOfPlane(cx::Vector3D delta_d_voxels)
	{
		return cx::Slider2D::shiftPosOutOfPlane(delta_d_voxels);
	}

	//Utility test functions
	void createDummyImage()
	{
		vtkImageDataPtr dummyImageData = cx::Image::createDummyImageData(mImageAxisSize, 1);
		cx::ImagePtr image(new cx::Image("DummyImage", dummyImageData, "DummyName"));
		mImage = image;
	}
	//Get protected variables
	ctkDoubleSlider *getSlider()
	{
		return mSlider;
	}
	double getLastSliderValue()
	{
		return mLastSliderValue;
	}

public slots:
	void imageChanged()
	{
		return cx::Slider2D::imageChanged();
	}
	void updateSliderPosition()
	{
		return cx::Slider2D::updateSliderPosition();
	}
	void sliderChanged(double sliderValue)
	{
		return cx::Slider2D::sliderChanged(sliderValue);
	}
};
}

TEST_CASE("Slider2DTest", "[unit][plugins][org.custusx.core.view]")
{
	Slider2DTestPtr slider2D = Slider2DTestPtr(new Slider2DTest());
	CHECK(slider2D);
}

TEST_CASE("Slider2D Test with no data", "[unit][plugins][org.custusx.core.view]")
{
	Slider2DTestPtr slider2D = Slider2DTestPtr(new Slider2DTest());
	REQUIRE(slider2D);
	slider2D->imageChanged();
	slider2D->updateSliderPosition();
	slider2D->sliderChanged(0);
	CHECK(slider2D->getOutOfPlaneVoxels() == 0);
	CHECK(slider2D->getDimension() == 2);
	CHECK(slider2D->getPlaneType() == cx::ptAXIAL);
	slider2D->protected_get_tool_d();
	double sliderValue = 0;
	double sliderValueDiff = 0;
	CHECK_FALSE(slider2D->correctSliderValue(sliderValue));
	CHECK(cx::similar(sliderValue, 0));
	slider2D->updateSliderDiffIfOutOfRange(sliderValueDiff);
	CHECK(cx::similar(sliderValueDiff, 0));
	slider2D->shiftPosOutOfPlane(cx::Vector3D::Identity());
}

TEST_CASE("Slider2D Plane type", "[unit][plugins][org.custusx.core.view]")
{
	Slider2DTestPtr slider2D = Slider2DTestPtr(new Slider2DTest());
	slider2D->mPlane = cx::ptAXIAL;
	CHECK(slider2D->getDimension() == 2);
	slider2D->mPlane = cx::ptCORONAL;
	CHECK(slider2D->getDimension() == 1);
	slider2D->mPlane = cx::ptSAGITTAL;
	CHECK(slider2D->getDimension() == 0);
}

TEST_CASE("Slider2D Test setting image", "[unit][plugins][org.custusx.core.view]")
{
	Slider2DTestPtr slider2D = Slider2DTestPtr(new Slider2DTest());
	CHECK_FALSE(slider2D->getImage());
	slider2D->createDummyImage();
	CHECK(slider2D->getImage());
	double initalMax = slider2D->getSlider()->maximum();
	slider2D->imageChanged();
	CHECK(slider2D->getSlider()->maximum() != initalMax);
	CHECK(cx::similar(slider2D->getSlider()->minimum(), 0));
	CHECK(cx::similar(slider2D->getSlider()->maximum(), slider2D->mImageAxisSize));
	CHECK(cx::similar(slider2D->getSlider()->sliderPosition(), slider2D->mToolposition));
}

TEST_CASE("Slider2D getOutOfPlaneVoxels", "[unit][plugins][org.custusx.core.view]")
{
	Slider2DTestPtr slider2D = Slider2DTestPtr(new Slider2DTest());
	slider2D->createDummyImage();
	CHECK(cx::similar(slider2D->getOutOfPlaneVoxels(), slider2D->mToolposition));
}

TEST_CASE("Slider2D updateSliderPosition", "[unit][plugins][org.custusx.core.view]")
{
	Slider2DTestPtr slider2D = Slider2DTestPtr(new Slider2DTest());
	CHECK(cx::similar(slider2D->getSlider()->sliderPosition(), 0));
	slider2D->createDummyImage();
	slider2D->updateSliderPosition();
	CHECK(cx::similar(slider2D->getSlider()->sliderPosition(), slider2D->mToolposition));
}

TEST_CASE("Slider2D correctSliderValue", "[unit][plugins][org.custusx.core.view]")
{
	Slider2DTestPtr slider2D = Slider2DTestPtr(new Slider2DTest());
	slider2D->createDummyImage();
	double sliderValue = 1000;
	CHECK(slider2D->correctSliderValue(sliderValue));
	CHECK(cx::similar(sliderValue, slider2D->mImageAxisSize));
	sliderValue = -5;
	CHECK(slider2D->correctSliderValue(sliderValue));
	CHECK(cx::similar(sliderValue, 0));
	sliderValue = 5;
	CHECK_FALSE(slider2D->correctSliderValue(sliderValue));
}

TEST_CASE("Slider2D sliderChanged", "[unit][plugins][org.custusx.core.view]")
{
	Slider2DTestPtr slider2D = Slider2DTestPtr(new Slider2DTest());
	CHECK(cx::similar(slider2D->getLastSliderValue(), 0));
	slider2D->createDummyImage();
	slider2D->imageChanged();
	CHECK(cx::similar(slider2D->getLastSliderValue(), 5));
	double sliderValue = 7;
	slider2D->sliderChanged(sliderValue);
	CHECK(cx::similar(sliderValue, slider2D->getLastSliderValue()));
}
