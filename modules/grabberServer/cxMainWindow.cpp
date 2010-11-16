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
}

MainWindow::~MainWindow()
{}

}//namespace cx
