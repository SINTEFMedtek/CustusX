#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QMainWindow>

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
};

}

#endif /* MAINWINDOW_H_ */
