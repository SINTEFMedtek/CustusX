/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include <memory>
#include "cxtestVisServices.h"
#include "cxViewGroup.h"
#include "cxViewGroupData.h"
#include "cxDataLocations.h"
#include "cxLogicManager.h"
#include "cxPatientModelService.h"

#include "cxActiveData.h"
#include "cxtestTestDataStructures.h"

namespace cxtest
{
typedef std::shared_ptr<class ViewGroupFixture> ViewGroupFixturePtr;
class ViewGroupFixture : public cx::ViewGroup
{
public:
	ViewGroupFixture(cx::CoreServicesPtr backend) : ViewGroup(backend, "0")
	{
		mBackend->patient()->insertData(mTestData.image1);
		mBackend->patient()->insertData(mTestData.image2);
	}
	void testMouseClick()
	{
		this->mouseClickInViewGroupSlot();
	}
	void addData(cx::DataPtr data)
	{
		if(!data)
			return;
		this->getData()->addDataSorted(data->getUid());
	}

	void addTestData()
	{
		this->addData(mTestData.image1);
		this->addData(mTestData.image2);
	}

	std::vector<cx::ImagePtr> getImages()
	{
		return this->getData()->getImages(cx::DataViewProperties::createFull());
	}

	TestDataStructures mTestData;
};

TEST_CASE("ViewGroup: get ActiveData", "[unit]")
{
	cx::DataLocations::setTestMode();
	cx::LogicManager::initialize();
	cx::VisServicesPtr services = cx::VisServices::create(cx::logicManager()->getPluginContext());
	cx::ActiveDataPtr activeData = services->patient()->getActiveData();

    ViewGroupFixturePtr viewGroup(new ViewGroupFixture(services));
    REQUIRE(viewGroup);

    cx::ImagePtr testImage1 = viewGroup->mTestData.image1;
    viewGroup->addData(testImage1);

    std::vector<cx::ImagePtr> images = viewGroup->getImages();
    REQUIRE(images.size() > 0);
    REQUIRE(std::find(images.begin(), images.end(), testImage1) != images.end());

    viewGroup->testMouseClick();

    cx::ImagePtr activeImage = activeData->getActive<cx::Image>();
    REQUIRE(activeImage == testImage1);

    cx::LogicManager::shutdown();
}


TEST_CASE("ViewGroup: ActiveImage change if ActiveImage is not in view group", "[unit]")
{
	cx::DataLocations::setTestMode();
	cx::LogicManager::initialize();
	cx::VisServicesPtr services = cx::VisServices::create(cx::logicManager()->getPluginContext());
	cx::ActiveDataPtr activeData = services->patient()->getActiveData();
	ViewGroupFixturePtr viewGroup(new ViewGroupFixture(services));
	viewGroup->addData(viewGroup->mTestData.image1);

    activeData->setActive(viewGroup->mTestData.image2);
    cx::ImagePtr activeImage = activeData->getActive<cx::Image>();
    REQUIRE(activeImage == viewGroup->mTestData.image2);

    viewGroup->testMouseClick();
    activeImage = activeData->getActive<cx::Image>();
    REQUIRE(activeImage == viewGroup->mTestData.image1);

    cx::LogicManager::shutdown();
}

TEST_CASE("ViewGroup: ActiveData don't change if ActiveImage is in view group", "[unit]")
{
	cx::DataLocations::setTestMode();
	cx::LogicManager::initialize();
	cx::VisServicesPtr services = cx::VisServices::create(cx::logicManager()->getPluginContext());
	cx::ActiveDataPtr activeData = services->patient()->getActiveData();
	ViewGroupFixturePtr viewGroup(new ViewGroupFixture(services));
	viewGroup->addTestData();

    std::vector<cx::ImagePtr>  images = viewGroup->getImages();
    REQUIRE(images.size() > 1);

    activeData->setActive(viewGroup->mTestData.image2);
    cx::ImagePtr activeImage = activeData->getActive<cx::Image>();
    REQUIRE(activeImage == viewGroup->mTestData.image2);

    viewGroup->testMouseClick();
    activeImage = activeData->getActive<cx::Image>();
    REQUIRE(activeImage == viewGroup->mTestData.image2);

    cx::LogicManager::shutdown();
}

TEST_CASE("CameraStyleData: Style is kept when the view angle changes", "[unit]")
{
	cx::CameraStyleData style(cx::cstTOOL_STYLE);
	style.mCameraViewAngle = 90.0/180.0*M_PI;
	CHECK(style.getStyle() == cx::cstTOOL_STYLE);
	CHECK_FALSE(style == cx::CameraStyleData(cx::cstTOOL_STYLE));
}

TEST_CASE("ViewGroupData: Scope exposure defaults to 1 and signals changes", "[unit]")
{
	cx::ViewGroupData viewGroupData(cx::CoreServicesPtr(), "0");
	CHECK(viewGroupData.getScopeExposure() == Approx(1.0));

	int changes = 0;
	QObject::connect(&viewGroupData, &cx::ViewGroupData::scopeExposureChanged, [&changes](){ ++changes; });
	viewGroupData.setScopeExposure(1.4);
	viewGroupData.setScopeExposure(1.4);
	CHECK(viewGroupData.getScopeExposure() == Approx(1.4));
	CHECK(changes == 1);
}

}//namespace cxtest
