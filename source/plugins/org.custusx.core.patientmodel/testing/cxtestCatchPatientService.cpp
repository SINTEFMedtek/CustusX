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

TEST_CASE("Active Image: set/get", "[unit]")
{
	cx::DataServicePtr dataManager = cx::DataManagerImpl::create();
	testDataStructures testData;

	dataManager->setActiveImage(testData.image1);

	REQUIRE(dataManager->getActiveImage() == testData.image1);
	REQUIRE_FALSE(dataManager->getActiveImage() == testData.image2);

	dataManager->setActiveData(testData.mesh1);
	REQUIRE(dataManager->getActiveImage() == testData.image1);
}


TEST_CASE("Active Image: activeImageChanged signal", "[unit]")
{
	cx::DataServicePtr dataManager = cx::DataManagerImpl::create();
	testDataStructures testData;

	cxtest::DirectSignalListener signalListener(dataManager.get(), SIGNAL(activeImageChanged(QString)));
	dataManager->setActiveImage(testData.image1);
	CHECK(signalListener.isReceived());

	cxtest::DirectSignalListener signalListener2(dataManager.get(), SIGNAL(activeImageChanged(QString)));
	dataManager->setActiveData(testData.image2);
	CHECK(signalListener2.isReceived());

	cxtest::DirectSignalListener signalListener3(dataManager.get(), SIGNAL(activeImageChanged(QString)));
	dataManager->setActiveData(testData.mesh1);
	CHECK_FALSE(signalListener3.isReceived());
}

TEST_CASE("Active Image: save/load in patient file", "[unit]")
{
	cx::DataManagerImplPtr dataManager = cx::DataManagerImpl::create();
	testDataStructures testData;
	cxtest::SessionStorageTestFixture storageFixture;

	cx::SpaceProviderPtr space(new cx::SpaceProviderImpl(cx::TrackingService::getNullObject(), cx::PatientModelService::getNullObject()));
	cx::DataFactoryPtr factory(new cx::DataFactory(cx::PatientModelService::getNullObject(), space));

	dataManager->setDataFactory(factory);

	cx::PatientDataPtr patientData = cx::PatientDataPtr(new cx::PatientData(dataManager, storageFixture.mSessionStorageService));

	storageFixture.createSessions();
	storageFixture.loadSession1();

	QString filename = cx::DataLocations::getExistingTestData("testing/TubeSegmentationFramework", "Default.mhd");
	QString info;
	cx::DataPtr data1 = patientData->importData(filename, info);
	REQUIRE(data1);

	dataManager->loadData(data1);


	CHECK_FALSE(dataManager->getActiveImage() == data1);

	dataManager->setActiveData(testData.image2);
	dataManager->setActiveData(data1);
	dataManager->setActiveData(testData.mesh1);
	CHECK(dataManager->getActiveImage() == data1);
	storageFixture.saveSession();


	storageFixture.loadSession2();
	CHECK_FALSE(dataManager->getActiveImage() == data1);
	dataManager->setActiveImage(testData.image2);
	storageFixture.saveSession();

	CHECK(dataManager->getActiveImage() == testData.image2);

	storageFixture.reloadSession1();
	REQUIRE(dataManager->getActiveImage());
	CHECK(dataManager->getActiveImage()->getUid() == data1->getUid());
}

TEST_CASE("Active Data: set/get", "[unit]")
{
	cx::DataServicePtr dataManager = cx::DataManagerImpl::create();
	testDataStructures testData;

	dataManager->setActiveData(testData.image1);

	REQUIRE(dataManager->getActiveData() == testData.image1);
	REQUIRE_FALSE(dataManager->getActiveData() == testData.image2);

	dataManager->setActiveData(testData.image2);
	REQUIRE(dataManager->getActiveData() == testData.image2);
}

TEST_CASE("Active Data: Get data of specific type", "[unit]")
{
	cx::DataServicePtr dataManager = cx::DataManagerImpl::create();
	testDataStructures testData;

	dataManager->setActiveData(testData.image1);
	REQUIRE(dataManager->getActiveData<cx::Image>() == testData.image1);
	REQUIRE_FALSE(dataManager->getActiveData<cx::Mesh>());

	dataManager->setActiveData(testData.mesh1);
	REQUIRE(dataManager->getActiveData<cx::Mesh>() == testData.mesh1);
	REQUIRE(dataManager->getActiveData<cx::Image>() == testData.image1);

	dataManager->setActiveData(testData.image2);
	REQUIRE(dataManager->getActiveData<cx::Image>() == testData.image2);
	REQUIRE(dataManager->getActiveData<cx::Mesh>() == testData.mesh1);
	REQUIRE_FALSE(dataManager->getActiveData<cx::Image>() == testData.image1);
}

TEST_CASE("Active Data: activeDataChanged signal", "[unit]")
{
	cx::DataServicePtr dataManager = cx::DataManagerImpl::create();
	testDataStructures testData;

	cxtest::DirectSignalListener signalListener(dataManager.get(), SIGNAL(activeDataChanged(QString)));
	dataManager->setActiveData(testData.image2);
	CHECK(signalListener.isReceived());

	cxtest::DirectSignalListener signalListener2(dataManager.get(), SIGNAL(activeDataChanged(QString)));
	dataManager->setActiveData(testData.mesh1);
	CHECK(signalListener2.isReceived());
}

TEST_CASE("Active Data: Call set multiple times", "[unit]")
{
	cx::DataServicePtr dataManager = cx::DataManagerImpl::create();
	testDataStructures testData;

	dataManager->setActiveData(testData.image1);
	REQUIRE(dataManager->getActiveData() == testData.image1);

	dataManager->setActiveData(testData.image2);
	REQUIRE(dataManager->getActiveData() == testData.image2);

	dataManager->setActiveData(testData.image1);
	dataManager->setActiveData(testData.image1);
	REQUIRE(dataManager->getActiveData() == testData.image1);
}