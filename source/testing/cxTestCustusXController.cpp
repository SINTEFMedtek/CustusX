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
#include "cxRenderTimer.h"
#include "cxToolManager.h"
#include "cxViewManager.h"
#include "cxStateService.h"
#include "cxPatientService.h"

CustusXController::CustusXController(QObject* parent) : QObject(parent)
{
  mTestData += "Test Results:\n";
  mMainWindow = NULL;
  mBaseTime = 1000;
}
void CustusXController::start()
{
  qApp->setOrganizationName("SINTEF");
  qApp->setOrganizationDomain("test.sintef.no");
  qApp->setApplicationName("CustusX3");
//  qApp->setWindowIcon(QIcon(":/icons/CustusX.png"));
//  qApp->setWindowIcon(QIcon(":/icons/.png"));

  cx::MainWindow::initialize();

  mMainWindow = new cx::MainWindow(std::vector<cx::PluginBasePtr>());
  mMainWindow->show();
#ifdef __APPLE__ // needed on mac for bringing to front: does the opposite on linux
  mMainWindow->activateWindow();
#endif
  mMainWindow->raise();


  QTimer::singleShot(      0,   this, SLOT(initialBeginCheckRenderSlot()) );
  QTimer::singleShot(      0,   this, SLOT(loadPatientSlot()) );
}
void CustusXController::stop()
{
  delete mMainWindow;
  cx::MainWindow::shutdown(); // shutdown all global resources, _after_ gui is deleted.
}

void CustusXController::loadPatientSlot()
{
  cx::patientService()->getPatientData()->loadPatient(mPatientFolder);
  cx::stateService()->getWorkflow()->setActiveState("NavigationUid");
  mMainWindow->setGeometry( 10, 10, 1200, 800);

  if (!ssc::DataManager::getInstance()->getImages().size())
    return;

  ssc::ImagePtr image = ssc::DataManager::getInstance()->getImages().begin()->second;
  ssc::DoubleBoundingBox3D bb_r = transform(image->get_rMd(), image->boundingBox());

  ssc::DataManager::getInstance()->setCenter(bb_r.center());

  ssc::DummyToolPtr dummyTool(new ssc::DummyTool(cx::ToolManager::getInstance()));
  dummyTool->setToolPositionMovement(dummyTool->createToolPositionMovementTranslationOnly(bb_r));
  cx::ToolManager::getInstance()->runDummyTool(dummyTool);
}

void CustusXController::initialBeginCheckRenderSlot()
{
  cx::viewManager()->getRenderTimer()->reset(mBaseTime);
  QTimer::singleShot( 5*1000,   this, SLOT(initialEndCheckRenderSlot()) );
}

void CustusXController::initialEndCheckRenderSlot()
{
  std::cout << cx::viewManager()->getRenderTimer()->dumpStatistics() << std::endl;
  mTestData += cx::viewManager()->getRenderTimer()->dumpStatistics();

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
//  std::cout << cx::viewManager()->getRenderTimer()->dumpStatistics() << std::endl;
//  mTestData += cx::viewManager()->getRenderTimer()->dumpStatistics();

//  this->displayResultsSlot();
}

void CustusXController::displayResultsSlot()
{
  //std::cout << "Hello TextEdit!" << std::endl;
  QTextEdit* textEdit = new QTextEdit;
  textEdit->resize(800,480);
  textEdit->setText(mTestData);
  textEdit->show();
}
