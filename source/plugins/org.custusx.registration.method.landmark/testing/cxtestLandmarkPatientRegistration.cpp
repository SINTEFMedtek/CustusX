/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxtestSessionStorageTestFixture.h"
#include "cxPatientModelService.h"
#include "cxVector3D.h"
#include "cxLandmark.h"
#include "cxRegServices.h"
#include "cxtestDirectSignalListener.h"
#include "cxRegistrationService.h"
#include "cxSessionStorageService.h"


namespace cxtest
{

TEST_CASE("RegistrationPlugin: Patient landmarks visible after registering, save, quit and load patient", "[unit][plugins][org.custusx.registration]")
{
    SessionStorageTestFixture storageFixture;
    cx::RegServicesPtr regServices = cx::RegServices::create(storageFixture.mContext);
    cx::PatientModelServicePtr patientModelService = regServices->patient();
    TestDataStructures testData;

    storageFixture.createSessions();
    storageFixture.loadSession1();
    patientModelService->insertData(testData.image1);

    cx::Vector3D p_target(0,0,0);
    REQUIRE(patientModelService->getPatientLandmarks()->getLandmarks().size()==0);
    regServices->registration()->setFixedData(testData.image1);
    patientModelService->getPatientLandmarks()->setLandmark(cx::Landmark(testData.image1->getUid(), p_target));
    REQUIRE(patientModelService->getPatientLandmarks()->getLandmarks().size()==1);
    storageFixture.saveSession();

    storageFixture.loadSession2();
    REQUIRE(patientModelService->getPatientLandmarks()->getLandmarks().size()==0);
    storageFixture.saveSession();

    cxtest::DirectSignalListener fixedDataChangedSignal(regServices->registration().get(), SIGNAL(fixedDataChanged(QString)));
    cxtest::DirectSignalListener isLoadingSecondSignal(regServices->session().get(), SIGNAL(isLoadingSecond(QDomElement&)));
    storageFixture.loadSession1();
    CHECK(fixedDataChangedSignal.isReceived());
    CHECK(isLoadingSecondSignal.isReceived());
    REQUIRE(patientModelService->getPatientLandmarks()->getLandmarks().size()==1);
}

}//cxtest
