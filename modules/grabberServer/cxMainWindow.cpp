#include "cxMainWindow.h"

#include <QDockWidget>
#include <QStatusBar>
#include "iostream"
#include "sscTypeConversions.h"

namespace cx
{

MainWindow::MainWindow(QStringList arguments) :
  QMainWindow(0),
  mConsoleWidget(new ssc::ConsoleWidget(this)),
  mQueueSizeLabel(new QLabel("Queue size: 0"))
{
  this->setCentralWidget(new QWidget());
  mGrabberServerWidget = new MacGrabberServerWidget(this->centralWidget());
  this->setCentralWidget(mGrabberServerWidget);
  connect(mGrabberServerWidget, SIGNAL(queueSize(int)), this, SLOT(queueSizeSlot(int)));
  
  QDockWidget* consoleDockWidget = new QDockWidget(mConsoleWidget->windowTitle(), this);
  consoleDockWidget->setObjectName(mConsoleWidget->objectName() + "DockWidget");
  consoleDockWidget->setWidget(mConsoleWidget);
  this->addDockWidget(Qt::BottomDockWidgetArea, consoleDockWidget);
  consoleDockWidget->setVisible(true); // default visibility

  this->setMinimumSize(800,600);
  this->statusBar()->addPermanentWidget(mQueueSizeLabel);

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

void MainWindow::queueSizeSlot(int queueSize)
{
  mQueueSizeLabel->setText("Queue size: "+qstring_cast(queueSize));
}
}//namespace cx
