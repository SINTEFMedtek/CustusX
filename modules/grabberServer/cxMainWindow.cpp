#include "cxMainWindow.h"

#include "cxGrabberServerWidget.h"
#include <QDockWidget>

namespace cx
{

MainWindow::MainWindow() :
  QMainWindow(0),
  mConsoleWidget(new ssc::ConsoleWidget(this))
{
  this->setCentralWidget(new QWidget());
  mGrabberServerWidget = new GrabberServerWidget(this->centralWidget());
  this->setCentralWidget(mGrabberServerWidget);
  
  QDockWidget* dockWidget = new QDockWidget(mConsoleWidget->windowTitle(), this);
  dockWidget->setObjectName(mConsoleWidget->objectName() + "DockWidget");
  dockWidget->setWidget(mConsoleWidget);
  this->addDockWidget(Qt::BottomDockWidgetArea, dockWidget);
  dockWidget->setVisible(true); // default visibility

  this->setMinimumSize(800,600);
}

MainWindow::~MainWindow()
{}

}//namespace cx
