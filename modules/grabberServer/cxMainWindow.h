#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QMainWindow>
#include "sscConsoleWidget.h"
#include "cxGrabberServerWidget.h"

namespace cx
{
/**
 * \class MainWindow
 *
 * \brief Mainwindow for displaying a GrabberServerWidget and a ConsoleWidget.
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
};

}

#endif /* MAINWINDOW_H_ */
