/*
 * cxTestCustusXController.cpp
 *
 *  \date Oct 19, 2010
 *      \author christiana
 */

#include "cxTestCustusXController.h"

#include <sstream>
#include <QTextEdit>
#include <QTimer>

#include "sscDataManager.h"
#include "sscDummyTool.h"
#include "sscTypeConversions.h"
#include "sscData.h"
#include "sscConsoleWidget.h"
#include "sscImage.h"
#include "cxPatientData.h"
#include "cxCyclicActionLogger.h"
#include "cxToolManager.h"
#include "cxViewManager.h"
#include "cxStateService.h"
#include "cxPatientService.h"
#include "cxLogicManager.h"
#include "cxWorkflowStateMachine.h"


#include "cxClippingWidget.h"
#include "cxInteractiveClipper.h"
#include "cxViewManager.h"
#include "cxDataManager.h"

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
  qApp->setApplicationName("CustusX3");
//  qApp->setWindowIcon(QIcon(":/icons/CustusX.png"));
//  qApp->setWindowIcon(QIcon(":/icons/.png"));

  cx::LogicManager::initialize();

  mMainWindow = new cx::MainWindow(std::vector<cx::PluginBasePtr>());
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
  cx::patientService()->getPatientData()->loadPatient(mPatientFolder);
  cx::stateService()->getWorkflow()->setActiveState("NavigationUid");
  mMainWindow->setGeometry( 0, 0, 2560, 1440);

  if (!cx::DataManager::getInstance()->getImages().size())
		return;

  cx::ImagePtr image = cx::DataManager::getInstance()->getImages().begin()->second;
  cx::DoubleBoundingBox3D bb_r = transform(image->get_rMd(), image->boundingBox());

  cx::DataManager::getInstance()->setCenter(bb_r.center());

  cx::DummyToolPtr dummyTool(new cx::DummyTool(cx::cxToolManager::getInstance()));
  dummyTool->setToolPositionMovement(dummyTool->createToolPositionMovementTranslationOnly(bb_r));
  cx::cxToolManager::getInstance()->runDummyTool(dummyTool);
}


void CustusXController::enableSlicingSlot()
{
		cx::InteractiveClipperPtr interactiveClipper = cx::viewManager()->getClipper();
		interactiveClipper->setSlicePlane(cx::ptAXIAL);

	//	ImagePtr image = dataManager()->getActiveImage();
		std::map<QString, cx::ImagePtr> imageMap = cx::dataManager()->getImages();
		if(imageMap.size() > 0)
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
