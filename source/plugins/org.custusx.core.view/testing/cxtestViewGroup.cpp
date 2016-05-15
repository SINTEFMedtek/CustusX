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
typedef boost::shared_ptr<class ViewGroupFixture> ViewGroupFixturePtr;
class ViewGroupFixture : public cx::ViewGroup
{
public:
	ViewGroupFixture(cx::CoreServicesPtr backend) : ViewGroup(backend)
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

}//namespace cxtest
