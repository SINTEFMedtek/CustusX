/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include <QString>
#include "cxPatientStorage.h"
#include "cxtestSessionStorageTestFixture.h"

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
	cxtest::SessionStorageTestFixture storageFixture;

	TestVariable variableToTest;
	CHECK(variableToTest.get() == "Empty");

    QString testString1("first");
    QString testString2("second");
    variableToTest.set(testString1);

	storageFixture.createSessions();

	CHECK(variableToTest.get() == testString1);

	cx::PatientStorage storage(storageFixture.mSessionStorageService, "TestNode");
	storage.storeVariable("testVariable", boost::bind(&TestVariable::get, &variableToTest), boost::bind(&TestVariable::set, &variableToTest, _1));

	storageFixture.loadSession1();
	CHECK(variableToTest.get() == testString1);
	storageFixture.saveSession();

	storageFixture.loadSession2();
	CHECK(variableToTest.get() == testString1);
	variableToTest.set(testString2);
	storageFixture.saveSession();

    CHECK(variableToTest.get() == testString2);
	CHECK_FALSE(variableToTest.get() == testString1);

	storageFixture.loadSession1();
	CHECK(variableToTest.get() == testString1);
}
}//cxtest
