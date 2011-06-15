#include "cxMainWindow.h"

#include <QDockWidget>
#include <QStatusBar>
#include "iostream"
#include "sscTypeConversions.h"

#ifdef __APPLE__
  #include "cxMacGrabberServerWidget.h"
#endif /*APPLE*/
#ifdef WIN32
  #include "cxWinGrabberServerWidget.h"
#endif /*WIN32*/

namespace cx
{

MainWindow::MainWindow(QStringList arguments) :
  QMainWindow(0),
  mConsoleWidget(new ssc::ConsoleWidget(this)),
  mQueueInfoLabel(new QLabel("Queue size: 0")),
  mImagesDroppedLabel(new QLabel("Images dropped: 0"))
{
  this->setCentralWidget(new QWidget());

#ifdef __APPLE__
  mGrabberServerWidget = new MacGrabberServerWidget(this->centralWidget());
#endif /*APPLE*/
#ifdef WIN32
  mGrabberServerWidget = new WinGrabberServerWidget(this->centralWidget());
#endif /*WIN32*/

  this->setCentralWidget(mGrabberServerWidget);
  connect(mGrabberServerWidget, SIGNAL(queueInfo(int, int)), this, SLOT(queueInfoSlot(int, int)));
  
  QDockWidget* consoleDockWidget = new QDockWidget(mConsoleWidget->windowTitle(), this);
  consoleDockWidget->setObjectName(mConsoleWidget->objectName() + "DockWidget");
  consoleDockWidget->setWidget(mConsoleWidget);
  this->addDockWidget(Qt::BottomDockWidgetArea, consoleDockWidget);
  consoleDockWidget->setVisible(true); // default visibility

  this->setMinimumSize(800,600);
  this->statusBar()->addPermanentWidget(mQueueInfoLabel);
  this->statusBar()->addPermanentWidget(mImagesDroppedLabel);

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

void MainWindow::queueInfoSlot(int queueInfo, int imagesDropped)
{
  mQueueInfoLabel->setText("Queue size: "+qstring_cast(queueInfo));
  mImagesDroppedLabel->setText("Images dropped: "+qstring_cast(imagesDropped));
}
}//namespace cx
