/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
	cx::settings()->setValue("Automation/autoShowNewDataInViewGroup1", false);
	cx::settings()->setValue("Automation/autoShowNewDataInViewGroup2", false);
	cx::settings()->setValue("Automation/autoShowNewDataInViewGroup3", false);
	cx::settings()->setValue("Automation/autoShowNewDataInViewGroup4", true);

	QList<unsigned> showInViewGroups = viewservice->getAutoShowViewGroupNumbers();
	REQUIRE(showInViewGroups.size() == 2);
	CHECK(showInViewGroups[0] == 0);
	CHECK(showInViewGroups[1] == 4);

	cx::LogicManager::shutdown();
}
