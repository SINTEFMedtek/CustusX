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

#include "cxDataManager.h"
#include "cxDummyTool.h"
#include "cxTypeConversions.h"
#include "cxData.h"
#include "cxConsoleWidget.h"
#include "cxImage.h"
#include "cxPatientData.h"
#include "cxCyclicActionLogger.h"
#include "cxTrackingService.h"
#include "cxViewManager.h"
#include "cxStateService.h"
#include "cxPatientService.h"
#include "cxLogicManager.h"
#include "cxWorkflowStateMachine.h"


#include "cxClippingWidget.h"
#include "cxInteractiveClipper.h"
#include "cxViewManager.h"
#include "cxDataManager.h"
#include "cxSettings.h"
#include "cxPatientModelService.h"

CustusXController::CustusXController(QObject* parent) : QObject(parent)
{
  mTestData += "Test Results:\n";
  mMainWindow = NULL;
  mBaseTime = 1000;
	mMeasuredFPS = 0;
	mEnableSlicing = false;
}
void CustusXController::start()
{
  qApp->setOrganizationName("SINTEF");
  qApp->setOrganizationDomain("test.sintef.no");
  qApp->setApplicationName("CustusX");
//  qApp->setWindowIcon(QIcon(":/icons/CustusX.png"));
//  qApp->setWindowIcon(QIcon(":/icons/.png"));

  cx::LogicManager::initialize();
  cx::settings()->setValue("Automation/autoSave", "false");

  mMainWindow = new cx::MainWindow(std::vector<cx::GUIExtenderServicePtr>());
  mMainWindow->show();
#ifdef __APPLE__ // needed on mac for bringing to front: does the opposite on linux
  mMainWindow->activateWindow();
#endif
  mMainWindow->raise();


  QTimer::singleShot(      0,   this, SLOT(initialBeginCheckRenderSlot()) );
  QTimer::singleShot(      0,   this, SLOT(loadPatientSlot()) );
	if(mEnableSlicing)
		QTimer::singleShot(      0,   this, SLOT(enableSlicingSlot()) );

}
void CustusXController::stop()
{
  delete mMainWindow;
  cx::LogicManager::shutdown(); // shutdown all global resources, _after_ gui is deleted.
}

void CustusXController::loadPatientSlot()
{
  cx::patientService()->loadPatient(mPatientFolder);
  cx::stateService()->getWorkflow()->setActiveState("NavigationUid");
  mMainWindow->setGeometry( 0, 0, 2560, 1440);

  if (!cx::dataService()->getImages().size())
		return;

  cx::ImagePtr image = cx::dataService()->getImages().begin()->second;
  cx::DoubleBoundingBox3D bb_r = transform(image->get_rMd(), image->boundingBox());

  cx::dataService()->setCenter(bb_r.center());

  std::vector<cx::TrackingSystemServicePtr> systems = cx::trackingService()->getTrackingSystems();
  for (unsigned i=0; i<systems.size(); ++i)
	  cx::trackingService()->unInstallTrackingSystem(systems[i]);

  cx::DummyToolPtr dummyTool(new cx::DummyTool());
  dummyTool->setToolPositionMovement(dummyTool->createToolPositionMovementTranslationOnly(bb_r));
  cx::trackingService()->runDummyTool(dummyTool);
}


void CustusXController::enableSlicingSlot()
{
		cx::InteractiveClipperPtr interactiveClipper = cx::viewManager()->getClipper();
		interactiveClipper->setSlicePlane(cx::ptAXIAL);

	//	ImagePtr image = dataManager()->getActiveImage();
		std::map<QString, cx::ImagePtr> imageMap = cx::dataManager()->getImages();
		if(!imageMap.empty())
		{
			cx::ImagePtr image = imageMap.begin()->second;
			interactiveClipper->setImage(image);
			interactiveClipper->useClipper(true);
//			std::cout << "clip in image: " << image->getName()  << std::endl;
		}
		else
			std::cout << "No images!!!"  << std::endl;

}

void CustusXController::initialBeginCheckRenderSlot()
{
  cx::viewManager()->getRenderTimer()->reset(mBaseTime);
  QTimer::singleShot( 5*1000,   this, SLOT(initialEndCheckRenderSlot()) );
}

void CustusXController::initialEndCheckRenderSlot()
{
//  std::cout << cx::viewManager()->getRenderTimer()->dumpStatistics() << std::endl;
//  mTestData += cx::viewManager()->getRenderTimer()->dumpStatistics();

  // start next timing
  cx::viewManager()->getRenderTimer()->reset(5*mBaseTime);
  QTimer::singleShot(4*mBaseTime,   this, SLOT(secondEndCheckRenderSlot()) );
}
//
//void CustusXController::secondBeginCheckRenderSlot()
//{
//  cx::viewManager()->getRenderTimer()->reset(100000);
//
//  QTimer::singleShot(20*1000,   this, SLOT(secondEndCheckRenderSlot()) );
//}

void CustusXController::secondEndCheckRenderSlot()
{

  std::cout << cx::viewManager()->getRenderTimer()->dumpStatistics() << std::endl;
  mTestData += cx::viewManager()->getRenderTimer()->dumpStatistics() + "\n";
  mMeasuredFPS = cx::viewManager()->getRenderTimer()->getFPS();
//  mTestData += "\n";

	//this->displayResultsSlot();
  QTimer::singleShot(2*1000,   qApp, SLOT(quit()) );
}

void CustusXController::displayResultsSlot()
{
  //std::cout << "Hello TextEdit!" << std::endl;
  QTextEdit* textEdit = new QTextEdit;
  textEdit->resize(900,480);
  textEdit->setText(mTestData);
  textEdit->show();
}
