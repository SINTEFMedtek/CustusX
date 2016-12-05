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

#include <QWidget>
#include "cxVisServices.h"
#include "cxLogicManager.h"
#include "cxDataLocations.h"
#include "cxtestViewServiceMock.h"
#include "cxtestViewCollectionWidgetMixedMock.h"
#include "cxtestLayoutWidgetUsingViewWidgetsMock.h"
#include "cxSettings.h"

void checkContextMenuPolicy(Qt::ContextMenuPolicy policy, cxtest::ViewServiceMockPtr viewservice)
{
	int numberOfViews = viewservice->getViewCollectionWidgets().size();
	int widgetschecked = 0;
	for(int i=0; i<numberOfViews; ++i)
	{
		QWidget* widget = viewservice->getLayoutWidget(i);
		REQUIRE(widget);
		cxtest::ViewCollectionWidgetMixedMock* vcwmm = dynamic_cast<cxtest::ViewCollectionWidgetMixedMock*>(widget);
		if(vcwmm)
		{
			REQUIRE(vcwmm->getViewCollectionWidget()->contextMenuPolicy() == policy);
			widgetschecked=+1;
		}
		cxtest::LayoutWidgetUsingViewWidgetsMock* lwuvwm = dynamic_cast<cxtest::LayoutWidgetUsingViewWidgetsMock*>(widget);
		if(lwuvwm)
		{
			std::vector<cx::ViewWidget*> widgets = lwuvwm->getViewWidgets();
			for(int j=0; j<widgets.size(); ++j)
			{
				REQUIRE(widgets[j]->contextMenuPolicy() == policy);
				widgetschecked=+1;
			}
		}
	}
	REQUIRE(widgetschecked > 0);
}

TEST_CASE("Can turn custom context menu off", "[viewservice][unit]")
{
	cx::DataLocations::setTestMode();
	cx::LogicManager::initialize();

	ctkPluginContext* context = cx::LogicManager::getInstance()->getPluginContext();

	cxtest::ViewServiceMockPtr viewservice;
	viewservice.reset(new cxtest::ViewServiceMock(context));
	viewservice->createLayoutWidget(NULL, 0);
	viewservice->createLayoutWidget(NULL, 1);

	checkContextMenuPolicy(Qt::CustomContextMenu, viewservice);

	viewservice->enableContextMenuForViews(false);
	checkContextMenuPolicy(Qt::PreventContextMenu, viewservice);

	viewservice->enableContextMenuForViews(true);
	checkContextMenuPolicy(Qt::CustomContextMenu, viewservice);

	cx::LogicManager::shutdown();
}

TEST_CASE("ViewService: Auto show in view groups", "[unit][plugins][org.custusx.core.view]")
{
	cx::DataLocations::setTestMode();
	cx::LogicManager::initialize();

	ctkPluginContext* context = cx::LogicManager::getInstance()->getPluginContext();
	cxtest::ViewServiceMockPtr viewservice = cxtest::ViewServiceMockPtr(new cxtest::ViewServiceMock(context));

	cx::settings()->setValue("Automation/autoShowNewDataInViewGroup0", true);
	cx::settings()->setValue("Automation/autoShowNewDataInViewGroup4", true);

//    cxtest::TestVisServicesPtr dummyservices = cxtest::TestVisServices::create();
//    ViewManagerFixturePtr viewManager = ViewManagerFixturePtr(new ViewManagerFixture(dummyservices));
	QList<unsigned> showInViewGroups = viewservice->getAutoShowViewGroupNumbers();
	REQUIRE(showInViewGroups.size() == 2);
	CHECK(showInViewGroups[0] == 0);
	CHECK(showInViewGroups[1] == 4);

	cx::LogicManager::shutdown();
}