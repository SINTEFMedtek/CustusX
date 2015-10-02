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

#include "cxFastPatientRegistrationWidget.h"
#include "cxRegServices.h"
#include "cxLogicManager.h"
#include "cxtestFastPatientRegistrationWidgetFixture.h"
#include "cxViewServiceNull.h"

namespace
{
typedef boost::shared_ptr<class VisualizationServiceFixture> VisualizationServiceFixturePtr;
class VisualizationServiceFixture : public cx::VisualizationServiceNull
{
public:
	void emitPointSampled()
	{
		emit pointSampled(cx::Vector3D(1, 1, 1));
	}
};


class FastPatientRegistrationWidgetHelper
{
public:
	FastPatientRegistrationWidgetHelper()
	{
		cx::LogicManager::initialize();
		cx::RegServicesPtr services = cx::RegServices::create(cx::logicManager()->getPluginContext());

		viewFixture = VisualizationServiceFixturePtr(new VisualizationServiceFixture());
		services->visualizationService = viewFixture;

		widgetFixture = cxtest::FastPatientRegistrationWidgetFixturePtr(new cxtest::FastPatientRegistrationWidgetFixture(*services, NULL));
	}
	~FastPatientRegistrationWidgetHelper()
	{
		cx::LogicManager::shutdown();
	}

	cxtest::FastPatientRegistrationWidgetFixturePtr widgetFixture;
	VisualizationServiceFixturePtr viewFixture;
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

	helper.viewFixture->emitPointSampled();
	CHECK_FALSE(helper.widgetFixture->mPointSampled);

	helper.widgetFixture->triggerShowEvent();
	helper.viewFixture->emitPointSampled();
	CHECK(helper.widgetFixture->mPointSampled);

	helper.widgetFixture->mPointSampled = false;
	helper.widgetFixture->triggerHideEvent();
	helper.viewFixture->emitPointSampled();
	CHECK_FALSE(helper.widgetFixture->mPointSampled);
}

}//cxtest