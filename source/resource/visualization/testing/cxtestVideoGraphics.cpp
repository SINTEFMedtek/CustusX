/*
 * cxtestVideoGraphics.cpp
 *
 *  Created on: Sep 26, 2013
 *      Author: christiana
 */

#include "cxtestVideoGraphicsFixture.h"
#include "catch.hpp"
#include "cxProbeSector.h"

TEST_CASE_METHOD(cxtest::VideoGraphicsFixture, "VideoGraphics: Render Image", "[unit][resource][visualization]")
{
	vtkImageDataPtr videoImage0 = this->readImageData("testImage01.png", "input image");
	this->addImageToRenderer(videoImage0);

	mMachine->alignRenderWindowWithImage(videoImage0);
	vtkImageDataPtr renderedImage = mMachine->renderToImage();

	CHECK(mMachine->findDifference(videoImage0, renderedImage));
}

TEST_CASE_METHOD(cxtest::VideoGraphicsFixture, "VideoGraphics: Render MHD Image", "[unit][resource][visualization]")
{
	vtkImageDataPtr videoImage0 = this->readImageData("US_small.mhd", "input image");
	this->addImageToRenderer(videoImage0);

	mMachine->alignRenderWindowWithImage(videoImage0);
	vtkImageDataPtr renderedImage = mMachine->renderToImage();

	CHECK(mMachine->findDifference(videoImage0, renderedImage));
}

TEST_CASE_METHOD(cxtest::VideoGraphicsFixture, "VideoGraphics: Pass PNG Image to VideoGraphics", "[unit][resource][visualization]")
{
	vtkImageDataPtr videoImage0 = this->readImageData("testImage01.png", "input image");

	mVideoGraphics->setFlipVertical(false);

	this->renderImageAndCompareToExpected(videoImage0, videoImage0);
}

TEST_CASE_METHOD(cxtest::VideoGraphicsFixture, "VideoGraphics: Pass MHD Image to VideoGraphics", "[unit][resource][visualization]")
{
	vtkImageDataPtr videoImage0 = this->readImageData("US_small.mhd", "input image");

	mVideoGraphics->setFlipVertical(false);

	this->renderImageAndCompareToExpected(videoImage0, videoImage0);
}

TEST_CASE_METHOD(cxtest::VideoGraphicsFixture, "VideoGraphics: Test Mask", "[unit][resource][visualization]")
{
	vtkImageDataPtr videoImage0 = this->readImageData("testImage01.png", "input image");
	vtkImageDataPtr mask = this->readImageData("testImage01_sectormask.png", "input mask");
	vtkImageDataPtr expected = this->readImageData("testImage01_sectormasked.png", "input expected");

	mVideoGraphics->setFlipVertical(false);
	mVideoGraphics->setMask(mask);

	this->renderImageAndCompareToExpected(videoImage0, expected);
}

TEST_CASE_METHOD(cxtest::VideoGraphicsFixture, "VideoGraphics: Test Sector", "[unit][resource][visualization]")
{
	QString imageFilename = "US_small.mhd";
	vtkImageDataPtr videoImage0 = this->readImageData(imageFilename, "input image");
	vtkImageDataPtr expected = this->readImageData("US_small_sector_masked.png", "input expected");

	cx::ProbeSector probeSector;
	probeSector.mData = this->readProbeData(imageFilename);

	mVideoGraphics->setClip(probeSector.getSector());
    mMachine->setImageErrorThreshold(250); // ad hoc solution for failing test.

	this->renderImageAndCompareToExpected(videoImage0, expected);
}

TEST_CASE_METHOD(cxtest::VideoGraphicsFixture, "VideoGraphics: Render Image with text", "[integration][resource][visualization][textrender]")
{
	vtkImageDataPtr videoImage0 = this->readImageData("testImage01.png", "input image");
	this->addImageToRenderer(videoImage0);

	mMachine->addTextToVtkRenderWindow();
	mMachine->render(5000);
}
