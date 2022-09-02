/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include <QCheckBox>

#include "cxFastPatientRegistrationWidget.h"
#include "cxRegServices.h"
#include "cxLogicManager.h"
#include "cxtestFastPatientRegistrationWidgetFixture.h"
#include "cxViewServiceNull.h"

namespace
{
typedef boost::shared_ptr<class ViewServiceMock> ViewServiceFixturePtr;
typedef boost::shared_ptr<class TestRegServices> TestRegServicesPtr;
class ViewServiceMock : public cx::ViewServiceNull
{
public:
	void emitPointSampled()
	{
		emit pointSampled(cx::Vector3D(1, 1, 1));
	}
};

class TestRegServices : public cx::RegServices
{
public:
	static TestRegServicesPtr create(ctkPluginContext* context)
	{
		return TestRegServicesPtr(new TestRegServices(context));
	}
	ViewServiceFixturePtr viewFixture;
protected:
	TestRegServices(ctkPluginContext* context) :
		RegServices(context)
	{
		viewFixture = ViewServiceFixturePtr(new ViewServiceMock());
		this->mViewService = viewFixture;
	}
};


class FastPatientRegistrationWidgetHelper
{
public:
	FastPatientRegistrationWidgetHelper()
	{
		cx::LogicManager::initialize();
		mServices = TestRegServices::create(cx::logicManager()->getPluginContext());
		widgetFixture = cxtest::FastPatientRegistrationWidgetFixturePtr(new cxtest::FastPatientRegistrationWidgetFixture(mServices, NULL));
	}
	~FastPatientRegistrationWidgetHelper()
	{
		cx::LogicManager::shutdown();
	}

	ViewServiceFixturePtr view()
	{
		return mServices->viewFixture;
	}

	cxtest::FastPatientRegistrationWidgetFixturePtr widgetFixture;
protected:
	TestRegServicesPtr mServices;
};

}//namespace

namespace cxtest
{

TEST_CASE("FastPatientRegistrationWidget: Sample with mouse click is turned on/off when widget is shown/hidden",
		  "[unit][plugins][registration][hide]")
{
	FastPatientRegistrationWidgetHelper helper;

	CHECK_FALSE(helper.widgetFixture->getMouseClickSample()->isChecked());

	helper.widgetFixture->triggerShowEvent();
	CHECK(helper.widgetFixture->getMouseClickSample()->isChecked());

	helper.widgetFixture->triggerHideEvent();
	CHECK_FALSE(helper.widgetFixture->getMouseClickSample()->isChecked());
}

TEST_CASE("FastPatientRegistrationWidget: Receive pointSampled signals when sample with mouse click is on",
		  "[unit][plugins][registration][hide]")
{
	FastPatientRegistrationWidgetHelper helper;

	helper.view()->emitPointSampled();
	CHECK_FALSE(helper.widgetFixture->mPointSampled);

	helper.widgetFixture->triggerShowEvent();
	helper.view()->emitPointSampled();
	CHECK(helper.widgetFixture->mPointSampled);

	helper.widgetFixture->mPointSampled = false;
	helper.widgetFixture->triggerHideEvent();
	helper.view()->emitPointSampled();
	CHECK_FALSE(helper.widgetFixture->mPointSampled);
}


TEST_CASE("FastPatientRegistrationWidget: Test getting next landmark when landmark list is empty", "[unit][plugins][org.custusx.registration]")
{
	FastPatientRegistrationWidgetHelper helper;
	QString landmark = helper.widgetFixture->getNextLandmark();
	REQUIRE(landmark.isEmpty());
}

}//cxtest
