#include "cxMainWindow.h"

#include "cxGrabberServerWidget.h"
#include <QDockWidget>

namespace cx
{

MainWindow::MainWindow() :
  QMainWindow(0),
  mConsoleWidget(new ssc::ConsoleWidget(this))
  //mPropertiesWidget(new GrabberServerPropertiesWidget(this))
{
  this->setCentralWidget(new QWidget());
  mGrabberServerWidget = new GrabberServerWidget(this->centralWidget());
  this->setCentralWidget(mGrabberServerWidget);
  
  //mPropertiesWidget->setGrabber(mGrabberServerWidget->getGrabber());
  //mPropertiesWidget->setServer(mGrabberServerWidget->getServer());

  QDockWidget* consoleDockWidget = new QDockWidget(mConsoleWidget->windowTitle(), this);
  consoleDockWidget->setObjectName(mConsoleWidget->objectName() + "DockWidget");
  consoleDockWidget->setWidget(mConsoleWidget);
  this->addDockWidget(Qt::BottomDockWidgetArea, consoleDockWidget);
  consoleDockWidget->setVisible(true); // default visibility

  /*QDockWidget* propertiesDockWidget = new QDockWidget(mPropertiesWidget->windowTitle(), this);
  propertiesDockWidget->setObjectName(mPropertiesWidget->objectName() + "DockWidget");
  propertiesDockWidget->setWidget(mPropertiesWidget);
  this->addDockWidget(Qt::BottomDockWidgetArea, propertiesDockWidget);
  propertiesDockWidget->setVisible(true); // default visibility*/

  this->setMinimumSize(800,600);
}

MainWindow::~MainWindow()
{}

}//namespace cx
