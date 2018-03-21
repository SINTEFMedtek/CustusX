/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxDataManagerImpl.h"
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
#include "cxActiveData.h"
#include "cxTypeConversions.h"

namespace cxtest {

TEST_CASE("DataManagerImpl setup/shutdown works multiple times", "[unit]")
{
	cx::ActiveDataPtr activeData = cx::ActiveDataPtr(new cx::ActiveData(cx::PatientModelService::getNullObject(), cx::SessionStorageService::getNullObject()));
    for (unsigned i=0; i<2; ++i)
    {
		cx::DataServicePtr service = cx::DataManagerImpl::create(activeData);
		REQUIRE(service);
		CHECK(service.unique());
		service.reset();
    }
}

TEST_CASE("ActiveData: save/load in patient file", "[unit]")
{
	SessionStorageTestFixture storageFixture;
	cx::PatientModelServicePtr patientModelService = storageFixture.mPatientModelService;
	TestDataStructures testData;
	cx::ActiveDataPtr activeData = patientModelService->getActiveData();

	storageFixture.createSessions();
	storageFixture.loadSession1();

	QString filename = cx::DataLocations::getExistingTestData("testing/default_volume", "Default.mhd");
	QString info;
	cx::DataPtr data1 = patientModelService->importData(filename, info);
	REQUIRE(data1);

	patientModelService->insertData(testData.mesh1);
	patientModelService->insertData(testData.image2);
	patientModelService->insertData(data1);

	CHECK_FALSE(activeData->getActive() == data1);

	activeData->setActive(testData.image2);
	activeData->setActive(data1);
	activeData->setActive(testData.mesh1);
	CHECK(activeData->getActive<cx::Image>() == data1);
	storageFixture.saveSession();


	storageFixture.loadSession2();
	CHECK_FALSE(activeData->getActive<cx::Image>() == data1);
	activeData->setActive(testData.image2);
	storageFixture.saveSession();

	CHECK(activeData->getActive<cx::Image>() == testData.image2);

	storageFixture.reloadSession1();
	REQUIRE(activeData->getActive<cx::Image>());
	CHECK(activeData->getActive<cx::Image>()->getUid() == data1->getUid());
}

TEST_CASE("ActiveData: Set using uid", "[unit]")
{
	SessionStorageTestFixture storageFixture;
	cx::PatientModelServicePtr patientModelService = storageFixture.mPatientModelService;
	TestDataStructures testData;
	cx::ActiveDataPtr activeData = patientModelService->getActiveData();

	patientModelService->insertData(testData.image1);

	activeData->setActive(testData.image1->getUid());
	REQUIRE(activeData->getActive() == testData.image1);
}

TEST_CASE("StringPropertyActiveData works", "[unit][resource][core]")
{
	SessionStorageTestFixture storageFixture;
	cx::PatientModelServicePtr patientModelService = storageFixture.mPatientModelService;
	TestDataStructures testData;
	cx::ActiveDataPtr activeData = patientModelService->getActiveData();
	CHECK_FALSE(activeData->getActive());

	patientModelService->insertData(testData.image1);

	cx::StringPropertyActiveDataPtr activeDataProperty = cx::StringPropertyActiveData::New(patientModelService, "image");
	REQUIRE(activeDataProperty);

	CHECK_FALSE(activeData->getActive());
	CHECK(activeDataProperty->setValue(testData.image1->getUid()));
	CHECK(activeData->getActive());
	REQUIRE(activeDataProperty->getValue() == testData.image1->getUid());
}

TEST_CASE("Operating table set and get", "[unit][org.custusx.core.patientmodel]")
{
    SessionStorageTestFixture storageFixture;
    cx::PatientModelServicePtr patientModelService = storageFixture.mPatientModelService;

    cxtest::DirectSignalListener otChangedSignal(patientModelService.get(), SIGNAL(operatingTableChanged()));
    cx::OperatingTable ot(cx::createTransformIJC(cx::Vector3D(0,1,0),
                                             cx::Vector3D(0,0,1),
                                             cx::Vector3D(3,4,5)));
	CHECK(!cx::similar(ot.rMot, patientModelService->getOperatingTable().rMot));
    patientModelService->setOperatingTable(ot);
	CHECK(cx::similar(ot.rMot, patientModelService->getOperatingTable().rMot));

    CHECK(otChangedSignal.isReceived());
}

} //cxtest
