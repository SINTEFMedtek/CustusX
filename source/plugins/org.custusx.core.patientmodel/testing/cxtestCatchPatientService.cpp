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
#include "cxDataManagerImpl.h"
#include "cxtestDummyDataManager.h"
#include "cxtestDirectSignalListener.h"
#include "cxtestSessionStorageTestFixture.h"
#include "cxPatientData.h"
#include "cxDataLocations.h"
#include "cxPatientModelService.h"
#include "cxNullDeleter.h"
#include "cxSpaceProviderImpl.h"
#include "cxTrackingService.h"
#include "cxDataFactory.h"
#include "cxSelectDataStringProperty.h"

namespace {

struct testDataStructures
{
	cx::ImagePtr image1;
	cx::ImagePtr image2;
	cx::MeshPtr mesh1;
	testDataStructures()
	{
		vtkImageDataPtr dummyImageData = cx::Image::createDummyImageData(2, 1);
		image1 = cx::ImagePtr(new cx::Image("imageUid1", dummyImageData, "imageName1"));
		image2 = cx::ImagePtr(new cx::Image("imageUid2", dummyImageData, "imageName2"));
		mesh1 = cx::Mesh::create("meshUid1","meshName1");
	}
};

} // namespace

TEST_CASE("DataManagerImpl setup/shutdown works multiple times", "[unit]")
{
    for (unsigned i=0; i<2; ++i)
    {
		cx::DataServicePtr service = cx::DataManagerImpl::create();
		REQUIRE(service);
		CHECK(service.unique());
		service.reset();
    }
}

//Moved
TEST_CASE("Active Image: set/get", "[unit]")
{
	cxtest::SessionStorageTestFixture storageFixture;
	cx::PatientModelServicePtr patientModelService = storageFixture.mPatientModelService;
	testDataStructures testData;

	patientModelService->setActiveData(testData.image1);

	REQUIRE(patientModelService->getActiveData<cx::Image>() == testData.image1);
	REQUIRE_FALSE(patientModelService->getActiveData<cx::Image>() == testData.image2);

	patientModelService->setActiveData(testData.mesh1);
	REQUIRE(patientModelService->getActiveData<cx::Image>() == testData.image1);
}


//Moved
TEST_CASE("Active Image: activeImageChanged signal", "[unit]")
{
	cxtest::SessionStorageTestFixture storageFixture;
	cx::PatientModelServicePtr patientModelService = storageFixture.mPatientModelService;
	testDataStructures testData;

	cxtest::DirectSignalListener signalListener(patientModelService.get(), SIGNAL(activeImageChanged(QString)));
	patientModelService->setActiveData(testData.image1);
	CHECK(signalListener.isReceived());

	cxtest::DirectSignalListener signalListener3(patientModelService.get(), SIGNAL(activeImageChanged(QString)));
	patientModelService->setActiveData(testData.mesh1);
	CHECK_FALSE(signalListener3.isReceived());

	cxtest::DirectSignalListener signalListener2(patientModelService.get(), SIGNAL(activeImageChanged(QString)));
	patientModelService->setActiveData(testData.image1);
	CHECK(signalListener2.isReceived());
}

TEST_CASE("Active Image: save/load in patient file", "[unit]")
{
	testDataStructures testData;
	cxtest::SessionStorageTestFixture storageFixture;
	cx::PatientModelServicePtr patientModelService = storageFixture.mPatientModelService;

	storageFixture.createSessions();
	storageFixture.loadSession1();

	QString filename = cx::DataLocations::getExistingTestData("testing/TubeSegmentationFramework", "Default.mhd");
	QString info;
	cx::DataPtr data1 = patientModelService->importData(filename, info);
	REQUIRE(data1);

	patientModelService->insertData(data1);

	CHECK_FALSE(patientModelService->getActiveData() == data1);

	patientModelService->setActiveData(testData.image2);
	patientModelService->setActiveData(data1);
	patientModelService->setActiveData(testData.mesh1);
	CHECK(patientModelService->getActiveData<cx::Image>() == data1);
	storageFixture.saveSession();


	storageFixture.loadSession2();
	CHECK_FALSE(patientModelService->getActiveData<cx::Image>() == data1);
	patientModelService->setActiveData(testData.image2);
	storageFixture.saveSession();

	CHECK(patientModelService->getActiveData<cx::Image>() == testData.image2);

	storageFixture.reloadSession1();
	REQUIRE(patientModelService->getActiveData<cx::Image>());
	CHECK(patientModelService->getActiveData<cx::Image>()->getUid() == data1->getUid());
}

//Moved
TEST_CASE("Active Data: set/get", "[unit]")
{
	cxtest::SessionStorageTestFixture storageFixture;
	cx::PatientModelServicePtr patientModelService = storageFixture.mPatientModelService;
	testDataStructures testData;

	patientModelService->setActiveData(testData.image1);

	REQUIRE(patientModelService->getActiveData() == testData.image1);
	REQUIRE_FALSE(patientModelService->getActiveData() == testData.image2);

	patientModelService->setActiveData(testData.image2);
	REQUIRE(patientModelService->getActiveData() == testData.image2);
}

