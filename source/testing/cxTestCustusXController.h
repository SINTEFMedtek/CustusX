/*
 * cxTestCustusXController.h
 *
 *  Created on: Oct 19, 2010
 *      Author: christiana
 */
#ifndef CXTESTCUSTUSXCONTROLLER_H_
#define CXTESTCUSTUSXCONTROLLER_H_


#include <QApplication>
#include "cxMainWindow.h"
#include "cxPatientData.h"

/**Helper object for automated control of the CustusX application.
 *
 */
class CustusXController : public QObject
{
  Q_OBJECT

public:
  cx::MainWindow* mMainWindow;
  QString mPatientFolder;

  CustusXController(QObject* parent) : QObject(parent)
  {
    mMainWindow = NULL;
  }
  void start()
  {
    qApp->setOrganizationName("SINTEF");
    qApp->setOrganizationDomain("test.sintef.no");
    qApp->setApplicationName("CustusX3");
    qApp->setWindowIcon(QIcon(":/icons/CustusX.png"));
    qApp->setWindowIcon(QIcon(":/icons/.png"));

    cx::MainWindow::initialize();

    mMainWindow = new cx::MainWindow;
    mMainWindow->show();
  #ifdef __APPLE__ // needed on mac for bringing to front: does the opposite on linux
    mMainWindow->activateWindow();
  #endif
    mMainWindow->raise();
  }
  void stop()
  {
    delete mMainWindow;
    cx::MainWindow::shutdown(); // shutdown all global resources, _after_ gui is deleted.
  }

public slots:
  void loadPatientSlot()
  {
    cx::stateManager()->getPatientData()->loadPatient("/Users/christiana/Patients/Kaisa_Speed_Test.cx3");
  }

private:
};


#endif /* CXTESTCUSTUSXCONTROLLER_H_ */
