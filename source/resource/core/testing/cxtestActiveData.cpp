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
#include "cxActiveData.h"
#include "cxMesh.h"
#include "cxImage.h"
#include "cxTrackedStream.h"
#include "cxtestDirectSignalListener.h"
#include "cxTestVideoSource.h"


namespace
{
struct testDataStructures
{
	cx::ImagePtr image1;
	cx::ImagePtr image2;
	cx::MeshPtr mesh1;
	cx::TrackedStreamPtr trackedStream1;
	testDataStructures()
	{
		vtkImageDataPtr dummyImageData = cx::Image::createDummyImageData(2, 1);
		image1 = cx::ImagePtr(new cx::Image("imageUid1", dummyImageData, "imageName1"));
		image2 = cx::ImagePtr(new cx::Image("imageUid2", dummyImageData, "imageName2"));
		mesh1 = cx::Mesh::create("meshUid1","meshName1");
		trackedStream1 = cx::TrackedStream::create("trackedStreamUid1","trackedStreamName1");
	}
};
} // namespace

TEST_CASE("ActiveData: set/get", "[unit]")
{
	testDataStructures testData;
	cx::ActiveData activeData;

	activeData.setActiveData(testData.image1);

	CHECK(activeData.getActiveData() == testData.image1);
	CHECK_FALSE(activeData.getActiveData() == testData.image2);

	activeData.setActiveData(testData.image2);
	CHECK(activeData.getActiveData() == testData.image2);
}

TEST_CASE("ActiveData: Active Image set/get", "[unit]")
{
	testDataStructures testData;
	cx::ActiveData activeData;

	activeData.setActiveData(testData.image1);

	CHECK(activeData.getActiveData<cx::Image>() == testData.image1);
	CHECK_FALSE(activeData.getActiveData<cx::Image>() == testData.image2);

	activeData.setActiveData(testData.mesh1);
	CHECK(activeData.getActiveData<cx::Image>() == testData.image1);
}

TEST_CASE("ActiveData: Active Image - activeImageChanged signal", "[unit]")
{
	testDataStructures testData;
	cx::ActiveData activeData;

	cxtest::DirectSignalListener signalListener(&activeData, SIGNAL(activeImageChanged(QString)));
	activeData.setActiveData(testData.image1);
	CHECK(signalListener.isReceived());

	cxtest::DirectSignalListener signalListener3(&activeData, SIGNAL(activeImageChanged(QString)));
	activeData.setActiveData(testData.mesh1);
	CHECK_FALSE(signalListener3.isReceived());

	cxtest::DirectSignalListener signalListener2(&activeData, SIGNAL(activeImageChanged(QString)));
	activeData.setActiveData(testData.image1);
	CHECK(signalListener2.isReceived());
}

TEST_CASE("ActiveData: Get data of specific type", "[unit]")
{
	testDataStructures testData;
	cx::ActiveData activeData;

	activeData.setActiveData(testData.image1);
	CHECK(activeData.getActiveData<cx::Image>() == testData.image1);
	CHECK_FALSE(activeData.getActiveData<cx::Mesh>());

	activeData.setActiveData(testData.mesh1);
	CHECK(activeData.getActiveData<cx::Mesh>() == testData.mesh1);
	CHECK(activeData.getActiveData<cx::Image>() == testData.image1);

	activeData.setActiveData(testData.image2);
	CHECK(activeData.getActiveData<cx::Image>() == testData.image2);
	CHECK(activeData.getActiveData<cx::Mesh>() == testData.mesh1);
	CHECK_FALSE(activeData.getActiveData<cx::Image>() == testData.image1);
}

TEST_CASE("ActiveData: activeDataChanged signal", "[unit]")
{
	testDataStructures testData;
	cx::ActiveData activeData;

	cxtest::DirectSignalListener signalListener(&activeData, SIGNAL(activeDataChanged(QString)));
	activeData.setActiveData(testData.image2);
	CHECK(signalListener.isReceived());

	cxtest::DirectSignalListener signalListener2(&activeData, SIGNAL(activeDataChanged(QString)));
	activeData.setActiveData(testData.mesh1);
	CHECK(signalListener2.isReceived());
}

TEST_CASE("ActiveData: Call set multiple times", "[unit]")
{
	testDataStructures testData;
	cx::ActiveData activeData;

	activeData.setActiveData(testData.image1);
	CHECK(activeData.getActiveData() == testData.image1);

	activeData.setActiveData(testData.image2);
	CHECK(activeData.getActiveData() == testData.image2);

	activeData.setActiveData(testData.image1);
	activeData.setActiveData(testData.image1);
	CHECK(activeData.getActiveData() == testData.image1);
}

TEST_CASE("ActiveData: Get using type regexp", "[unit]")
{
	testDataStructures testData;
	cx::ActiveData activeData;

	activeData.setActiveData(testData.image2);
	activeData.setActiveData(testData.mesh1);
	activeData.setActiveData(testData.image1);

	QString regexp("image");
	CHECK(activeData.getActiveData(regexp) == activeData.getActiveData());

	regexp = "mesh";
	REQUIRE(activeData.getActiveData(regexp));
	CHECK(activeData.getActiveData(regexp)->getUid() == testData.mesh1->getUid());
}

TEST_CASE("ActiveData: Get images both from Image and TrackedStream", "[unit]")
{
	testDataStructures testData;
	cx::ActiveData activeData;
	activeData.setActiveData(testData.image1);

	REQUIRE(activeData.getDerivedActiveImage());
	CHECK(activeData.getDerivedActiveImage() == testData.image1);

	activeData.setActiveData(testData.trackedStream1);
	CHECK_FALSE(activeData.getDerivedActiveImage());
}

TEST_CASE("ActiveData: Get image from TrackedStream with VideoSource", "[unit]")
{
	testDataStructures testData;
	cx::ActiveData activeData;
	activeData.setActiveData(testData.trackedStream1);

	cx::TestVideoSourcePtr videoSource(new cx::TestVideoSource("TestVideoSourceUid", "TestVideoSource" , 80, 40));
	testData.trackedStream1->setVideoSource(videoSource);
	REQUIRE(activeData.getDerivedActiveImage());
	CHECK(activeData.getDerivedActiveImage()->getUid() != testData.trackedStream1->getUid());
	CHECK(activeData.getDerivedActiveImage()->getUid().contains(testData.trackedStream1->getUid()));
}
