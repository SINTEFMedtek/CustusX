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
#include "cxTestCustusXController.h"

#include <sstream>
#include <QTextEdit>
#include <QTimer>

#include "cxDummyTool.h"
#include "cxTypeConversions.h"
#include "cxData.h"
#include "cxConsoleWidget.h"
#include "cxImage.h"
#include "cxCyclicActionLogger.h"
#include "cxTrackingService.h"
#include "cxStateService.h"
#include "cxLogicManager.h"

#include "cxClippingWidget.h"
#include "cxInteractiveClipper.h"
#include "cxSettings.h"
#include "cxPatientModelService.h"
#include "cxViewService.h"
#include "cxSessionStorageService.h"
#include "cxClippers.h"

#include "catch.hpp"

CustusXController::CustusXController(QObject* parent) : QObject(parent)
{
	mTestData += "Test Results:\n";
	mBaseTime = 1000;
	mMeasuredFPS = 0;
	mEnableSlicing = false;
    mNumInitialRenders = 0;
}

CustusXController::~CustusXController()
{
}

void CustusXController::start()
{
  qApp->setOrganizationName("SINTEF");
  qApp->setOrganizationDomain("test.sintef.no");
  qApp->setApplicationName("CustusX");

//  typedef cx::MainWindowApplicationComponent<cx::MainWindow> MainWindowComponent;
  mApplicationComponent.reset(new MainWindowComponent());
//  std::auto_ptr<cx::ApplicationComponentPtr mainwindow(new cx::MainWindowApplicationComponent<cx::MainWindow>());

  cx::LogicManager::initialize(mApplicationComponent);
  cx::settings()->setValue("Automation/autoSave", "false");
  cx::settings()->setValue("Automation/autoLoadRecentPatient", "");

  QTimer::singleShot(0, this, SLOT(initialBeginCheckRenderSlot()) );
  QTimer::singleShot(0, this, SLOT(loadPatientSlot()) );
	if(mEnableSlicing)
		QTimer::singleShot(0, this, SLOT(enableSlicingSlot()) );
}

void CustusXController::stop()
{
  cx::LogicManager::shutdown(); // shutdown all global resources, _after_ gui is deleted.
}

void CustusXController::loadPatientSlot()
{
  cx::logicManager()->getSessionStorageService()->load(mPatientFolder);
  cx::logicManager()->getStateService()->setWorkFlowState("NavigationUid");
//  mMainWindow->setGeometry( 0, 0, 2560, 1440);
  mApplicationComponent->mMainWindow->setGeometry(QRect(0, 0, 1200, 1200));

  if (!cx::logicManager()->getPatientModelService()->getDataOfType<cx::Image>().size())
		return;

  cx::ImagePtr image = cx::logicManager()->getPatientModelService()->getDataOfType<cx::Image>().begin()->second;
  cx::DoubleBoundingBox3D bb_r = transform(image->get_rMd(), image->boundingBox());

  cx::logicManager()->getPatientModelService()->setCenter(bb_r.center());

  std::vector<cx::TrackingSystemServicePtr> systems = cx::logicManager()->getTrackingService()->getTrackingSystems();
  for (unsigned i=0; i<systems.size(); ++i)
	  cx::logicManager()->getTrackingService()->unInstallTrackingSystem(systems[i]);

  cx::DummyToolPtr dummyTool(new cx::DummyTool());
  dummyTool->setToolPositionMovement(dummyTool->createToolPositionMovementTranslationOnly(bb_r));
  cx::logicManager()->getTrackingService()->runDummyTool(dummyTool);
}


void CustusXController::enableSlicingSlot()
{
	cx::ClippersPtr clippers = cx::logicManager()->getViewService()->getClippers();
	QString clipperName("Axial");
	REQUIRE(clippers->exists(clipperName));
	cx::InteractiveClipperPtr interactiveClipper = clippers->getClipper(clipperName);

		std::map<QString, cx::ImagePtr> imageMap = cx::logicManager()->getPatientModelService()->getDataOfType<cx::Image>();
		if(!imageMap.empty())
		{
			cx::ImagePtr image = imageMap.begin()->second;
			interactiveClipper->setData(image);
			interactiveClipper->useClipper(true);
		}
		else
			std::cout << "No images!!!"  << std::endl;

}

void CustusXController::initialBeginCheckRenderSlot()
{
  cx::logicManager()->getViewService()->getRenderTimer()->reset(mBaseTime);
  connect(cx::logicManager()->getViewService().get(), &cx::ViewService::renderFinished, this, &CustusXController::initialEndCheckRenderSlot);
}

void CustusXController::initialEndCheckRenderSlot()
{
    mNumInitialRenders++;
    if(mNumInitialRenders < 2)
        return;

	disconnect(cx::logicManager()->getViewService().get(), &cx::ViewService::renderFinished, this, &CustusXController::initialEndCheckRenderSlot);
  // start next timing
  cx::logicManager()->getViewService()->getRenderTimer()->reset(5*mBaseTime);
  QTimer::singleShot(4*mBaseTime,   this, SLOT(secondEndCheckRenderSlot()) );
}

void CustusXController::secondEndCheckRenderSlot()
{

  std::cout << cx::logicManager()->getViewService()->getRenderTimer()->dumpStatistics() << std::endl;
  mTestData += cx::logicManager()->getViewService()->getRenderTimer()->dumpStatistics() + "\n";
  mMeasuredFPS = cx::logicManager()->getViewService()->getRenderTimer()->getFPS();

  QTimer::singleShot(2*1000,   qApp, SLOT(quit()) );
}

void CustusXController::displayResultsSlot()
{
  QTextEdit* textEdit = new QTextEdit;
  textEdit->resize(900,480);
  textEdit->setText(mTestData);
  textEdit->show();
}

void CustusXController::changeToNewProfile()
{
	cx::logicManager()->restartWithNewProfile("new_profile");
}

