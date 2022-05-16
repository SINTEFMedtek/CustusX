/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include <QActionGroup>
#include <QTimer>
#include "cxVisServices.h"
#include "cxViewService.h"
#include "cxtestDirectSignalListener.h"
#include "cxtestVisualizationHelper.h"
#include "cxViewServiceNull.h"
#include "cxLogicManager.h"
#include "cxStateService.h"
#include "cxMainWindowApplicationComponent.h"
#include "cxMainWindow.h"
#include "cxDataLocations.h"
#include "cxLayoutData.h"

#include "cxToolImpl.h"
#include "cxTrackingService.h"
#include "cxDummyTool.h"
#include "cxViewGroup.h"
#include "cxDummyToolManager.h"

namespace
{
typedef boost::shared_ptr<class ViewServiceFixture> ViewServiceFixturePtr;
class ViewServiceFixture
{
public:
	ViewServiceFixture()
	{
		cx::DataLocations::setTestMode();
		cx::ApplicationComponentPtr mainwindow(new cx::MainWindowApplicationComponent<cx::MainWindow>());
		cx::LogicManager::initialize(mainwindow);
		mServices = cx::VisServices::create(cx::logicManager()->getPluginContext());
	}
	~ViewServiceFixture()
	{
		cx::LogicManager::shutdown();
	}

	void quickRun()
	{
		QTimer::singleShot(100, qApp, SLOT(quit()));
		qApp->exec();
	}

	cx::VisServicesPtr mServices;
};
}// namespace

TEST_CASE("ViewWrapper2D: Emits pointSampled signal when anyplane", "[integration][plugins][org.custusx.core.view]")
{
	cxtest::VisualizationHelper visHelper;

	cxtest::DirectSignalListener signalListener(visHelper.viewWrapper.get(), SIGNAL(pointSampled(Vector3D)));
	visHelper.viewWrapper->emitPointSampled();
	CHECK_FALSE(signalListener.isReceived());

	visHelper.viewWrapper->initializePlane(cx::ptANYPLANE);

	visHelper.viewWrapper->emitPointSampled();
	CHECK(signalListener.isReceived());
}

TEST_CASE("ViewService: Get active view group", "[integration][plugins][org.custusx.core.view]")
{
	ViewServiceFixture fixture;
	fixture.quickRun();

	CHECK(fixture.mServices->view()->getActiveViewGroup());
	CHECK(fixture.mServices->view()->getActiveGroupId() >= 0);
}

TEST_CASE("ViewService: Get camera style interactor action group and actions", "[integration][plugins][org.custusx.core.view]")
{
	ViewServiceFixture fixture;
	fixture.quickRun();

	QActionGroup* actionGroup = fixture.mServices->view()->getInteractorStyleActionGroup();
	REQUIRE(actionGroup);

	QList<QAction*> actions = actionGroup->actions();
	REQUIRE(actions.size() >= 4);
}

TEST_CASE("ViewService: Setting camera style also marks action as selected in camera style action group", "[integration][plugins][org.custusx.core.view]")
{
	ViewServiceFixture fixture;
	fixture.quickRun();

	QActionGroup* actionGroup = fixture.mServices->view()->getInteractorStyleActionGroup();
	REQUIRE(actionGroup);

	fixture.mServices->view()->setCameraStyle(cx::cstDEFAULT_STYLE, 0);

	QAction* selectedAction = actionGroup->checkedAction();
	CHECK(selectedAction);

	fixture.mServices->view()->setCameraStyle(cx::cstTOOL_STYLE, 0);

	QAction* selectedAction2 = actionGroup->checkedAction();
	CHECK(selectedAction != selectedAction2);
}

TEST_CASE("ViewService: set/get camera style", "[integration][plugins][org.custusx.core.view]")
{
	ViewServiceFixture fixture;
	fixture.quickRun();

	QActionGroup* actionGroup = fixture.mServices->view()->getInteractorStyleActionGroup();
	REQUIRE(actionGroup);

	QAction* selectedAction = actionGroup->checkedAction();
	REQUIRE(selectedAction);
	REQUIRE(selectedAction->data().toString() == enum2string(cx::cstDEFAULT_STYLE));

	fixture.mServices->view()->setCameraStyle(cx::cstTOOL_STYLE, 0);
	selectedAction = actionGroup->checkedAction();
	REQUIRE(selectedAction->data().toString() == enum2string(cx::cstTOOL_STYLE));
}

TEST_CASE("ViewService: Add new default layout", "[integration][plugins][org.custusx.core.view]")
{
	ViewServiceFixture fixture;

	//Test layout
	QString testLayoutName("TEST_LAYOUT");
	cx::LayoutData layout = cx::LayoutData::create(testLayoutName, "TESTLAYOUT", 1, 1);
	layout.setView(1, cx::ptAXIAL, cx::LayoutRegion(0, 0));

	fixture.mServices->view()->setActiveLayout(testLayoutName);
	CHECK_FALSE(fixture.mServices->view()->getActiveLayout() == testLayoutName);

	fixture.mServices->view()->addDefaultLayout(layout);
	fixture.mServices->view()->setActiveLayout(testLayoutName);
	REQUIRE(fixture.mServices->view()->getActiveLayout() == testLayoutName);
}