//Moved
TEST_CASE("Active Data: Get data of specific type", "[unit]")
{
	cxtest::SessionStorageTestFixture storageFixture;
	cx::PatientModelServicePtr patientModelService = storageFixture.mPatientModelService;
	testDataStructures testData;

	patientModelService->setActiveData(testData.image1);
	REQUIRE(patientModelService->getActiveData<cx::Image>() == testData.image1);
	REQUIRE_FALSE(patientModelService->getActiveData<cx::Mesh>());

	patientModelService->setActiveData(testData.mesh1);
	REQUIRE(patientModelService->getActiveData<cx::Mesh>() == testData.mesh1);
	REQUIRE(patientModelService->getActiveData<cx::Image>() == testData.image1);

	patientModelService->setActiveData(testData.image2);
	REQUIRE(patientModelService->getActiveData<cx::Image>() == testData.image2);
	REQUIRE(patientModelService->getActiveData<cx::Mesh>() == testData.mesh1);
	REQUIRE_FALSE(patientModelService->getActiveData<cx::Image>() == testData.image1);
}

//Moved
TEST_CASE("Active Data: activeDataChanged signal", "[unit]")
{
	testDataStructures testData;
	cxtest::SessionStorageTestFixture storageFixture;
	cx::PatientModelServicePtr patientModelService = storageFixture.mPatientModelService;

	cxtest::DirectSignalListener signalListener(patientModelService.get(), SIGNAL(activeDataChanged(QString)));
	patientModelService->setActiveData(testData.image2);
	CHECK(signalListener.isReceived());

	cxtest::DirectSignalListener signalListener2(patientModelService.get(), SIGNAL(activeDataChanged(QString)));
	patientModelService->setActiveData(testData.mesh1);
	CHECK(signalListener2.isReceived());
}

//Moved
TEST_CASE("Active Data: Remove data", "[unit]")
{
	cxtest::SessionStorageTestFixture storageFixture;
	cx::PatientModelServicePtr patientModelService = storageFixture.mPatientModelService;
	testDataStructures testData;

	patientModelService->insertData(testData.image1);
	patientModelService->insertData(testData.image2);
	patientModelService->insertData(testData.mesh1);

	patientModelService->setActiveData(testData.mesh1);
	patientModelService->setActiveData(testData.image2);
	patientModelService->setActiveData(testData.image1);

	cxtest::DirectSignalListener signalListener(patientModelService.get(), SIGNAL(activeDataChanged(QString)));
	patientModelService->removeData(testData.image1->getUid());
	CHECK(patientModelService->getActiveData() == testData.image2);
	CHECK(signalListener.isReceived());

	patientModelService->removeData(testData.image2->getUid());
	CHECK(patientModelService->getActiveData() == testData.mesh1);

	patientModelService->removeData(testData.mesh1->getUid());
	CHECK_FALSE(patientModelService->getActiveData());
}

//Moved
TEST_CASE("Active Data: Call set multiple times", "[unit]")
{
	cxtest::SessionStorageTestFixture storageFixture;
	cx::PatientModelServicePtr patientModelService = storageFixture.mPatientModelService;
	testDataStructures testData;

	patientModelService->setActiveData(testData.image1);
	REQUIRE(patientModelService->getActiveData() == testData.image1);

	patientModelService->setActiveData(testData.image2);
	REQUIRE(patientModelService->getActiveData() == testData.image2);

	patientModelService->setActiveData(testData.image1);
	patientModelService->setActiveData(testData.image1);
	REQUIRE(patientModelService->getActiveData() == testData.image1);
}

TEST_CASE("Set ActiveData using uid", "[unit]")
{
	cxtest::SessionStorageTestFixture storageFixture;
	cx::PatientModelServicePtr patientModelService = storageFixture.mPatientModelService;
	testDataStructures testData;

	patientModelService->insertData(testData.image1);

	patientModelService->setActiveData(testData.image1->getUid());
	REQUIRE(patientModelService->getActiveData() == testData.image1);
}

TEST_CASE("StringPropertyActiveData works", "[unit][resource][core]")
{
	testDataStructures testData;
	cxtest::SessionStorageTestFixture storageFixture;
	cx::PatientModelServicePtr patientModelService = storageFixture.mPatientModelService;

	patientModelService->insertData(testData.image1);

	cx::StringPropertyActiveDataPtr activeDataProperty = cx::StringPropertyActiveData::New(patientModelService, "image");
	REQUIRE(activeDataProperty);

	activeDataProperty->setValue(testData.image1->getUid());
	REQUIRE(activeDataProperty->getValue() == testData.image1->getUid());
}
