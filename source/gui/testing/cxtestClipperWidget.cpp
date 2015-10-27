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
#include "cxClipperWidget.h"
#include "cxVisServices.h"
#include "cxClippers.h"
#include "cxInteractiveClipper.h"
#include "cxtestSessionStorageTestFixture.h"
#include "cxPatientModelService.h"

namespace cxtest
{

typedef boost::shared_ptr<class VisServicesFixture> VisServicesFixturePtr;
class VisServicesFixture : public cx::VisServices
{
public:
	void setPatient(cx::PatientModelServicePtr patient)
	{
		mPatientModelService = patient;
	}
};

class ClipperWidgetFixture : public cx::ClipperWidget
{
public:
	ClipperWidgetFixture() :
		ClipperWidget(cxtest::VisServicesFixture::getNullObjects(), NULL)
	{
		cx::ClippersPtr clippers = cx::ClippersPtr(new cx::Clippers(mServices));
		QString clipperName = clippers->getClipperNames().first();
		testClipper = clippers->getClipper(clipperName);

		clipperName = clippers->getClipperNames().last();
		testClipper2 = clippers->getClipper(clipperName);
	}

	void setPatientService(cx::PatientModelServicePtr patient)
	{
		cxtest::VisServicesFixturePtr services = boost::static_pointer_cast<cxtest::VisServicesFixture>(mServices);
		services->setPatient(patient);
	}

	void createTestPatient()
	{
		cxtest::SessionStorageTestFixture storageFixture;
		cx::PatientModelServicePtr patientService = storageFixture.mPatientModelService;
		storageFixture.createSessions();
		storageFixture.loadSession1();

		this->setPatientService(patientService);//Use real patient service instead of null object
	}

	cx::InteractiveClipperPtr testClipper;
	cx::InteractiveClipperPtr testClipper2;
};


TEST_CASE_METHOD(cxtest::ClipperWidgetFixture, "ClipperWidget: Set clipper", "[unit][gui][widget]")
{
	CHECK_FALSE(mClipper);
	this->setClipper(testClipper);
	CHECK(mClipper);
}

TEST_CASE_METHOD(cxtest::ClipperWidgetFixture, "ClipperWidget: Enable clipper", "[unit][gui][widget]")
{
	this->setClipper(testClipper);
	this->mUseClipperCheckBox->setChecked(true);
	CHECK(mUseClipperCheckBox->isChecked());
	CHECK(testClipper->getUseClipper());

	this->mUseClipperCheckBox->setChecked(false);
	CHECK_FALSE(testClipper->getUseClipper());

	this->setClipper(testClipper2);
	this->mUseClipperCheckBox->setChecked(true);
	CHECK(testClipper2->getUseClipper());

	this->setClipper(testClipper);
	CHECK_FALSE(testClipper->getUseClipper());
	CHECK_FALSE(mUseClipperCheckBox->isChecked());
}

TEST_CASE_METHOD(cxtest::ClipperWidgetFixture, "ClipperWidget: Insert data", "[unit][gui][widget]")
{
	cxtest::SessionStorageTestFixture storageFixture;
	cx::PatientModelServicePtr patientService = storageFixture.mPatientModelService;
	storageFixture.createSessions();
	storageFixture.loadSession1();

	this->setPatientService(patientService);//Use real patient service instead of null object

	CHECK(this->getDatas().size() == 0);
	CHECK(this->mCheckBoxes.size() == 0);

	TestDataStructures testData;
	patientService->insertData(testData.mesh1);
	patientService->insertData(testData.image1);

	CHECK(this->getDatas().size() == 2);

	this->setClipper(testClipper);
	CHECK(this->mCheckBoxes.size() == 2);

	patientService->insertData(testData.image2);
	this->prePaintEvent();//Force update of data structures
	CHECK(this->getDatas().size() == 3);
	CHECK(this->mCheckBoxes.size() == 3);
}

TEST_CASE_METHOD(cxtest::ClipperWidgetFixture, "ClipperWidget: Turn clipping on/off for a mesh", "[unit][gui][widget]")
{
	cxtest::SessionStorageTestFixture storageFixture;
	cx::PatientModelServicePtr patientService = storageFixture.mPatientModelService;
	storageFixture.createSessions();
	storageFixture.loadSession1();

	this->setPatientService(patientService);//Use real patient service instead of null object

	TestDataStructures testData;
	patientService->insertData(testData.mesh1);
	patientService->insertData(testData.image1);
	patientService->insertData(testData.image2);

	this->setClipper(testClipper);

	this->mUseClipperCheckBox->setChecked(true);

	QString uid = testData.mesh1->getUid();

	CHECK(this->mCheckBoxes.size() == 3);
	REQUIRE(this->mCheckBoxes.contains(uid));
	QCheckBox *checkBox = this->mCheckBoxes[uid];
	REQUIRE(checkBox);
	REQUIRE_FALSE(checkBox->isChecked());


	std::vector<vtkPlanePtr> clipPlanes = testData.mesh1->getAllClipPlanes();
	REQUIRE(clipPlanes.size() == 0);

	CHECK(testClipper->getDatas().size() == 0);

	checkBox->click();
	clipPlanes = testData.mesh1->getAllClipPlanes();
	CHECK(testClipper->getDatas().size() == 1);
	REQUIRE(clipPlanes.size() == 1);


	checkBox->click();
	REQUIRE_FALSE(checkBox->isChecked());
	clipPlanes = testData.mesh1->getAllClipPlanes();
	CHECK(testClipper->getDatas().size() == 0);
	REQUIRE(clipPlanes.size() == 0);

}

}//cxtest