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
#include "cxLogicManager.h"
#include "cxViewService.h"

namespace cxtest
{

typedef boost::shared_ptr<class ManageClippersWidgetFixture> ManageClippersWidgetFixturePtr;
class ManageClippersWidgetFixture : public cx::ManageClippersWidget
{
public:
	ManageClippersWidgetFixture() :
		ManageClippersWidget(cx::VisServices::getNullObjects(), NULL)
	{}
	ManageClippersWidgetFixture(cx::VisServicesPtr services) :
		ManageClippersWidget(services, NULL)
	{}
	cx::StringPropertyPtr getClipperSelector()
	{
		return mClipperSelector;
	}
	cx::InteractiveClipperPtr getCurrentClipper()
	{
		return mCurrentClipper;
	}
	void newClipperButtonClicked()
	{
		cx::ManageClippersWidget::newClipperButtonClicked();
	}
};

class ManageClippersWidgetTestHelper
{
public:
	ManageClippersWidgetTestHelper()
	{
		cx::LogicManager::initialize();
		services = cx::VisServices::create(cx::logicManager()->getPluginContext());
		fixture = ManageClippersWidgetFixturePtr(new ManageClippersWidgetFixture(services));
	}
	~ManageClippersWidgetTestHelper()
	{
		cx::LogicManager::shutdown();
	}

	ManageClippersWidgetFixturePtr fixture;
	cx::VisServicesPtr services;
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

TEST_CASE("VisServices: Init default clippers", "[unit][gui][widget]")
{
	ManageClippersWidgetTestHelper helper;

	cx::ClippersPtr clippers = helper.services->view()->getClippers();
	REQUIRE(clippers);
	clippers->importList(QString());//Init with default clippers

	REQUIRE(clippers->size() == 6);
}

TEST_CASE_METHOD(cxtest::ClippersFixture, "Clippers: Init default clippers", "[unit][gui][widget]")
{
	REQUIRE(this->getInitialClipperNames().size() == 6);
	REQUIRE(this->size() == 6);
}

TEST_CASE("ClippersWidget: Select clipper", "[unit][gui][widget]")
{
	ManageClippersWidgetTestHelper helper;
	helper.services->view()->getClippers()->importList(QString());//Init with default clippers

	ClippersFixture clippersFixture;
	QStringList defaultNames = clippersFixture.getInitialClipperNames();

	QString clipperName = defaultNames.at(3);
	REQUIRE_FALSE(clipperName.isEmpty());
	helper.fixture->getClipperSelector()->setValue(clipperName);

	clipperName = defaultNames.at(2);
	REQUIRE_FALSE(clipperName.isEmpty());

	cxtest::DirectSignalListener clippersChangedSignal(helper.fixture->getClipperSelector().get(), SIGNAL(changed()));
	helper.fixture->getClipperSelector()->setValue(clipperName);
	CHECK(clippersChangedSignal.isReceived());

	REQUIRE(helper.fixture->getCurrentClipper());
}

TEST_CASE("ClippersWidget: Create new clipper", "[unit][gui][widget]")
{
	ManageClippersWidgetTestHelper helper;
	ManageClippersWidgetFixturePtr fixture = helper.fixture;

	int initNumClippers = helper.services->view()->getClippers()->size();
	CHECK(fixture->getClipperSelector()->getValueRange().size() == initNumClippers);

	fixture->newClipperButtonClicked();

	int numClippers = helper.services->view()->getClippers()->size();
	CHECK(numClippers == initNumClippers + 1);
	CHECK(fixture->getClipperSelector()->getValueRange().size() == numClippers);
}

TEST_CASE("ClippersWidget: Automatic naming of new clipper", "[unit][gui][widget]")
{
	ManageClippersWidgetTestHelper helper;
	ManageClippersWidgetFixturePtr fixture = helper.fixture;

	QString clipperName = fixture->getClipperSelector()->getValue();
	fixture->newClipperButtonClicked();

	QString clipperName2 = fixture->getClipperSelector()->getValue();
	CHECK(clipperName2 != clipperName);
	CHECK(clipperName2.endsWith("2"));

	fixture->newClipperButtonClicked();

	QString clipperName3 = fixture->getClipperSelector()->getValue();
	CHECK(clipperName3 != clipperName);
	CHECK(clipperName3 != clipperName2);
	CHECK(clipperName3.endsWith("3"));
}

TEST_CASE("ClippersWidget: Load clipper names", "[unit][gui][widget]")
{
	ManageClippersWidgetTestHelper helper;

	QStringList range = helper.fixture->getClipperSelector()->getValueRange();
	REQUIRE(range.size() == 6);
	QString clipperName = range.first();
	INFO(QString("clipperName: %1").arg(clipperName));
	CHECK(clipperName.length() > 1);
}

TEST_CASE("ClippersWidget: New patient gets default clippers", "[unit][gui][widget]")
{
	cxtest::SessionStorageTestFixture storageFixture;
	ManageClippersWidgetFixturePtr fixture = ManageClippersWidgetFixturePtr(new ManageClippersWidgetFixture(storageFixture.mServices));
	storageFixture.createSessions();
	storageFixture.loadSession1();

	QStringList range = fixture->getClipperSelector()->getValueRange();
	CHECK(range.size() == 6);
}

//TEST_CASE_METHOD(cxtest::ManageClippersWidgetFixture, "ClippersWidget: Clipper is set on new patient", "[unit][gui][widget]")
//{
//	cxtest::SessionStorageTestFixture storageFixture;
//	storageFixture.createSessions();
//	storageFixture.loadSession1();

//	REQUIRE_FALSE(this->mClipperSelector->getValue().isEmpty());
//}

//TEST_CASE_METHOD(cxtest::ManageClippersWidgetFixture, "ClippersWidget: New clipper name is saved/loaded", "[unit][gui][widget]")
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
