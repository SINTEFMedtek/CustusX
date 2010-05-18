#include "cxMainWindow.h"

#include <QtGui>
#include "sscTypeConversions.h"
#include "sscTime.h"
#include "cxDataManager.h"
#include "cxViewManager.h"
#include "cxRepManager.h"
#include "cxToolManager.h"
#include "cxMessageManager.h"
#include "cxRegistrationManager.h"
#include "cxCustomStatusBar.h"
#include "cxContextDockWidget.h"
#include "cxBrowserWidget.h"
#include "cxNavigationWidget.h"
#include "cxTransferFunctionWidget.h"
#include "cxImageRegistrationWidget.h"
#include "cxToolPropertiesWidget.h"
#include "cxPatientRegistrationWidget.h"
#include "cxView3D.h"
#include "cxView2D.h"
#include "cxViewGroup.h"
#include "cxPreferencesDialog.h"
#include "cxShiftCorrectionWidget.h"
#include "cxImagePropertiesWidget.h"
#include "cxPointSamplingWidget.h"
#include "sscReconstructionWidget.h"
#include "cxPatientData.h"

namespace cx
{
  
MainWindow::MainWindow() :
  mCurrentWorkflowState(PATIENT_DATA),
  mCentralWidget(new QWidget(this)),
  mContextDockWidget(new ContextDockWidget(this)),
  //mImageRegistrationWidget(new ImageRegistrationWidget(mContextDockWidget)),
  //mPatientRegistrationWidget(new PatientRegistrationWidget(mContextDockWidget)),
  //mTransferFunctionWidget(new TransferFunctionWidget(mContextDockWidget)),
  //mShiftCorrectionWidget(new ShiftCorrectionWidget(mContextDockWidget)),
  mImageRegistrationWidget(new ImageRegistrationWidget(NULL)),
  mPatientRegistrationWidget(new PatientRegistrationWidget(NULL)),
  mTransferFunctionWidget(new TransferFunctionWidget(mContextDockWidget)),
  mShiftCorrectionWidget(new ShiftCorrectionWidget(NULL)),
  mBrowserWidget(new BrowserWidget(mContextDockWidget)),
  //mNavigationWidget(new NavigationWidget(mContextDockWidget)),
  mNavigationWidget(new NavigationWidget(NULL)),
  mCustomStatusBar(new CustomStatusBar()),
  mImagePropertiesWidget(new ImagePropertiesWidget(this)),
  mToolPropertiesWidget(new ToolPropertiesWidget(this)),
  mPointSamplingWidget(new PointSamplingWidget(this)),
  mReconstructionWidget(new ssc::ReconstructionWidget(this)),
  mImageRegistrationIndex(-1),
  mShiftCorrectionIndex(-1),
  mPatientRegistrationIndex(-1),
  mNavigationIndex(-1),
  mSettings(new QSettings()),
  mPatientData(new PatientData(this))
  //mActivePatientFolder("")
{
#ifdef VERSION_NUMBER
  this->setWindowTitle(QString("CustusX %1").arg(VERSION_NUMBER));
#else
#endif
  
  //make sure the transferefunctionwidget it fully initialized
  mTransferFunctionWidget->init();

  this->createActions();
  this->createToolBars();
  this->createMenus();
  this->createStatusBar();

  this->setCentralWidget(viewManager()->stealCentralWidget());
  
  // Initialize settings if empty
  if (!mSettings->contains("globalPatientDataFolder"))
    mSettings->setValue("globalPatientDataFolder", QDir::homePath()+"/Patients");
  if (!mSettings->contains("toolConfigFilePath"))
    mSettings->setValue("toolConfigFilePath", QDir::homePath());
  if (!mSettings->contains("globalApplicationName"))
    mSettings->setValue("globalApplicationName", "Nevro");
  if (!mSettings->contains("globalPatientNumber"))
    mSettings->setValue("globalPatientNumber", 1);
  //if (!mSettings->contains("applicationNames"))
    mSettings->setValue("applicationNames", "Nevro,Lap,Vasc,Lung");
  
  
  if (!mSettings->contains("renderingInterval"))
    mSettings->setValue("renderingInterval", 33);
  if (!mSettings->contains("shadingOn"))
    mSettings->setValue("shadingOn", true);  

  //debugging
  connect(messageManager(), SIGNAL(emittedMessage(const QString&, int)),
          this, SLOT(loggingSlot(const QString&, int)));

  connect(mPatientData.get(), SIGNAL(patientChanged()), this, SLOT(patientChangedSlot()));

  this->changeState(PATIENT_DATA, PATIENT_DATA);
  
  // TODO: Find a better way to do this
  //if we remove this section some items stack in the upper left corner,
  //probably some items missing a parent, check it out.
  // Solution?: Set parent to NULL in constructors
  /*mImageRegistrationIndex = mContextDockWidget->addTab(mImageRegistrationWidget,
                                                       QString("Image Registration"));
  mContextDockWidget->removeTab(mImageRegistrationIndex);
  mPatientRegistrationIndex = mContextDockWidget->addTab(mPatientRegistrationWidget,
                                                         QString("Patient Registration"));
  mContextDockWidget->removeTab(mPatientRegistrationIndex);
  mNavigationIndex = mContextDockWidget->addTab(mNavigationWidget,
                                                         QString("Navigation"));
  mContextDockWidget->removeTab(mNavigationIndex);*/
    

//  QDockWidget* imagePropertiesDockWidget = new QDockWidget("Image Properties", this);
//  imagePropertiesDockWidget->setObjectName("ImagePropertiesDockWidget");
//  imagePropertiesDockWidget->setWidget(mImagePropertiesWidget);
//  this->addDockWidget(Qt::LeftDockWidgetArea, imagePropertiesDockWidget);

  this->addAsDockWidget(mImagePropertiesWidget);
  this->addAsDockWidget(mToolPropertiesWidget);
  this->addAsDockWidget(mPointSamplingWidget);
  this->addAsDockWidget(mReconstructionWidget);
  
  // Restore saved window states
  // Must be done after all DockWidgets are created
  if(!restoreGeometry(mSettings->value("mainWindow/geometry").toByteArray()))
    this->resize(QSize(1200,1000));//Set initial size if no previous size exist
  restoreState(mSettings->value("mainWindow/windowState").toByteArray());
  
  // Don't show the Widget before all elements are initialized
  this->show();
}

void MainWindow::addAsDockWidget(QWidget* widget)
{
  QDockWidget* dockWidget = new QDockWidget(widget->windowTitle(), this);
  dockWidget->setObjectName(widget->objectName()+"DockWidget");
  dockWidget->setWidget(widget);
  this->addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
  dockWidget->setVisible(false); // default visibility
}

MainWindow::~MainWindow()
{}
void MainWindow::createActions()
{
  //TODO: add shortcuts and tooltips
	
  // File
  mNewPatientAction = new QAction(tr("&New patient"), this);
  mNewPatientAction->setShortcut(tr("Ctrl+N"));
  mSaveFileAction = new QAction(tr("&Save Patient file"), this);
  mSaveFileAction->setShortcut(tr("Ctrl+S"));
  mLoadFileAction = new QAction(tr("&Load Patient file"), this);
  mLoadFileAction->setShortcut(tr("Ctrl+L"));
  
  connect(mNewPatientAction, SIGNAL(triggered()),
          this, SLOT(newPatientSlot()));
  connect(mLoadFileAction, SIGNAL(triggered()),
          this, SLOT(loadPatientFileSlot()));
  connect(mSaveFileAction, SIGNAL(triggered()),
          this, SLOT(savePatientFileSlot()));

  // Application
  mAboutAction = new QAction(tr("&About"), this);  // About burde gitt About CustusX, det gj√∏r det ikke av en eller annen grunn???
  mAboutAction->setShortcut(tr("Ctrl+A"));
  mAboutAction->setStatusTip(tr("Show the application's About box"));
  mPreferencesAction = new QAction(tr("&Preferences"), this);
  mPreferencesAction->setShortcut(tr("Ctrl+P"));
  mPreferencesAction->setStatusTip(tr("Show the preferences dialog"));

  mDebugModeAction = new QAction(tr("&Debug Mode"), this);
  mDebugModeAction->setShortcut(tr("Ctrl+D"));
  mDebugModeAction->setCheckable(true);
  mDebugModeAction->setChecked(dataManager()->getDebugMode());
  mDebugModeAction->setStatusTip(tr("Set debug mode, this enables lots of weird stuff."));

  mQuitAction = new QAction(tr("&Quit"), this);
  mQuitAction->setShortcut(tr("Ctrl+Q"));
  mQuitAction->setStatusTip(tr("Exit the application"));
  
  connect(mAboutAction, SIGNAL(triggered()), this, SLOT(aboutSlot()));
  connect(mPreferencesAction, SIGNAL(triggered()), this, SLOT(preferencesSlot()));
  connect(mDebugModeAction, SIGNAL(triggered(bool)), dataManager(), SLOT(setDebugMode(bool)));
  connect(dataManager(), SIGNAL(debugModeChanged(bool)), mDebugModeAction, SLOT(setChecked(bool)));
  connect(mQuitAction, SIGNAL(triggered()), this, SLOT(quitSlot()));
  
  //View
  this->mToggleContextDockWidgetAction = mContextDockWidget->toggleViewAction();
  mToggleContextDockWidgetAction->setText("Context Widget");
  
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
  mImportDataAction = new QAction(QIcon(":/icons/open.png"), tr("&Import data"), this);
  mImportDataAction->setShortcut(tr("Ctrl+I"));
  mImportDataAction->setStatusTip(tr("Import image data"));
  
  mDeleteDataAction = new QAction(tr("Delete current image"), this);
  mDeleteDataAction->setStatusTip(tr("Delete selected volume"));

  mLoadPatientRegistrationFromFile = new QAction(tr("Load patient registration from file"), this);
  mLoadPatientRegistrationFromFile->setStatusTip("Select a txt-file to use as patient registration");

  connect(mImportDataAction, SIGNAL(triggered()),
          this, SLOT(importDataSlot()));
  connect(mDeleteDataAction, SIGNAL(triggered()),
          this, SLOT(deleteDataSlot()));
  connect(mLoadPatientRegistrationFromFile, SIGNAL(triggered()),
          this, SLOT(loadPatientRegistrationSlot()));

  //tool
  mToolsActionGroup = new QActionGroup(this);
  mConfigureToolsAction =  new QAction(tr("Tool configuration"), mToolsActionGroup);
  mInitializeToolsAction =  new QAction(tr("Initialize"), mToolsActionGroup);
  mStartTrackingToolsAction =  new QAction(tr("Start tracking"), mToolsActionGroup);
  mStopTrackingToolsAction =  new QAction(tr("Stop tracking"), mToolsActionGroup);
  mSaveToolsPositionsAction = new QAction(tr("Save positions"), this);

  mConfigureToolsAction->setChecked(true);

  connect(mConfigureToolsAction, SIGNAL(triggered()),
          this, SLOT(configureSlot()));
  connect(mInitializeToolsAction, SIGNAL(triggered()),
          toolManager(), SLOT(initialize()));
  connect(mStartTrackingToolsAction, SIGNAL(triggered()),
          toolManager(), SLOT(startTracking()));
  connect(mStopTrackingToolsAction, SIGNAL(triggered()),
          toolManager(), SLOT(stopTracking()));
  connect(mSaveToolsPositionsAction, SIGNAL(triggered()), 
          toolManager(), SLOT(saveToolsSlot()));

  //layout
  mLayoutActionGroup = new QActionGroup(this);
  mLayoutActionGroup->setExclusive(true);
  
  std::vector<LayoutType> layouts = ViewManager::getInstance()->availableLayouts();
  for (unsigned i=0; i<layouts.size(); ++i)
    addLayoutAction(layouts[i]);

  connect(viewManager(), SIGNAL(activeLayoutChanged()), this, SLOT(layoutChangedSlot()));
  layoutChangedSlot();

  //context widgets
  this->addDockWidget(Qt::LeftDockWidgetArea, mContextDockWidget);


  mCenterToImageCenterAction = new QAction(tr("Center Image"), this);
  connect(mCenterToImageCenterAction, SIGNAL(triggered()), this, SLOT(centerToImageCenterSlot()));
  mCenterToTooltipAction = new QAction(tr("Center Tool"), this);
  connect(mCenterToTooltipAction, SIGNAL(triggered()), this, SLOT(centerToTooltipSlot()));


  //TODO remove
  /*connect(mContextDockWidget, SIGNAL(currentImageChanged(ssc::ImagePtr)),
          viewManager(), SLOT(currentImageChangedSlot(ssc::ImagePtr)));*/

  connect(dataManager(), SIGNAL(activeImageChanged(std::string)),
          mImageRegistrationWidget, SLOT(activeImageChangedSlot()));
  connect(dataManager(), SIGNAL(activeImageChanged(std::string)),
          mPatientRegistrationWidget, SLOT(activeImageChangedSlot()));
  connect(dataManager(), SIGNAL(activeImageChanged(std::string)),
          mTransferFunctionWidget, SLOT(activeImageChangedSlot()));
  
  connect(this, SIGNAL(deleteCurrentImage()),
          mContextDockWidget, SLOT(deleteCurrentImageSlot()));
}

void MainWindow::centerToImageCenterSlot()
{
  Navigation().centerToImageCenter();
}

void MainWindow::centerToTooltipSlot()
{
  Navigation().centerToTooltip();
}

//void MainWindow::setActivePatient(const QString& activePatientFolder)
//{
//  mActivePatientFolder = activePatientFolder;
//  //TODO
//  //Update gui in some way to show which patient is active
//}

void MainWindow::newPatientSlot()
{
  QString patientDatafolder = mSettings->value("globalPatientDataFolder").toString();
  QString name = QDateTime::currentDateTime().toString(ssc::timestampSecondsFormat()) + "_";
  //name += "_";
  name += mSettings->value("globalApplicationName").toString() + "_";
  //name += "_";
  name += mSettings->value("globalPatientNumber").toString() + ".cx3";
  //name += ".cx3";


  // Create folders
  if(!QDir().exists(patientDatafolder))
  {
    QDir().mkdir(patientDatafolder);
    messageManager()->sendInfo("Made a new patient folder: "+patientDatafolder.toStdString());
  }

  QString choosenDir = patientDatafolder + "/" + name;
  //choosenDir += "/";
  //choosenDir += name;
  // Open file dialog, get patient data folder
  choosenDir = QFileDialog::getSaveFileName(this,
                                            tr("Select directory to save patient in"),
                                            choosenDir);
  if (choosenDir == QString::null)
    return; // On cancel

  // Update global patient number
  int patientNumber = mSettings->value("globalPatientNumber").toInt();
  mSettings->setValue("globalPatientNumber", ++patientNumber);

//  createPatientFolders(choosenDir);
  mPatientData->newPatient(choosenDir);
}

void MainWindow::savePatientFileSlot()
{
  if(mPatientData->getActivePatientFolder().isEmpty())
  {
    messageManager()->sendWarning("No patient selected, select or create patient before saving!");
    this->newPatientSlot();
    return;
  }

  mPatientData->savePatient();
}

void MainWindow::loadPatientFileSlot()
{
  // Open file dialog
  QString choosenDir = QFileDialog::getExistingDirectory(this, tr("Open directory"),
                                                         mSettings->value("globalPatientDataFolder").toString(),
                                                         QFileDialog::ShowDirsOnly);
  if (choosenDir == QString::null)
    return; // On cancel

  mPatientData->loadPatient(choosenDir);
}

void MainWindow::importDataSlot()
{
  this->savePatientFileSlot();

  messageManager()->sendInfo("Importing data...");
  QString fileName = QFileDialog::getOpenFileName( this,
                                  QString(tr("Select data file")),
                                  mSettings->value("globalPatientDataFolder").toString(),
                                  tr("Image/Mesh (*.mhd *.mha *.stl *.vtk)"));
  if(fileName.isEmpty())
  {
    messageManager()->sendInfo("Import canceled");
    return;
  }

  mPatientData->importData(fileName);
}

void MainWindow::patientChangedSlot()
{
//  mReconstructionWidget->selectData(mPatientData->getActivePatientFullPath()+"/US_Acq/");
}

/** Called when the layout is changed: update the layout menu
 */
void MainWindow::layoutChangedSlot()
{
  LayoutType type = viewManager()->getActiveLayout();
  QList<QAction*> actions = mLayoutActionGroup->actions();
  for (int i=0; i<actions.size(); ++i)
  {
    if (actions[i]->data().toInt()==static_cast<int>(type))
      actions[i]->setChecked(true);
  }
}

/** Called when a layout is selected: introspect the sending action
 *  in order to get correct layout; set it.
 */
void MainWindow::setLayoutSlot()
{
  QAction* action = dynamic_cast<QAction*>(sender());
  if (!action)
    return;
  LayoutType type = static_cast<LayoutType>(action->data().toInt());
  viewManager()->setActiveLayout(type);
}

/** Add one layout as an action to the layout menu.
 */
QAction* MainWindow::addLayoutAction(LayoutType layout)
{
  QAction* action = new QAction(qstring_cast(ViewManager::layoutText(layout)), mLayoutActionGroup);
  action->setCheckable(true);
  action->setData(QVariant(static_cast<int>(layout)));
  connect(action, SIGNAL(triggered()), this, SLOT(setLayoutSlot()));
  return action;
}

void MainWindow::createMenus()
{
  mCustusXMenu = new QMenu(tr("CustusX"), this);;
	mFileMenu = new QMenu(tr("File"), this);;
  mWindowMenu = new QMenu(tr("Window"), this);;
  mWorkflowMenu = new QMenu(tr("Workflow"), this);;
  mDataMenu = new QMenu(tr("Data"), this);
  mToolMenu = new QMenu(tr("Tracking"), this);
  mLayoutMenu = new QMenu(tr("Layouts"), this);

  // Application
  this->menuBar()->addMenu(mCustusXMenu);
  mCustusXMenu->addAction(mAboutAction);
  mCustusXMenu->addAction(mPreferencesAction);
  
  // File
  this->menuBar()->addMenu(mFileMenu);
  mFileMenu->addAction(mNewPatientAction);
  mFileMenu->addAction(mSaveFileAction);
  mFileMenu->addAction(mLoadFileAction);
  mFileMenu->addSeparator();
  mFileMenu->addAction(mDebugModeAction);

  // View
  this->menuBar()->addMenu(mWindowMenu);
  mWindowMenu->addAction(mToggleContextDockWidgetAction);
  
  //workflow
  this->menuBar()->addMenu(mWorkflowMenu);
  mWorkflowMenu->addAction(mPatientDataWorkflowAction);
  mWorkflowMenu->addAction(mImageRegistrationWorkflowAction);
  mWorkflowMenu->addAction(mPatientRegistrationWorkflowAction);
  mWorkflowMenu->addAction(mNavigationWorkflowAction);
  mWorkflowMenu->addAction(mUSAcquisitionWorkflowAction);

  //data
  this->menuBar()->addMenu(mDataMenu);
  mDataMenu->addAction(mImportDataAction);
  mDataMenu->addAction(mDeleteDataAction);
  mDataMenu->addAction(mLoadPatientRegistrationFromFile);

  //tool
  this->menuBar()->addMenu(mToolMenu);
  mToolMenu->addAction(mConfigureToolsAction);
  mToolMenu->addAction(mInitializeToolsAction);
  mToolMenu->addAction(mStartTrackingToolsAction);
  mToolMenu->addAction(mStopTrackingToolsAction);
  mToolMenu->addSeparator();
  mToolMenu->addAction(mSaveToolsPositionsAction);

  //layout
  this->menuBar()->addMenu(mLayoutMenu);
  mLayoutMenu->addActions(mLayoutActionGroup->actions());
}

void MainWindow::createToolBars()
{
  mDataToolBar = addToolBar("Data");
  mDataToolBar->setObjectName("DataToolBar");
  mDataToolBar->addAction(mImportDataAction);

  mToolToolBar = addToolBar("Tools");
  mToolToolBar->setObjectName("ToolToolBar");
  mToolToolBar->addAction(mStartTrackingToolsAction);
  mToolToolBar->addAction(mStopTrackingToolsAction);

  mNavigationToolBar = addToolBar("Navigation");
  mNavigationToolBar->setObjectName("NavigationToolBar");
  mNavigationToolBar->addAction(mCenterToImageCenterAction);
  mNavigationToolBar->addAction(mCenterToTooltipAction);
}
void MainWindow::createStatusBar()
{
  //TODO, not working as intended
  this->setStatusBar(mCustomStatusBar);
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
    messageManager()->sendWarning("Could not determine what workflow state to deactivate.");
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
    messageManager()->sendWarning("Could not determine what workflow state to activate.");
    this->activatePatientDataState();
    return;
    break;
  };
}
void MainWindow::activatePatientDataState()
{
  mCurrentWorkflowState = PATIENT_DATA;

  //should never be removed
  mContextDockWidget->addTab(mBrowserWidget, QString("Browser"));
  mContextDockWidget->addTab(mTransferFunctionWidget, 
                             QString("Transfer functions"));
}
void MainWindow::deactivatePatientDataState()
{}
void MainWindow::activateImageRegistationState()
{  
  QString imagesPath = mPatientData->getActivePatientFullPath()+"/Images";
  mShiftCorrectionWidget->init(imagesPath);
  //TODO: Finish ShiftCorrection
  //Don't show ShiftCorrection in release
  //mShiftCorrectionIndex = mContextDockWidget->addTab(mShiftCorrectionWidget, 
  //    QString("Shift correction"));
  mImageRegistrationIndex = mContextDockWidget->addTab(mImageRegistrationWidget,
      QString("Image Registration"));
  
  viewManager()->setRegistrationMode(ssc::rsIMAGE_REGISTRATED);

  ssc::ProbeRepPtr probeRep = repManager()->getProbeRep("ProbeRep_1");
/*  LandmarkRepPtr landmarkRep = mRepManager->getLandmarkRep("LandmarkRep_1");
  viewManager()->get3DView("View3D_1")->addRep(landmarkRep);
  viewManager()->get3DView("View3D_1")->addRep(probeRep);
  */
  connect(mImageRegistrationWidget, SIGNAL(thresholdChanged(int)),
          probeRep.get(), SLOT(setThresholdSlot(int)));

  mCurrentWorkflowState = IMAGE_REGISTRATION;
}
void MainWindow::deactivateImageRegistationState()
{
  if(mShiftCorrectionIndex != -1)
  {
    mContextDockWidget->removeTab(mShiftCorrectionIndex);
    mShiftCorrectionIndex = -1;
  }
  if(mImageRegistrationIndex != -1)
  {
    viewManager()->setRegistrationMode(ssc::rsNOT_REGISTRATED);
    //TODO: Fix: This doesn't work if we remove more than one tab since the indexes changes on remove.
    // Use clear() instead and add the tabs that should be present
    mContextDockWidget->removeTab(mImageRegistrationIndex);
    mImageRegistrationIndex = -1;
    
    ssc::ProbeRepPtr probeRep = repManager()->getProbeRep("ProbeRep_1");
/*    LandmarkRepPtr landmarkRep = mRepManager->getLandmarkRep("LandmarkRep_1");
    viewManager()->get3DView("View3D_1")->removeRep(landmarkRep);
    viewManager()->get3DView("View3D_1")->removeRep(probeRep);*/

    disconnect(mImageRegistrationWidget, SIGNAL(thresholdChanged(const int)),
            probeRep.get(), SLOT(setThresholdSlot(const int)));
  }
}
void MainWindow::activatePatientRegistrationState()
{
  mPatientRegistrationIndex = mContextDockWidget->addTab(mPatientRegistrationWidget,
      QString("Patient Registration"));
  
  viewManager()->setRegistrationMode(ssc::rsPATIENT_REGISTRATED);
//  LandmarkRepPtr landmarkRep = mRepManager->getLandmarkRep("LandmarkRep_1");
//  viewManager()->get3DView("View3D_1")->addRep(landmarkRep);
  
  mCurrentWorkflowState = PATIENT_REGISTRATION;
}
void MainWindow::deactivatePatientRegistrationState()
{
  if(mPatientRegistrationIndex != -1)
  {
    viewManager()->setRegistrationMode(ssc::rsNOT_REGISTRATED);
    mContextDockWidget->removeTab(mPatientRegistrationIndex);
    mPatientRegistrationIndex = -1;
  }
}
void MainWindow::activateNavigationState()
{
  mNavigationIndex = mContextDockWidget->addTab(mNavigationWidget,
      QString("Navigation"));

  mCurrentWorkflowState = NAVIGATION;
}
void MainWindow::deactivateNavigationState()
{
  if(mNavigationIndex != -1)
  {
    mContextDockWidget->removeTab(mNavigationIndex);
    mNavigationIndex = -1;
  }
}
void MainWindow::activateUSAcquisitionState()
{
  mCurrentWorkflowState = US_ACQUISITION;
}
void MainWindow::deactivateUSAcquisitionState()
{}
  
  
void MainWindow::aboutSlot()
{
  QMessageBox::about(this, tr("About CustusX"),
                     tr("<h2>CustusX version %1</h2> "
                        "<p>Created by SINTEF Medical Technology."
                        "<p><a href=http://www.sintef.no/Home/Technology-and-Society/Medical-technology> www.sintef.no </a>"
                        "<p>An application for Image Guided Surgery."
                        "<p>Created using Qt, VTK, ITK, IGSTK, SSC.").arg(VERSION_NUMBER));
}

