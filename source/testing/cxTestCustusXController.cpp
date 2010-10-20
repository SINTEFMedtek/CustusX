/*
 * cxTestCustusXController.cpp
 *
 *  Created on: Oct 19, 2010
 *      Author: christiana
 */

#include "cxTestCustusXController.h"

#include "cxPatientData.h"
#include "cxRenderTimer.h"
#include "sscDataManager.h"
#include "cxToolManager.h"
#include "sscDummyTool.h"

CustusXController::CustusXController(QObject* parent) : QObject(parent)
{
  mMainWindow = NULL;
}
void CustusXController::start()
{
  qApp->setOrganizationName("SINTEF");
  qApp->setOrganizationDomain("test.sintef.no");
  qApp->setApplicationName("CustusX3");
//  qApp->setWindowIcon(QIcon(":/icons/CustusX.png"));
//  qApp->setWindowIcon(QIcon(":/icons/.png"));

  cx::MainWindow::initialize();

  mMainWindow = new cx::MainWindow;
  mMainWindow->show();
#ifdef __APPLE__ // needed on mac for bringing to front: does the opposite on linux
  mMainWindow->activateWindow();
#endif
  mMainWindow->raise();
}
void CustusXController::stop()
{
  delete mMainWindow;
  cx::MainWindow::shutdown(); // shutdown all global resources, _after_ gui is deleted.
}

void CustusXController::loadPatientSlot()
{
  cx::stateManager()->getPatientData()->loadPatient("/Users/christiana/Patients/Kaisa_Speed_Test.cx3");

  if (!ssc::DataManager::getInstance()->getImages().size())
    return;

  ssc::ImagePtr image = ssc::DataManager::getInstance()->getImages().begin()->second;
  ssc::DoubleBoundingBox3D bb_r = transform(image->get_rMd(), image->boundingBox());
  //ssc::Vector3D center = image->boundingBox().center();
  //center = image->get_rMd().coord(center);
  ssc::DataManager::getInstance()->setCenter(bb_r.center());

  ssc::DummyToolPtr dummyTool(new ssc::DummyTool());
//  dummyTool->setToolPositionMovementBB(bb_r);
  dummyTool->setToolPositionMovement(dummyTool->createToolPositionMovementTranslationOnly(bb_r));
  cx::ToolManager::getInstance()->runDummyTool(dummyTool);
}
void CustusXController::beginCheckRenderSlot()
{
  cx::viewManager()->getRenderTimer()->reset(100000);
}
void CustusXController::endCheckRenderSlot()
{
  cx::viewManager()->getRenderTimer()->dumpStatistics();
}
