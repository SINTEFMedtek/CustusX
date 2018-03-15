/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

