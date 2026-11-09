/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include <vtkImageData.h>
#include "cxRegistrationTransform.h"
#include "cxImageLUT2D.h"
#include "cxActiveData.h"
#include "cxMesh.h"
#include "cxImage.h"
#include "cxTrackedStream.h"
#include "cxtestDirectSignalListener.h"
#include "cxTestVideoSource.h"
#include "cxPatientModelService.h"
#include "cxtestTestDataStructures.h"

TEST_CASE("ActiveData: set/get", "[unit]")
{
	cxtest::TestDataStructures testData;
	cx::ActiveData activeData(cx::PatientModelService::getNullObject(), cx::SessionStorageService::getNullObject());

	activeData.setActive(testData.image1);

	CHECK(activeData.getActive() == testData.image1);
	CHECK_FALSE(activeData.getActive() == testData.image2);

	activeData.setActive(testData.image2);
	CHECK(activeData.getActive() == testData.image2);
}

TEST_CASE("ActiveData: Active Image set/get", "[unit]")
{
	cxtest::TestDataStructures testData;
	cx::ActiveData activeData(cx::PatientModelService::getNullObject(), cx::SessionStorageService::getNullObject());

	activeData.setActive(testData.image1);

	CHECK(activeData.getActive<cx::Image>() == testData.image1);
	CHECK_FALSE(activeData.getActive<cx::Image>() == testData.image2);

	activeData.setActive(testData.mesh1);
	CHECK(activeData.getActive<cx::Image>() == testData.image1);
}

TEST_CASE("ActiveData: Active Image - activeImageChanged signal", "[unit]")
{
	cxtest::TestDataStructures testData;
	cx::ActiveData activeData(cx::PatientModelService::getNullObject(), cx::SessionStorageService::getNullObject());

	cxtest::DirectSignalListener signalListener(&activeData, SIGNAL(activeImageChanged(QString)));
	activeData.setActive(testData.image1);
	CHECK(signalListener.isReceived());

	cxtest::DirectSignalListener signalListener3(&activeData, SIGNAL(activeImageChanged(QString)));
	activeData.setActive(testData.mesh1);
	CHECK_FALSE(signalListener3.isReceived());

	cxtest::DirectSignalListener signalListener2(&activeData, SIGNAL(activeImageChanged(QString)));
	activeData.setActive(testData.image1);
	CHECK(signalListener2.isReceived());
}

TEST_CASE("ActiveData: Get data of specific type", "[unit]")
{
	cxtest::TestDataStructures testData;
	cx::ActiveData activeData(cx::PatientModelService::getNullObject(), cx::SessionStorageService::getNullObject());

	activeData.setActive(testData.image1);
	CHECK(activeData.getActive<cx::Image>() == testData.image1);
	CHECK_FALSE(activeData.getActive<cx::Mesh>());

	activeData.setActive(testData.mesh1);
	CHECK(activeData.getActive<cx::Mesh>() == testData.mesh1);
	CHECK(activeData.getActive<cx::Image>() == testData.image1);

	activeData.setActive(testData.image2);
	CHECK(activeData.getActive<cx::Image>() == testData.image2);
	CHECK(activeData.getActive<cx::Mesh>() == testData.mesh1);
	CHECK_FALSE(activeData.getActive<cx::Image>() == testData.image1);
}

TEST_CASE("ActiveData: activeDataChanged signal", "[unit]")
{
	cxtest::TestDataStructures testData;
	cx::ActiveData activeData(cx::PatientModelService::getNullObject(), cx::SessionStorageService::getNullObject());

	cxtest::DirectSignalListener signalListener(&activeData, SIGNAL(activeDataChanged(QString)));
	activeData.setActive(testData.image2);
	CHECK(signalListener.isReceived());

	cxtest::DirectSignalListener signalListener2(&activeData, SIGNAL(activeDataChanged(QString)));
	activeData.setActive(testData.mesh1);
	CHECK(signalListener2.isReceived());
}

TEST_CASE("ActiveData: Call set multiple times", "[unit]")
{
	cxtest::TestDataStructures testData;
	cx::ActiveData activeData(cx::PatientModelService::getNullObject(), cx::SessionStorageService::getNullObject());

	activeData.setActive(testData.image1);
	CHECK(activeData.getActive() == testData.image1);

	activeData.setActive(testData.image2);
	CHECK(activeData.getActive() == testData.image2);

	activeData.setActive(testData.image1);
	activeData.setActive(testData.image1);
	CHECK(activeData.getActive() == testData.image1);
}

