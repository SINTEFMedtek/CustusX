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

/**Helper object for automated control of the CustusX application.
 *
 */
class CustusXController : public QObject
{
  Q_OBJECT

public:
  cx::MainWindow* mMainWindow;
  QString mPatientFolder;

  CustusXController(QObject* parent);
  void start();
  void stop();

public slots:
  void loadPatientSlot();
  void beginCheckRenderSlot();
  void endCheckRenderSlot();

private:
};


#endif /* CXTESTCUSTUSXCONTROLLER_H_ */