TEST_CASE("ViewWrapper: Controlling tool equals active tool", "[integration][plugins][org.custusx.core.view]")
{
	cxtest::VisualizationHelper visHelper;
	cx::ToolPtr controllingTool = visHelper.viewWrapper->getControllingTool();
	REQUIRE(controllingTool);
	
	cx::ToolPtr activeTool = visHelper.services->tracking()->getActiveTool();
	CHECK(controllingTool == activeTool);
}

TEST_CASE("ViewWrapper: Changing active tool also changes controlling tool", "[integration][plugins][org.custusx.core.view]")
{
	cxtest::VisualizationHelper visHelper;
	
	cx::DummyToolPtr tool2(new cx::DummyTool("dummytool2"));
	cx::DummyToolManager::DummyToolManagerPtr toolManager = boost::dynamic_pointer_cast<cx::DummyToolManager>(visHelper.services->tracking());
	REQUIRE(toolManager);
	toolManager->addTool(tool2);
	
	visHelper.services->tracking()->setActiveTool(tool2->getUid());
	CHECK(visHelper.viewWrapper->getControllingTool()->getUid() == tool2->getUid());
}

TEST_CASE("ViewWrapper: Setting controlling tool overrides active tool for view", "[integration][plugins][org.custusx.core.view]")
{
	cxtest::VisualizationHelper visHelper;
		
	cx::DummyToolPtr tool2(new cx::DummyTool("dummytool2"));
	cx::DummyToolManager::DummyToolManagerPtr toolManager = boost::dynamic_pointer_cast<cx::DummyToolManager>(visHelper.services->tracking());
	REQUIRE(toolManager);
	toolManager->addTool(tool2);
	
	cx::ViewGroupDataPtr viewGroupData  = cx::ViewGroupDataPtr(new cx::ViewGroupData(visHelper.services, "testViewGroup"));
	
	REQUIRE(viewGroupData);
	visHelper.viewWrapper->setViewGroup(viewGroupData);
	viewGroupData->setControllingTool(tool2);
	
	cx::ToolPtr controllingTool = visHelper.viewWrapper->getControllingTool();
	cx::ToolPtr  activeTool = visHelper.services->tracking()->getActiveTool();
	CHECK(controllingTool == tool2);
	CHECK_FALSE(controllingTool == activeTool);
	//CX_LOG_DEBUG() << "controllingTool: " << controllingTool->getName();
	//CX_LOG_DEBUG() << "activeTool: " << activeTool->getName();
}

TEST_CASE("ViewWrapper: Resetting controlling tool reverts back to using active tool", "[integration][plugins][org.custusx.core.view]")
{
	cxtest::VisualizationHelper visHelper;
		
	cx::DummyToolPtr tool2(new cx::DummyTool("dummytool2"));
	cx::DummyToolManager::DummyToolManagerPtr toolManager = boost::dynamic_pointer_cast<cx::DummyToolManager>(visHelper.services->tracking());
	REQUIRE(toolManager);
	toolManager->addTool(tool2);
	
	cx::ViewGroupDataPtr viewGroupData  = cx::ViewGroupDataPtr(new cx::ViewGroupData(visHelper.services, "testViewGroup"));
	
	REQUIRE(viewGroupData);
	visHelper.viewWrapper->setViewGroup(viewGroupData);
	viewGroupData->setControllingTool(tool2);
	
	cx::ToolPtr controllingTool = visHelper.viewWrapper->getControllingTool();
	cx::ToolPtr activeTool = visHelper.services->tracking()->getActiveTool();
	CHECK_FALSE(controllingTool == activeTool);
	
	viewGroupData->setControllingTool(cx::ToolPtr());
	
	controllingTool = visHelper.viewWrapper->getControllingTool();
	activeTool = visHelper.services->tracking()->getActiveTool();
	CHECK(controllingTool == activeTool);
}

TEST_CASE("ViewWrapper: ActiveTool", "[integration][plugins][org.custusx.core.view]")
{
	cxtest::VisualizationHelper visHelper;

	QString toolUid("dummytool2");
	cx::DummyToolPtr tool2(new cx::DummyTool(toolUid));
	cx::DummyToolManager::DummyToolManagerPtr toolManager = boost::dynamic_pointer_cast<cx::DummyToolManager>(visHelper.services->tracking());
	REQUIRE(toolManager);
	toolManager->addTool(tool2);
	CHECK_FALSE(visHelper.services->tracking()->getActiveTool()->getUid() == toolUid);

	visHelper.services->tracking()->setActiveTool(tool2->getUid());
	CHECK(visHelper.services->tracking()->getActiveTool()->getUid() == toolUid);

	visHelper.services->tracking()->clearActiveTool();
	CHECK_FALSE(visHelper.services->tracking()->getActiveTool());
}
