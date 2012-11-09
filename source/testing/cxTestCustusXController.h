/*
 * cxTestCustusXController.h
 *
 *  \date Oct 19, 2010
 *      \author christiana
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
  QString mTestData;
  int mBaseTime;

  CustusXController(QObject* parent);
  void start();
  void stop();

public slots:
  void loadPatientSlot();
  void initialBeginCheckRenderSlot();
  void initialEndCheckRenderSlot();
//  void secondBeginCheckRenderSlot();
  void secondEndCheckRenderSlot();
  void displayResultsSlot();

private:
};


#endif /* CXTESTCUSTUSXCONTROLLER_H_ */
