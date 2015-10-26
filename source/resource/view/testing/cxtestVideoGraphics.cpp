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
#include "cxtestVideoGraphicsFixture.h"
#include "catch.hpp"
#include "cxProbeSector.h"
#include "cxVtkHelperClasses.h"

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
	probeSector.mData = this->readProbeDefinition(imageFilename);

	mVideoGraphics->setClip(probeSector.getSector());
    mMachine->setImageErrorThreshold(250); // ad hoc solution for failing test.

	this->renderImageAndCompareToExpected(videoImage0, expected);
}

TEST_CASE_METHOD(cxtest::VideoGraphicsFixture, "VideoGraphics: Render Image with text", "[integration][resource][visualization][textrender]")
{
	vtkImageDataPtr videoImage0 = this->readImageData("testImage01.png", "input image");
	this->addImageToRenderer(videoImage0);

//	cx::TextDisplay::forceUseVtkTextMapper();
	mMachine->addTextToVtkRenderWindow("test text");
	mMachine->renderAndUpdateText(5000);
}
