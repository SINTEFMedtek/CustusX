#include "cxMainWindow.h"

#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QFileDialog>
#include <QStatusBar>
#include <QFileInfo>
#include "cxDataManager.h"
#include "cxViewManager.h"
#include "cxRepManager.h"
#include "cxToolManager.h"
#include "cxMessageManager.h"
#include "cxCustomStatusBar.h"
#include "cxContextDockWidget.h"
#include "cxTransferFunctionWidget.h"
#include "cxImageRegistrationWidget.h"
#include "cxPatientRegistrationWidget.h"

namespace cx
{
MainWindow::MainWindow() :
  mCurrentWorkflowState(PATIENT_DATA),
  mViewManager(ViewManager::getInstance()),
  mDataManager(DataManager::getInstance()),
  mToolManager(ToolManager::getInstance()),
  mRepManager(RepManager::getInstance()),
  mMessageManager(MessageManager::getInstance()),
  mCentralWidget(new QWidget(this)),
  mContextDockWidget(new ContextDockWidget(this)),
  mImageRegistrationWidget(new ImageRegistrationWidget(mContextDockWidget)),
  mPatientRegistrationWidget(new PatientRegistrationWidget(mContextDockWidget)),
  mTransferFunctionWidget(new TransferFunctionWidget(mContextDockWidget)),
  mImageRegistrationIndex(-1),
  mPatientRegistrationIndex(-1)
  //mCustomStatusBar(new CustomStatusBar())
{
  // Don't draw Widget yet. 
  // Prevents flicker and the visualizetion bug to start drawing lines and dots.
  setUpdatesEnabled(false);
  
  this->createActions();
  this->createToolBars();
  this->createMenus();
  this->createStatusBar();

  this->setCentralWidget(mCentralWidget);
  this->resize(QSize(1000,1000));
  this->show();

  mViewManager->setCentralWidget(*mCentralWidget);

  //debugging
  connect(mToolManager, SIGNAL(toolManagerReport(std::string)),
          this, SLOT(printSlot(std::string)));

  this->changeState(PATIENT_DATA, PATIENT_DATA);
  
  
  // Try to fix visualization bug by adding and removing tabs
  // Looks like mImageRegistrationWidget and mPatientRegistrationWidget
  // draws some lines and dots when the are added to mContextDockWidget
  // without more information about how and where the should be shown
  // TODO: Find a better way to do this
  mImageRegistrationIndex = mContextDockWidget->addTab(mImageRegistrationWidget,
                                                       QString("Image Registration"));
  mContextDockWidget->removeTab(mImageRegistrationIndex);
  mPatientRegistrationIndex = mContextDockWidget->addTab(mPatientRegistrationWidget,
                                                         QString("Patient Registration"));
  mContextDockWidget->removeTab(mPatientRegistrationIndex);
  
  // Ok to draw Widget
  setUpdatesEnabled(true);  
}
MainWindow::~MainWindow()
{}
void MainWindow::createActions()
{
  //TODO: add shortcuts and tooltips

  //workflow
  mWorkflowActionGroup = new QActionGroup(this);
  mPatientDataWorkflowAction = new QAction(tr("Acquire patient data"), mWorkflowActionGroup);
  mImageRegistrationWorkflowAction = new QAction(tr("Image registration"), mWorkflowActionGroup);
  mPatientRegistrationWorkflowAction = new QAction(tr("Patient registration"), mWorkflowActionGroup);
  mNavigationWorkflowAction = new QAction(tr("Navigation"), mWorkflowActionGroup);
  mUSAcquisitionWorkflowAction = new QAction(tr("US acquisition"), mWorkflowActionGroup);
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
  mConfigureToolsAction =  new QAction(tr("Tool configuration"), mToolsActionGroup);
  mInitializeToolsAction =  new QAction(tr("Initialize"), mToolsActionGroup);
  mStartTrackingToolsAction =  new QAction(tr("Start tracking"), mToolsActionGroup);
  mStopTrackingToolsAction =  new QAction(tr("Stop tracking"), mToolsActionGroup);

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
  m3D_1x1_LayoutAction = new QAction(tr("3D_1X1"), mLayoutActionGroup);
  m3DACS_2x2_LayoutAction = new QAction(tr("3DACS_2X2"), mLayoutActionGroup);
  m3DACS_1x3_LayoutAction = new QAction(tr("3DACS_1X3"), mLayoutActionGroup);
  mACSACS_2x3_LayoutAction = new QAction(tr("ACSACS_2X3"), mLayoutActionGroup);

  m3D_1x1_LayoutAction->setChecked(true);

  connect(m3D_1x1_LayoutAction, SIGNAL(triggered()),
      mViewManager, SLOT(setLayoutTo_3D_1X1()));
  connect(m3DACS_2x2_LayoutAction, SIGNAL(triggered()),
      mViewManager, SLOT(setLayoutTo_3DACS_2X2()));
  connect(m3DACS_1x3_LayoutAction, SIGNAL(triggered()),
      mViewManager, SLOT(setLayoutTo_3DACS_1X3()));
  connect(mACSACS_2x3_LayoutAction, SIGNAL(triggered()),
      mViewManager, SLOT(setLayoutTo_ACSACS_2X3()));

  //context widgets
  this->addDockWidget(Qt::LeftDockWidgetArea, mContextDockWidget);
  connect(mContextDockWidget, SIGNAL(currentImageChanged(ssc::ImagePtr)),
          mImageRegistrationWidget, SLOT(currentImageChangedSlot(ssc::ImagePtr)));
  connect(mContextDockWidget, SIGNAL(currentImageChanged(ssc::ImagePtr)),
          mPatientRegistrationWidget, SLOT(currentImageChangedSlot(ssc::ImagePtr)));
  connect(mContextDockWidget, SIGNAL(currentImageChanged(ssc::ImagePtr)),
          mTransferFunctionWidget, SLOT(currentImageChangedSlot(ssc::ImagePtr)));
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
  //TODO, not working as intended
  //this->setStatusBar(mCustomStatusBar);
}
void MainWindow::changeState(WorkflowState fromState, WorkflowState toState)
{
  switch (fromState)
  {
  case PATIENT_DATA:
    this->deactivatePatientDataState();
    break;
  case IMAGE_REGISTRATION:
    this->deactivateImageRegistationState();
    break;
  case PATIENT_REGISTRATION:
    this->deactivatePatientRegistrationState();
    break;
  case NAVIGATION:
    this->deactivateNavigationState();
    break;
  case US_ACQUISITION:
    this->deactivateUSAcquisitionState();
    break;
  default:
    mMessageManager->sendWarning("Could not determine what workflow state to deactivate.");
    return;
    break;
  };

  switch (toState)
  {
  case PATIENT_DATA:
    this->activatePatientDataState();
    break;
  case IMAGE_REGISTRATION:
    this->activateImageRegistationState();
    break;
  case PATIENT_REGISTRATION:
    this->activatePatientRegistrationState();
    break;
  case NAVIGATION:
    this->activateNavigationState();
    break;
  case US_ACQUISITION:
    this->activateUSAcquisitionState();
    break;
  default:
    mMessageManager->sendWarning("Could not determine what workflow state to activate.");
    this->activatePatientDataState();
    return;
    break;
  };
}
void MainWindow::activatePatientDataState()
{
  mCurrentWorkflowState = PATIENT_DATA;
  //should never be removed
  mImageRegistrationIndex = mContextDockWidget->addTab(mTransferFunctionWidget,
      QString("Transfer functions"));
}
void MainWindow::deactivatePatientDataState()
{}
void MainWindow::activateImageRegistationState()
{
  mImageRegistrationIndex = mContextDockWidget->addTab(mImageRegistrationWidget,
      QString("Image Registration"));

  mCurrentWorkflowState = IMAGE_REGISTRATION;
}
void MainWindow::deactivateImageRegistationState()
{
  if(mImageRegistrationIndex != -1)
  {
    mContextDockWidget->removeTab(mImageRegistrationIndex);
    mImageRegistrationIndex = -1;
  }
}
void MainWindow::activatePatientRegistrationState()
{
  mPatientRegistrationIndex = mContextDockWidget->addTab(mPatientRegistrationWidget,
      QString("Patient Registration"));

  mCurrentWorkflowState = PATIENT_REGISTRATION;
}
void MainWindow::deactivatePatientRegistrationState()
{
  if(mPatientRegistrationIndex != -1)
  {
    mContextDockWidget->removeTab(mPatientRegistrationIndex);
    mPatientRegistrationIndex = -1;
  }
}
void MainWindow::activateNavigationState()
{
  mCurrentWorkflowState = NAVIGATION;
}
void MainWindow::deactivateNavigationState()
{}
void MainWindow::activateUSAcquisitionState()
{
  mCurrentWorkflowState = US_ACQUISITION;
}
void MainWindow::deactivateUSAcquisitionState()
{}
void MainWindow::aboutSlot()
{
  //TODO
}
void MainWindow::preferencesSlot()
{
  //TODO
}
void MainWindow::quitSlot()
{
  //TODO
}
void MainWindow::patientDataWorkflowSlot()
{
  this->changeState(mCurrentWorkflowState, PATIENT_DATA);
}
void MainWindow::imageRegistrationWorkflowSlot()
{
  this->changeState(mCurrentWorkflowState, IMAGE_REGISTRATION);
}
void MainWindow::patientRegistrationWorkflowSlot()
{
  this->changeState(mCurrentWorkflowState, PATIENT_REGISTRATION);
}
void MainWindow::navigationWorkflowSlot()
{
  this->changeState(mCurrentWorkflowState, NAVIGATION);
}
void MainWindow::usAcquisitionWorkflowSlot()
{
  this->changeState(mCurrentWorkflowState, US_ACQUISITION);
}
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
//  std::cout << "fileName: " << fileName.toAscii() << std::endl;
  QFileInfo fileInfo(fileName);
  QString fileType = fileInfo.suffix();
  if(fileType.compare("mhd", Qt::CaseInsensitive) == 0 ||
     fileType.compare("hdr", Qt::CaseInsensitive) == 0)
  {
    ssc::ImagePtr image = mDataManager->loadImage(fileName.toStdString(), ssc::rtMETAIMAGE);
    mMessageManager->sendInfo("Meta data loaded.");
  }else if(fileType.compare("stl", Qt::CaseInsensitive) == 0)
  {
    mDataManager->loadMesh(fileName.toStdString(), ssc::mrtSTL);
    mMessageManager->sendInfo("STL data loaded.");
  }else if(fileType.compare("vtk", Qt::CaseInsensitive) == 0)
  {
    mDataManager->loadMesh(fileName.toStdString(), ssc::mrtPOLYDATA);
    mMessageManager->sendInfo("Vtk data loaded.");
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
void MainWindow::printSlot(std::string message)
{
  //TODO REMOVE just for debugging
  //std::cout << message << std::endl;
}
}//namespace cx
