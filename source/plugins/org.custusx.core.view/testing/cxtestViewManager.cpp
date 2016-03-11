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
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include "cxtestVisServices.h"
#include "cxViewManager.h"
#include "cxDataLocations.h"
#include "cxSettings.h"
#include "cxViewGroupData.h"
#include "cxtestTestDataStructures.h"
#include "cxLogicManager.h"
#include "cxView.h"
#include "cxLogger.h"
#include "cxPatientModelService.h"


namespace cxtest
{
typedef boost::shared_ptr<class ViewManagerFixture> ViewManagerFixturePtr;
class ViewManagerFixture : public cx::ViewManager
{
public:
	ViewManagerFixture(cx::VisServicesPtr services) :
		cx::ViewManager(services)
	{}
	QList<unsigned> getAutoShowViewGroupNumbers()
	{
		return this->getViewGroupsToAutoShowIn();
	}
	void resetCameraToSuperiorView()
	{
		this->autoResetCameraToSuperiorView();
	}
};

TEST_CASE("ViewManager: Auto show in view groups", "[unit][plugins][org.custusx.core.view]")
{
    cx::DataLocations::setTestMode();

    cx::settings()->setValue("Automation/autoShowNewDataInViewGroup0", true);
    cx::settings()->setValue("Automation/autoShowNewDataInViewGroup4", true);
    cxtest::TestVisServicesPtr dummyservices = cxtest::TestVisServices::create();
    ViewManagerFixturePtr viewManager = ViewManagerFixturePtr(new ViewManagerFixture(dummyservices));
    QList<unsigned> showInViewGroups = viewManager->getAutoShowViewGroupNumbers();
    REQUIRE(showInViewGroups.size() == 2);
    CHECK(showInViewGroups[0] == 0);
    CHECK(showInViewGroups[1] == 4);
}

TEST_CASE("ViewManager: Auto reset camera to superior view", "[unit][plugins][org.custusx.core.view]")
{
    cx::LogicManager::initialize();
    cx::DataLocations::setTestMode();

    TestDataStructures testData;


    cx::settings()->setValue("Automation/autoShowNewDataInViewGroup0", true);
    cx::settings()->setValue("Automation/autoShowNewDataInViewGroup4", true);

//    cxtest::TestVisServicesPtr dummyservices = cxtest::TestVisServices::create();
    cx::VisServicesPtr services = cx::VisServices::create(cx::logicManager()->getPluginContext());

    services->patient()->insertData(testData.image1);

    ViewManagerFixturePtr viewManager = ViewManagerFixturePtr(new ViewManagerFixture(services));
    viewManager->createLayoutWidget(NULL, 0);

    cx::CameraControlPtr camera = viewManager->getCameraControl();

    cx::ViewGroupDataPtr viewGroupData = viewManager->getViewGroup(0);
    viewGroupData->addDataSorted(testData.image1->getUid());

    cx::ViewPtr view0 = viewManager->get3DView(0);
    REQUIRE(view0);
    vtkCameraPtr vtkCamera = viewManager->get3DView(0)->getRenderer()->GetActiveCamera();
    cx::Vector3D position(vtkCamera->GetPosition());
    CX_LOG_DEBUG() << "camera pos: " << position;
    CX_LOG_DEBUG() << "focal point: " << cx::Vector3D(vtkCamera->GetFocalPoint());
    CX_LOG_DEBUG() << "View up: " << cx::Vector3D(vtkCamera->GetViewUp());
    viewManager->resetCameraToSuperiorView();
    vtkRendererPtr renderer = view0->getRenderer();
    REQUIRE(renderer);
//    renderer->ResetCamera();
    cx::Vector3D changedposition(vtkCamera->GetPosition());
    CX_LOG_DEBUG() << "camera pos: " << changedposition;
    CX_LOG_DEBUG() << "focal point: " << cx::Vector3D(vtkCamera->GetFocalPoint());
    CX_LOG_DEBUG() << "View up: " << cx::Vector3D(vtkCamera->GetViewUp());
    REQUIRE_FALSE(cx::similar(position, changedposition));

    cx::LogicManager::shutdown();
}

} // cxtest