TEST_CASE("ActiveData: Get using type regexp", "[unit]")
{
	cxtest::TestDataStructures testData;
	cx::ActiveData activeData(cx::PatientModelService::getNullObject(), cx::SessionStorageService::getNullObject());

	activeData.setActive(testData.image2);
	activeData.setActive(testData.mesh1);
	activeData.setActive(testData.image1);

	QString regexp(cx::Image::getTypeName());
	CHECK(activeData.getActiveUsingRegexp(regexp) == activeData.getActive());

	regexp = cx::Mesh::getTypeName();
	REQUIRE(activeData.getActiveUsingRegexp(regexp));
	CHECK(activeData.getActiveUsingRegexp(regexp)->getUid() == testData.mesh1->getUid());
}

TEST_CASE("ActiveData: Get images both from Image and TrackedStream", "[unit]")
{
	cxtest::TestDataStructures testData;
	cx::ActiveData activeData(cx::PatientModelService::getNullObject(), cx::SessionStorageService::getNullObject());
	activeData.setActive(testData.image1);

	REQUIRE(activeData.getDerivedActiveImage());
	CHECK(activeData.getDerivedActiveImage() == testData.image1);

	activeData.setActive(testData.trackedStream1);
	CHECK_FALSE(activeData.getDerivedActiveImage());
}

TEST_CASE("ActiveData: Get image from TrackedStream with VideoSource", "[unit]")
{
	cxtest::TestDataStructures testData;
	cx::ActiveData activeData(cx::PatientModelService::getNullObject(), cx::SessionStorageService::getNullObject());
	activeData.setActive(testData.trackedStream1);

	cx::TestVideoSourcePtr videoSource(new cx::TestVideoSource("TestVideoSourceUid", "TestVideoSource" , 80, 40));
	testData.trackedStream1->setVideoSource(videoSource);
	REQUIRE(activeData.getDerivedActiveImage());
	CHECK(activeData.getDerivedActiveImage()->getUid() != testData.trackedStream1->getUid());
	CHECK(activeData.getDerivedActiveImage()->getUid().contains(testData.trackedStream1->getUid()));
}

TEST_CASE("TrackedStream: a new video source gives a new image with the settings of the old one", "[unit]")
{
	cxtest::TestDataStructures testData;
	cx::TrackedStreamPtr stream = testData.trackedStream1;
	cx::TestVideoSourcePtr source(new cx::TestVideoSource("TestVideoSourceUid", "TestVideoSource" , 80, 40));
	stream->setVideoSource(source);
	cx::ImagePtr oldImage = stream->getChangingImage();
	oldImage->getLookupTable2D()->setWindow(77);
	cx::Transform3D rMd = cx::createTransformTranslate(cx::Vector3D(1, 2, 3));
	oldImage->get_rMd_History()->setRegistration(rMd);
	stream->setVideoSource(source);
	CHECK(stream->getChangingImage() == oldImage);

	cx::TestVideoSourcePtr newSource(new cx::TestVideoSource("TestVideoSourceUid", "TestVideoSource" , 80, 40));
	stream->setVideoSource(newSource);
	cx::ImagePtr newImage = stream->getChangingImage();
	CHECK(newImage != oldImage);
	CHECK(newImage->getBaseVtkImageData() == newSource->getVtkImageData());
	CHECK(newImage->getLookupTable2D()->getWindow() == Approx(77));
	CHECK(cx::similar(newImage->get_rMd(), rMd));

	oldImage->getLookupTable2D()->setWindow(50); // e.g. a widget that still edits the old image
	CHECK(newImage->getLookupTable2D()->getWindow() == Approx(50));
}

TEST_CASE("ActiveData: getActiveImageUid", "[unit]")
{
	cxtest::TestDataStructures testData;
	cx::ActiveData activeData(cx::PatientModelService::getNullObject(), cx::SessionStorageService::getNullObject());
	activeData.setActive(testData.image1);
	CHECK(activeData.getActiveImageUid() == testData.image1->getUid());
}

TEST_CASE("ActiveData: remove", "[unit]")
{
	cxtest::TestDataStructures testData;
	cx::ActiveData activeData(cx::PatientModelService::getNullObject(), cx::SessionStorageService::getNullObject());
	activeData.setActive(testData.image1);
	activeData.setActive(testData.image2);

	cxtest::DirectSignalListener signalListener(&activeData, SIGNAL(activeDataChanged(QString)));
	cxtest::DirectSignalListener signalListener2(&activeData, SIGNAL(activeImageChanged(QString)));
	activeData.remove(testData.image2);
	CHECK(signalListener.isReceived());
	CHECK(signalListener2.isReceived());

	CHECK(activeData.getActive() == testData.image1);

	activeData.remove(testData.image1);
	CHECK_FALSE(activeData.getActive());
}
