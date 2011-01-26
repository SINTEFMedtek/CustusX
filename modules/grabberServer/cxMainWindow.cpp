#include "cxMainWindow.h"

#include <QDockWidget>
#include "iostream"

namespace cx
{

MainWindow::MainWindow(QStringList arguments) :
  QMainWindow(0),
  mConsoleWidget(new ssc::ConsoleWidget(this))
{
  this->setCentralWidget(new QWidget());
  mGrabberServerWidget = new MacGrabberServerWidget(this->centralWidget());
  this->setCentralWidget(mGrabberServerWidget);
  
  QDockWidget* consoleDockWidget = new QDockWidget(mConsoleWidget->windowTitle(), this);
  consoleDockWidget->setObjectName(mConsoleWidget->objectName() + "DockWidget");
  consoleDockWidget->setWidget(mConsoleWidget);
  this->addDockWidget(Qt::BottomDockWidgetArea, consoleDockWidget);
  consoleDockWidget->setVisible(true); // default visibility

  this->setMinimumSize(800,600);

  this->handleArguments(arguments);
}

MainWindow::~MainWindow()
{}

void MainWindow::handleArguments(QStringList& arguments)
{
  if(arguments.contains("--auto"))
  {
    mGrabberServerWidget->startServerSlot(true);
  }
}
}//namespace cx
