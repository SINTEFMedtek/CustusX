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
#include <QString>
#include "cxPatientStorage.h"
#include "cxSessionStorageServiceImpl.h"
#include "cxDataLocations.h"
#include "cxLogicManager.h"


namespace
{

struct TestVariable
{
	QString mValue;
	TestVariable() : mValue("Empty") {}
	QString get() {	return mValue;}
	void set(QString val) {	mValue = val;}
};
}

namespace cxtest
{

TEST_CASE("PatientStorage save/load works", "[unit][resource][core]")
{
	cx::DataLocations::setTestMode();
	cx::LogicManager::initialize();
	ctkPluginContext* context = cx::LogicManager::getInstance()->getPluginContext();

	cx::SessionStorageServicePtr sessionStorageService = cx::SessionStorageServicePtr (new cx::SessionStorageServiceImpl(context));

	TestVariable variableToTest;

    QString testString1("first");
    QString testString2("second");
    variableToTest.set(testString1);

    CHECK(variableToTest.get() == testString1);

    cx::PatientStorage storage(sessionStorageService, "TestNode");
    storage.storeVariable("testVariable", boost::bind(&TestVariable::get, &variableToTest), boost::bind(&TestVariable::set, &variableToTest, _1));

    QString session1("/temp/TestPatient1.cx3");
    QString session2("/temp/TestPatient2.cx3");

     //Create session1. No values are read.
    sessionStorageService->load(cx::DataLocations::getTestDataPath() + session1);

    variableToTest.set(testString2);
    //Create session2. No values are read.
    sessionStorageService->load(cx::DataLocations::getTestDataPath() + session2);

    //Load session1. Variables will be read.
    sessionStorageService->load(cx::DataLocations::getTestDataPath() + session1);
    CHECK(variableToTest.get() == testString1);

    //Load session2.
    sessionStorageService->load(cx::DataLocations::getTestDataPath() + session2);
    CHECK(variableToTest.get() == testString2);

    cx::LogicManager::shutdown();
}
}//cxtest
