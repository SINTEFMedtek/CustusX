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
#include "cxManageClippersWidget.h"
#include "cxClippers.h"
#include "cxVisServices.h"
#include "cxtestDirectSignalListener.h"
#include "cxTypeConversions.h"
#include "cxDataLocations.h"
#include "cxLogger.h"
#include "cxtestSessionStorageTestFixture.h"

namespace cxtest
{

class ClippersWidgetFixture : public cx::ManageClippersWidget
{
public:
	ClippersWidgetFixture() :
		ManageClippersWidget(cx::VisServices::getNullObjects(), NULL)
	{}
};

class ClippersFixture : public cx::Clippers
{
public:
	ClippersFixture() :
		Clippers(cx::VisServices::getNullObjects())
	{}
	QStringList getInitialClipperNames()
	{
		return cx::Clippers::getInitialClipperNames();
	}
};

TEST_CASE_METHOD(cxtest::ClippersWidgetFixture, "ClippersWidget: Init default clippers", "[unit][gui][widget]")
{
	this->mClippers->importList(QString());//Init with default clippers

	REQUIRE(this->mClippers->size() == 6);
//	REQUIRE_FALSE(this->mClipperSelector->getValue().isEmpty());//TODO; fix
}

TEST_CASE_METHOD(cxtest::ClippersFixture, "Clippers: Init default clippers", "[unit][gui][widget]")
{
	REQUIRE(this->getInitialClipperNames().size() == 6);
	REQUIRE(this->size() == 6);
}

TEST_CASE_METHOD(cxtest::ClippersWidgetFixture, "ClippersWidget: Select clipper", "[unit][gui][widget]")
{
	this->mClippers->importList(QString());//Init with default clippers

	ClippersFixture clippersFixture;
	QStringList defaultNames = clippersFixture.getInitialClipperNames();

	QString clipperName = defaultNames.at(3);
	REQUIRE_FALSE(clipperName.isEmpty());
	this->mClipperSelector->setValue(clipperName);

	clipperName = defaultNames.at(2);
	REQUIRE_FALSE(clipperName.isEmpty());

	cxtest::DirectSignalListener clippersChangedSignal(this->mClipperSelector.get(), SIGNAL(changed()));
	this->mClipperSelector->setValue(clipperName);
	CHECK(clippersChangedSignal.isReceived());

	REQUIRE(mCurrentClipper);
}

TEST_CASE_METHOD(cxtest::ClippersWidgetFixture, "ClippersWidget: Create new clipper", "[unit][gui][widget]")
{
	int initNumClippers = this->mClippers->size();
	CHECK(mClipperSelector->getValueRange().size() == initNumClippers);

	this->newClipperButtonClicked();

	int numClippers = this->mClippers->size();
	CHECK(numClippers == initNumClippers + 1);
	CHECK(mClipperSelector->getValueRange().size() == numClippers);
}

TEST_CASE_METHOD(cxtest::ClippersWidgetFixture, "ClippersWidget: Automatic naming of new clipper", "[unit][gui][widget]")
{
	QString clipperName = mClipperSelector->getValue();
	this->newClipperButtonClicked();

	QString clipperName2 = mClipperSelector->getValue();
	CHECK(clipperName2 != clipperName);
	CHECK(clipperName2.endsWith("2"));

	this->newClipperButtonClicked();

	QString clipperName3 = mClipperSelector->getValue();
	CHECK(clipperName3 != clipperName);
	CHECK(clipperName3 != clipperName2);
	CHECK(clipperName3.endsWith("3"));
}

TEST_CASE_METHOD(cxtest::ClippersWidgetFixture, "ClippersWidget: Load clipper names", "[unit][gui][widget]")
{
	QStringList range = this->mClipperSelector->getValueRange();
	REQUIRE(range.size() == 6);
	QString clipperName = range.first();
	INFO(QString("clipperName: %1").arg(clipperName));
	CHECK(clipperName.length() > 1);
}

TEST_CASE_METHOD(cxtest::ClippersWidgetFixture, "ClippersWidget: New patient gets default clippers", "[unit][gui][widget]")
{
	cxtest::SessionStorageTestFixture storageFixture;
	storageFixture.createSessions();
	storageFixture.loadSession1();

	QStringList range = this->mClipperSelector->getValueRange();
	CHECK(range.size() == 6);
}

//TEST_CASE_METHOD(cxtest::ClippersWidgetFixture, "ClippersWidget: Clipper is set on new patient", "[unit][gui][widget]")
//{
//	cxtest::SessionStorageTestFixture storageFixture;
//	storageFixture.createSessions();
//	storageFixture.loadSession1();

//	REQUIRE_FALSE(this->mClipperSelector->getValue().isEmpty());
//}

//TEST_CASE_METHOD(cxtest::ClippersWidgetFixture, "ClippersWidget: New clipper name is saved/loaded", "[unit][gui][widget]")
//{
//	cxtest::SessionStorageTestFixture storageFixture;
//	storageFixture.createSessions();
//	storageFixture.loadSession1();

//	QString oldClipperName = mClipperSelector->getValue();
//	REQUIRE_FALSE(oldClipperName.isEmpty());

//	this->newClipperButtonClicked();

//	QStringList range = this->mClipperSelector->getValueRange();
//	CHECK(range.size() == 7);

//	QString newClipperName = mClipperSelector->getValue();
//	CHECK(newClipperName != oldClipperName);

//	storageFixture.reloadSession1();
//	range = this->mClipperSelector->getValueRange();
//	CHECK(range.size() == 7);
//	CHECK(newClipperName == mClipperSelector->getValue());
//}

}//cxtest