void MainWindow::preferencesSlot()
{
  PreferencesDialog prefDialog(this);
  prefDialog.exec();
}
void MainWindow::quitSlot()
{
  messageManager()->sendInfo("quitSlot - never called?");
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
  
void MainWindow::deleteDataSlot()
{
  emit deleteCurrentImage();
}
void MainWindow::loadPatientRegistrationSlot()
{
  /*Expecting a file that looks like this
   *00 01 02 03
   *10 11 12 13
   *20 21 22 23
   *30 31 32 33
   */

  QString registrationFilePath = QFileDialog::getOpenFileName(this,
      tr("Select patient registration file (*.txt)"),
      mSettings->value("globalPatientDataFolder").toString(),
      tr("Patient registration files (*.txt)"));

  //Check that the file can be open and read
  QFile registrationFile(registrationFilePath);
  if(!registrationFile.open(QIODevice::ReadOnly))
  {
    messageManager()->sendWarning("Could not open "+registrationFilePath.toStdString()+".");
    return;
  }else
  {
    vtkMatrix4x4* matrix = vtkMatrix4x4::New();
    //read the content, 4x4 matrix
    QTextStream inStream(&registrationFile);
    for(int i=0; i<4; i++)
    {
      QString line = inStream.readLine();
      std::cout << line.toStdString() << std::endl;
      QStringList list = line.split(" ", QString::SkipEmptyParts);
      if(list.size() != 4)
      {
        messageManager()->sendError(""+registrationFilePath.toStdString()+" is not correctly formated");
        return;
      }
      matrix->SetElement(i,0,list[0].toDouble());
      matrix->SetElement(i,1,list[1].toDouble());
      matrix->SetElement(i,2,list[2].toDouble());
      matrix->SetElement(i,3,list[3].toDouble());
    }
    //set the toolmanageres matrix
    ssc::Transform3DPtr patientRegistration(new ssc::Transform3D(matrix));
    registrationManager()->setManualPatientRegistration(patientRegistration);
    //std::cout << (*patientRegistration.get()) << std::endl;
    messageManager()->sendInfo("New patient registration is set.");
  }
}
void MainWindow::configureSlot()
{
  QString configFile = mSettings->value("toolConfigFilePath").toString();

  if(mSettings->value("toolConfigFilePath").toString() ==
      QDir::homePath())
  {
    QString configFile = QFileDialog::getOpenFileName(this,
        tr("Select configuration file (*.xml)"),
        mSettings->value("toolConfigFilePath").toString(),
        tr("Configuration files (*.xml)"));
    mSettings->setValue("toolConfigFilePath", configFile);
    messageManager()->sendInfo("Tool configuration file is now selected: "+
                              configFile.toStdString());
  }
  toolManager()->setConfigurationFile(configFile.toStdString());

  QString loggingPath = mPatientData->getActivePatientFullPath()+"/Logs/";
  QDir loggingDir(loggingPath);
  if(!loggingDir.exists())
  {
    loggingDir.mkdir(loggingPath);
    messageManager()->sendInfo("Made a folder for logging: "+loggingPath.toStdString());
  }
  toolManager()->setLoggingFolder(loggingPath.toStdString());

  toolManager()->configure();
  
  // moved to viewwrapper3d:
//  if(toolManager()->isConfigured())
//  {
//    View3D* view = viewManager()->get3DView("View3D_1");
//
//    ToolRep3DMap* toolRep3DMap = repManager()->getToolRep3DReps();
//    ToolRep3DMap::iterator repIt = toolRep3DMap->begin();
//    ssc::ToolManager::ToolMapPtr configuredTools = toolManager()->getConfiguredTools();
//    ssc::ToolManager::ToolMap::iterator toolIt = configuredTools->begin();
//    while((toolIt != configuredTools->end()) && (repIt != toolRep3DMap->end()))
//    {
//      if(toolIt->second->getType() != ssc::Tool::TOOL_REFERENCE)
//      {
//        repIt->second->setTool(toolIt->second);
//        view->addRep(repIt->second);
//        repIt++;
//      }
//      toolIt++;
//    }
//  }
}
void MainWindow::loggingSlot(const QString& message, int timeout)
{
  //TODO Write to file and a "console" inside CX3 maybe?
  std::cout << message.toStdString() << std::endl;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  mSettings->setValue("mainWindow/geometry", saveGeometry());
  mSettings->setValue("mainWindow/windowState", saveState());
  mSettings->sync();
  messageManager()->sendInfo("Closing: Save geometry and window state");
  QMainWindow::closeEvent(event);
}
}//namespace cx
