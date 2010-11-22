#include "cxMainWindow.h"

#include "cxGrabberServerWidget.h"

namespace cx
{

MainWindow::MainWindow() :
  QMainWindow(0)
{
  this->setCentralWidget(new QWidget());
  mGrabberServerWidget = new GrabberServerWidget(this->centralWidget());
  this->setCentralWidget(mGrabberServerWidget);
  //this->layout()->addWidget(mGrabberServerWidget);
  
  this->setMinimumSize(800,600);
}

MainWindow::~MainWindow()
{}

}//namespace cx
