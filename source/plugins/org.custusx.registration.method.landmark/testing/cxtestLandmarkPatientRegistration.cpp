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
