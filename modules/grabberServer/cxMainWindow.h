#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QMainWindow>
#include "sscConsoleWidget.h"
#include "cxGrabberServerPropertiesWidget.h"

namespace cx
{
class GrabberServerWidget;
/**
 * \class MainWindow
 *
 * \brief
 *
 * \date 16. nov. 2010
 * \author: Janne Beate Bakeng, SINTEF
 */
class MainWindow : public QMainWindow
{
  Q_OBJECT
public:
  MainWindow();
  ~MainWindow();

private:
  GrabberServerWidget* mGrabberServerWidget;
  ssc::ConsoleWidget*   mConsoleWidget;
  GrabberServerPropertiesWidget*  mPropertiesWidget;
};

}

#endif /* MAINWINDOW_H_ */
