#include "sscMainWindow.h"

namespace ssc
{

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
	//  http://www.openclipart.org/
	// http://tango.freedesktop.org/Tango_Desktop_Project
	// http://www.everaldo.com/crystal/

	this->setWindowTitle("QtSandbox");

    mTextEdit = new QPlainTextEdit;
    setCentralWidget(mTextEdit);

    addActions();
    addToolbar();
    addMenu();
    // play with this one:
    //setUnifiedTitleAndToolBarOnMac(true);

    createStatusBar();
}

void MainWindow::addActions()
{
	mAction1 = new QAction("Action1", this);
    mAction1->setIcon(QIcon(":/images/go-home.png"));

	mAction2 = new QAction("Action2", this);
    mAction2->setIcon(QIcon(":/images/application-exit.png"));

    mAboutQtAct = new QAction(tr("About &Qt"), this);
    mAboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(mAboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    mAboutAct = new QAction(tr("About QtSandbox"), this);
    mAboutAct->setStatusTip(tr("Show the Application's About box"));
    connect(mAboutAct, SIGNAL(triggered()), this, SLOT(about()));
}

void MainWindow::addToolbar()
{
	mToolbar = this->addToolBar("Mytoolbar");
	mToolbar->addAction(mAction1);
	mToolbar->addAction(mAction2);
}

void MainWindow::addMenu()
{
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(mAction1);
    fileMenu->addAction(mAction2);

    QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(mAboutQtAct);
    helpMenu->addAction(mAboutAct);
}


void MainWindow::about()
{
   QMessageBox::about(this, tr("About QtSandbox"),
            tr("The <b>QtSandbox</b> is a place where you can "
               "test out nifty Qt features in a small app. Enjoy!"));
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

}
