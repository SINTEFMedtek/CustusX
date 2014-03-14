#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QMainWindow>
#include <QLabel>
#include "cxConsoleWidget.h"
#include "cxGrabberServerWidget.h"

namespace cx
{
/**
 * \class MainWindow
 *
 * \brief Mainwindow for displaying a GrabberServerWidget and a ConsoleWidget.
 *
 * \date 16. nov. 2010
 * \\author Janne Beate Bakeng, SINTEF
 */
class MainWindow : public QMainWindow
{
  Q_OBJECT
public:
  MainWindow(QStringList arguments = QStringList());
  ~MainWindow();

  void handleArguments(QStringList& arguments);

private slots:
  void queueInfoSlot(int queueInfo, int imagesDropped);

private:
  GrabberServerWidget*  mGrabberServerWidget;
  ConsoleWidget*   mConsoleWidget;

  QLabel* mQueueInfoLabel;
  QLabel* mImagesDroppedLabel;
};

}

#endif /* MAINWINDOW_H_ */
