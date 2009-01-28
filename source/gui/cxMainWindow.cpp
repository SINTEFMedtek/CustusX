#include "cxMainWindow.h"

#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QFileDialog>
#include <QStatusBar>
#include <QFileInfo>
#include "sscDataManager.h"
#include "cxViewManager.h"
#include "cxRepManager.h"
#include "cxToolManager.h"
#include "cxCustomStatusBar.h"
#include "cxImageRegistrationDockWidget.h"

/**
 * cxMainWindow.cpp
 *
 * \brief
 *
 * \date Jan 20, 2009
 * \author: Janne Beate Bakeng, SINTEF
 */

namespace cx
{
MainWindow::MainWindow() :
  mViewManager(ViewManager::getInstance()),
  mDataManager(DataManager::getInstance()),
  mToolManager(ToolManager::getInstance()),
  mRepManager(RepManager::getInstance()),
  mCentralWidget(new QWidget()),
  mImageRegistrationDockWidget(new ImageRegistrationDockWidget()),
  mCustomStatusBar(new CustomStatusBar())
{
  this->createActions();
  this->createToolBars();
  this->createMenus();
  this->createStatusBar();

  this->setCentralWidget(mCentralWidget);
  this->resize(QSize(1000,1000));
  this->show();

  mViewManager->setCentralWidget(*mCentralWidget);
}
MainWindow::~MainWindow()
{}
void MainWindow::createActions()
{
  //TODO: add shortcuts and tooltips

  //workflow
  mWorkflowActionGroup = new QActionGroup(this);
  mPatientDataWorkflowAction = new QAction(tr("Acquire patient data"), this);
  mImageRegistrationWorkflowAction = new QAction(tr("Image registration"), this);
  mPatientRegistrationWorkflowAction = new QAction(tr("Patient registration"), this);
  mNavigationWorkflowAction = new QAction(tr("Navigation"), this);
  mUSAcquisitionWorkflowAction = new QAction(tr("US acquisition"), this);

  mWorkflowActionGroup->addAction(mPatientDataWorkflowAction);
  mWorkflowActionGroup->addAction(mImageRegistrationWorkflowAction);
  mWorkflowActionGroup->addAction(mPatientRegistrationWorkflowAction);
  mWorkflowActionGroup->addAction(mNavigationWorkflowAction);
  mWorkflowActionGroup->addAction(mUSAcquisitionWorkflowAction);
  mPatientDataWorkflowAction->setChecked(true);

  connect(mPatientDataWorkflowAction, SIGNAL(triggered()),
          this, SLOT(patientDataWorkflowSlot()));
  connect(mImageRegistrationWorkflowAction, SIGNAL(triggered()),
          this, SLOT(imageRegistrationWorkflowSlot()));
  connect(mPatientRegistrationWorkflowAction, SIGNAL(triggered()),
          this, SLOT(patientRegistrationWorkflowSlot()));
  connect(mNavigationWorkflowAction, SIGNAL(triggered()),
          this, SLOT(navigationWorkflowSlot()));
  connect(mUSAcquisitionWorkflowAction, SIGNAL(triggered()),
          this, SLOT(usAcquisitionWorkflowSlot()));

  //data
  mLoadDataAction = new QAction(QIcon(":/icons/open.png"), tr("&Load data"), this);
  mLoadDataAction->setShortcut(tr("Ctrl+L"));
  mLoadDataAction->setStatusTip(tr("Load image data"));

  connect(mLoadDataAction, SIGNAL(triggered()),
          this, SLOT(loadDataSlot()));

  //tool
  mToolsActionGroup = new QActionGroup(this);
  mConfigureToolsAction =  new QAction(tr("Tool configuration"), this);
  mInitializeToolsAction =  new QAction(tr("Initialize"), this);
  mStartTrackingToolsAction =  new QAction(tr("Start tracking"), this);
  mStopTrackingToolsAction =  new QAction(tr("Stop tracking"), this);

  mToolsActionGroup->addAction(mConfigureToolsAction);
  mToolsActionGroup->addAction(mInitializeToolsAction);
  mToolsActionGroup->addAction(mStartTrackingToolsAction);
  mToolsActionGroup->addAction(mStopTrackingToolsAction);
  mConfigureToolsAction->setChecked(true);

  connect(mConfigureToolsAction, SIGNAL(triggered()),
          this, SLOT(configureSlot()));
  connect(mInitializeToolsAction, SIGNAL(triggered()),
          mToolManager, SLOT(initialize()));
  connect(mStartTrackingToolsAction, SIGNAL(triggered()),
          mToolManager, SLOT(startTracking()));
  connect(mStopTrackingToolsAction, SIGNAL(triggered()),
          mToolManager, SLOT(stopTracking()));

  //layout
  mLayoutActionGroup = new QActionGroup(this);
  m3D_1x1_LayoutAction = new QAction(tr("3D_1X1"), this);
  m3DACS_2x2_LayoutAction = new QAction(tr("3DACS_2X2"), this);
  m3DACS_1x3_LayoutAction = new QAction(tr("3DACS_1X3"), this);
  mACSACS_2x3_LayoutAction = new QAction(tr("ACSACS_2X3"), this);

  mLayoutActionGroup->addAction(m3D_1x1_LayoutAction);
  mLayoutActionGroup->addAction(m3DACS_2x2_LayoutAction);
  mLayoutActionGroup->addAction(m3DACS_1x3_LayoutAction);
  mLayoutActionGroup->addAction(mACSACS_2x3_LayoutAction);
  m3D_1x1_LayoutAction->setChecked(true);

  connect(m3D_1x1_LayoutAction, SIGNAL(triggered()),
      mViewManager, SLOT(setLayoutTo_3D_1X1()));
  connect(m3DACS_2x2_LayoutAction, SIGNAL(triggered()),
      mViewManager, SLOT(setLayoutTo_3DACS_2X2()));
  connect(m3DACS_1x3_LayoutAction, SIGNAL(triggered()),
      mViewManager, SLOT(setLayoutTo_3DACS_1X3()));
  connect(mACSACS_2x3_LayoutAction, SIGNAL(triggered()),
      mViewManager, SLOT(setLayoutTo_ACSACS_2X3()));
}
void MainWindow::createMenus()
{
  mWorkflowMenu = new QMenu(tr("Workflow"), this);;
  mDataMenu = new QMenu(tr("Data"), this);
  mToolMenu = new QMenu(tr("Tracking"), this);
  mLayoutMenu = new QMenu(tr("Layouts"), this);

  //workflow
  this->menuBar()->addMenu(mWorkflowMenu);
  mWorkflowMenu->addAction(mPatientDataWorkflowAction);
  mWorkflowMenu->addAction(mImageRegistrationWorkflowAction);
  mWorkflowMenu->addAction(mPatientRegistrationWorkflowAction);
  mWorkflowMenu->addAction(mNavigationWorkflowAction);
  mWorkflowMenu->addAction(mUSAcquisitionWorkflowAction);

  //data
  this->menuBar()->addMenu(mDataMenu);
  mDataMenu->addAction(mLoadDataAction);

  //tool
  this->menuBar()->addMenu(mToolMenu);
  mToolMenu->addAction(mConfigureToolsAction);
  mToolMenu->addAction(mInitializeToolsAction);
  mToolMenu->addAction(mStartTrackingToolsAction);
  mToolMenu->addAction(mStopTrackingToolsAction);

  //tool
  this->menuBar()->addMenu(mLayoutMenu);
  mLayoutMenu->addAction(m3D_1x1_LayoutAction);
  mLayoutMenu->addAction(m3DACS_2x2_LayoutAction);
  mLayoutMenu->addAction(m3DACS_1x3_LayoutAction);
  mLayoutMenu->addAction(mACSACS_2x3_LayoutAction);
}
void MainWindow::createToolBars()
{
  mDataToolBar = addToolBar("Data");
  mDataToolBar->addAction(mLoadDataAction);

  mToolToolBar = addToolBar("Tools");
  mToolToolBar->addAction(mStartTrackingToolsAction);
  mToolToolBar->addAction(mStopTrackingToolsAction);

}
void MainWindow::createStatusBar()
{
  this->setStatusBar(mCustomStatusBar);
}
void MainWindow::aboutSlot()
{}
void MainWindow::preferencesSlot()
{}
void MainWindow::quitSlot()
{}
void MainWindow::patientDataWorkflowSlot()
{}
void MainWindow::imageRegistrationWorkflowSlot()
{}
void MainWindow::patientRegistrationWorkflowSlot()
{}
void MainWindow::navigationWorkflowSlot()
{}
void MainWindow::usAcquisitionWorkflowSlot()
{}
void MainWindow::loadDataSlot()
{
  this->statusBar()->showMessage(QString(tr("Loading data..")));
  QString fileName = QFileDialog::getOpenFileName( this,
                                  QString(tr("Select data file")),
                                  mCurrentPatientDataFolder );
  if(fileName.isEmpty())
  {
    statusBar()->showMessage(QString(tr("Load cancelled")));
    return;
  }
  QFileInfo fileInfo(fileName);
  QString fileType = fileInfo.completeSuffix();
  if(fileType.compare("mhd", Qt::CaseInsensitive) == 0 ||
     fileType.compare("hdr", Qt::CaseInsensitive) == 0)
  {
    mDataManager->loadImage(fileName.toStdString(), ssc::rtMETAIMAGE);
    this->statusBar()->showMessage(QString(tr("Meta data loaded.")));
  }else if(fileType.compare("stl", Qt::CaseInsensitive) == 0)
  {
    mDataManager->loadMesh(fileName.toStdString(), ssc::mrtSTL);
    this->statusBar()->showMessage(QString(tr("STL data loaded.")));
  }else if(fileType.compare("vtk", Qt::CaseInsensitive) == 0)
  {
    mDataManager->loadMesh(fileName.toStdString(), ssc::mrtPOLYDATA);
    //mMessageManager.sendInfo("Vtk data loaded.");
  }

}
void MainWindow::configureSlot()
{
  QString configFile = QFileDialog::getOpenFileName(this, tr("Open file"),
                                                    "/home",
                                                    tr("Configuration files (*.xml)"));
  mToolManager->setConfigurationFile(configFile.toStdString());

  QString loggingFolder = QFileDialog::getExistingDirectory(this, tr("Open directory"),
                                                            "/home",
                                                            QFileDialog::ShowDirsOnly);
  mToolManager->setLoggingFolder(loggingFolder.toStdString());

  mToolManager->configure();
}
}//namespace cx
