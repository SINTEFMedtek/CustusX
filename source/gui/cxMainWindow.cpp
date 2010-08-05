#include "cxMainWindow.h"

#include <QtGui>
#include "sscTime.h"
#include "sscMessageManager.h"
#include "cxDataManager.h"
#include "cxViewManager.h"
#include "cxRepManager.h"
#include "cxToolManager.h"
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
#include "cxRegistrationHistoryWidget.h"
#include "cxDataLocations.h"
#include "cxMeshPropertiesWidget.h"
#include "cxLayoutEditor.h"

namespace cx
{

  
MainWindow::MainWindow() :
  mCurrentWorkflowState(PATIENT_DATA),
  mCentralWidget(new QWidget(this)),
  mContextDockWidget(new ContextDockWidget(this)),
  mImageRegistrationWidget(new ImageRegistrationWidget(NULL)),
  mPatientRegistrationWidget(new PatientRegistrationWidget(NULL)),
  mTransferFunctionWidget(new TransferFunctionWidget(mContextDockWidget)),
  mShiftCorrectionWidget(new ShiftCorrectionWidget(NULL)),
  mBrowserWidget(new BrowserWidget(mContextDockWidget)),
  mNavigationWidget(new NavigationWidget(NULL)),
  mCustomStatusBar(new CustomStatusBar()),
  mImagePropertiesWidget(new ImagePropertiesWidget(this)),
  mToolPropertiesWidget(new ToolPropertiesWidget(this)),
  mMeshPropertiesWidget(new MeshPropertiesWidget(this)),
  mPointSamplingWidget(new PointSamplingWidget(this)),
  mReconstructionWidget(new ssc::ReconstructionWidget(this, ssc::XmlOptionFile(DataLocations::getXmlSettingsFile(),"CustusX").descend("usReconstruction"), DataLocations::getShaderPath() )),
  mRegistrationHistoryWidget(new RegistrationHistoryWidget(this)),
  mImageRegistrationIndex(-1),
  mShiftCorrectionIndex(-1),
  mPatientRegistrationIndex(-1),
  mNavigationIndex(-1),
  mSettings(DataLocations::getSettings()),
  mPatientData(new PatientData(this)),
  mStateMachineManager(new StateMachineManager())
{
  mLayoutActionGroup = NULL;
#ifdef VERSION_NUMBER_VERBOSE
  this->setWindowTitle(QString("CustusX %1").arg(VERSION_NUMBER_VERBOSE));
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
  if (!mSettings->contains("globalApplicationName"))
    mSettings->setValue("globalApplicationName", "Lab");
  if (!mSettings->contains("globalPatientNumber"))
    mSettings->setValue("globalPatientNumber", 1);
  //if (!mSettings->contains("applicationNames"))
    mSettings->setValue("applicationNames", "Nevro,Lap,Vasc,Lung,Lab");
  
  
  if (!mSettings->contains("renderingInterval"))
    mSettings->setValue("renderingInterval", 33);
  if (!mSettings->contains("shadingOn"))
    mSettings->setValue("shadingOn", true);  

  //debugging
  connect(ssc::messageManager(), SIGNAL(emittedMessage(const QString&, int)),
          this, SLOT(loggingSlot(const QString&, int)));
  ssc::messageManager()->setCoutFlag(false);

  connect(mPatientData.get(), SIGNAL(patientChanged()), this, SLOT(patientChangedSlot()));

  // initialize toolmanager config file
  ToolManager::getInstance()->setConfigurationFile(string_cast(DataLocations::getToolConfigFilePath()));

  connect(viewManager(), SIGNAL(activeLayoutChanged()), this, SLOT(layoutChangedSlot()));
  this->layoutChangedSlot();

  this->changeState(PATIENT_DATA, PATIENT_DATA);

  this->addAsDockWidget(mImagePropertiesWidget);
  this->addAsDockWidget(mToolPropertiesWidget);
  this->addAsDockWidget(mMeshPropertiesWidget);
  this->addAsDockWidget(mPointSamplingWidget);
  this->addAsDockWidget(mReconstructionWidget);
  this->addAsDockWidget(mRegistrationHistoryWidget);
  
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
{
//  std::cout << "MainWindow::~MainWindow()" << std::endl;
}

void MainWindow::initialize()
{
  cx::DataManager::initialize();
  cx::ToolManager::initializeObject();
}

/** deallocate all global resources. Assumes MainWindow already has been destroyed and the mainloop is exited
 *
 */
void MainWindow::shutdown()
{
  ViewManager::destroyInstance();
  RegistrationManager::shutdown();
  RepManager::destroyInstance();
  cx::ToolManager::shutdown();
  cx::DataManager::shutdown();
}

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
  mDebugModeAction->setChecked(DataManager::getInstance()->getDebugMode());
  mDebugModeAction->setStatusTip(tr("Set debug mode, this enables lots of weird stuff."));

  mQuitAction = new QAction(tr("&Quit"), this);
  mQuitAction->setShortcut(tr("Ctrl+Q"));
  mQuitAction->setStatusTip(tr("Exit the application"));
  
  connect(mAboutAction, SIGNAL(triggered()), this, SLOT(aboutSlot()));
  connect(mPreferencesAction, SIGNAL(triggered()), this, SLOT(preferencesSlot()));
  connect(mDebugModeAction, SIGNAL(triggered(bool)), DataManager::getInstance(), SLOT(setDebugMode(bool)));
  connect(DataManager::getInstance(), SIGNAL(debugModeChanged(bool)), mDebugModeAction, SLOT(setChecked(bool)));
  connect(mQuitAction, SIGNAL(triggered()), this, SLOT(quitSlot()));
  
  //View
  this->mToggleContextDockWidgetAction = mContextDockWidget->toggleViewAction();
  mToggleContextDockWidgetAction->setText("Context Widget");
  
  //workflow
  /*mWorkflowActionGroup = new QActionGroup(this);
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
          this, SLOT(usAcquisitionWorkflowSlot()));*/

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
  //mStartTrackingToolsAction =  new QAction(tr("Start tracking"), mToolsActionGroup);
  //mStopTrackingToolsAction =  new QAction(tr("Stop tracking"), mToolsActionGroup);
  mTrackingToolsAction =  new QAction(tr("Start tracking"), mToolsActionGroup);
  mSaveToolsPositionsAction = new QAction(tr("Save positions"), this);

  mConfigureToolsAction->setChecked(true);

  connect(mConfigureToolsAction, SIGNAL(triggered()), this, SLOT(configureSlot()));
  connect(mInitializeToolsAction, SIGNAL(triggered()), ssc::toolManager(), SLOT(initialize()));
  connect(mTrackingToolsAction, SIGNAL(triggered()), this, SLOT(toggleTrackingSlot()));
  connect(mSaveToolsPositionsAction, SIGNAL(triggered()), ssc::toolManager(), SLOT(saveToolsSlot()));
  connect(ssc::toolManager(), SIGNAL(trackingStarted()), this, SLOT(updateTrackingActionSlot()));
  connect(ssc::toolManager(), SIGNAL(trackingStopped()), this, SLOT(updateTrackingActionSlot()));
  this->updateTrackingActionSlot();

  mNewLayoutAction = new QAction(tr("New Layout"), this);
  mNewLayoutAction->setToolTip("Create a new Custom Layout");
  connect(mNewLayoutAction, SIGNAL(triggered()), this, SLOT(newCustomLayoutSlot()));
  mEditLayoutAction = new QAction(tr("Edit Layout"), this);
  mEditLayoutAction->setToolTip("Edit the current Custom Layout");
  connect(mEditLayoutAction, SIGNAL(triggered()), this, SLOT(editCustomLayoutSlot()));
  mDeleteLayoutAction = new QAction(tr("Delete Layout"), this);
  mDeleteLayoutAction->setToolTip("Delete the current Custom Layout");
  connect(mDeleteLayoutAction, SIGNAL(triggered()), this, SLOT(deleteCustomLayoutSlot()));

  //context widgets
  this->addDockWidget(Qt::LeftDockWidgetArea, mContextDockWidget);


  mCenterToImageCenterAction = new QAction(tr("Center Image"), this);
  connect(mCenterToImageCenterAction, SIGNAL(triggered()), this, SLOT(centerToImageCenterSlot()));
  mCenterToTooltipAction = new QAction(tr("Center Tool"), this);
  connect(mCenterToTooltipAction, SIGNAL(triggered()), this, SLOT(centerToTooltipSlot()));


  //TODO remove
  /*connect(mContextDockWidget, SIGNAL(currentImageChanged(ssc::ImagePtr)),
          viewManager(), SLOT(currentImageChangedSlot(ssc::ImagePtr)));*/

  connect(ssc::dataManager(), SIGNAL(activeImageChanged(std::string)),
          mImageRegistrationWidget, SLOT(activeImageChangedSlot()));
  connect(ssc::dataManager(), SIGNAL(activeImageChanged(std::string)),
          mPatientRegistrationWidget, SLOT(activeImageChangedSlot()));
  connect(ssc::dataManager(), SIGNAL(activeImageChanged(std::string)),
          mTransferFunctionWidget, SLOT(activeImageChangedSlot()));
  
  connect(this, SIGNAL(deleteCurrentImage()),
          mContextDockWidget, SLOT(deleteCurrentImageSlot()));
}

void MainWindow::centerToImageCenterSlot()
{
  Navigation().centerToImage(ssc::dataManager()->getActiveImage());
}

void MainWindow::centerToTooltipSlot()
{
  Navigation().centerToTooltip();
}

void MainWindow::updateTrackingActionSlot()
{
  if (ssc::toolManager()->isTracking())
    mTrackingToolsAction->setText("Stop Tracking");
  else
    mTrackingToolsAction->setText("Start Tracking");
}

void MainWindow::toggleTrackingSlot()
{
 if (ssc::toolManager()->isTracking())
   ssc::toolManager()->stopTracking();
 else
   ssc::toolManager()->startTracking();
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
    ssc::messageManager()->sendInfo("Made a new patient folder: "+patientDatafolder.toStdString());
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
    ssc::messageManager()->sendWarning("No patient selected, select or create patient before saving!");
    this->newPatientSlot();
    return;
  }

  mPatientData->savePatient();
}

void MainWindow::loadPatientFileSlot()
{
  
  QString patientDatafolder = mSettings->value("globalPatientDataFolder").toString();
  // Create folder
  if(!QDir().exists(patientDatafolder))
  {
    QDir().mkdir(patientDatafolder);
    ssc::messageManager()->sendInfo("Made a new patient folder: "+patientDatafolder.toStdString());
  }
  // Open file dialog
  QString choosenDir = QFileDialog::getExistingDirectory(this, tr("Select patient"),
                                                         patientDatafolder,
                                                         QFileDialog::ShowDirsOnly);
  if (choosenDir == QString::null)
    return; // On cancel

  mPatientData->loadPatient(choosenDir);
}

void MainWindow::importDataSlot()
{
  this->savePatientFileSlot();

  ssc::messageManager()->sendInfo("Importing data...");
  QString fileName = QFileDialog::getOpenFileName( this,
                                  QString(tr("Select data file for import")),
                                  mSettings->value("globalPatientDataFolder").toString(),
                                  tr("Image/Mesh (*.mhd *.mha *.stl *.vtk)"));
  if(fileName.isEmpty())
  {
    ssc::messageManager()->sendInfo("Import canceled");
    return;
  }

  mPatientData->importData(fileName);
}

void MainWindow::patientChangedSlot()
{
  mReconstructionWidget->selectData(mPatientData->getActivePatientFolder()+"/US_Acq/");
  mReconstructionWidget->reconstructer()->setOutputBasePath(mPatientData->getActivePatientFolder());
  mReconstructionWidget->reconstructer()->setOutputRelativePath("Images");

  QString loggingPath = mPatientData->getActivePatientFolder()+"/Logs/";
  QDir loggingDir(loggingPath);
  if(!loggingDir.exists())
  {
    loggingDir.mkdir(loggingPath);
    ssc::messageManager()->sendInfo("Made a folder for tool logging: "+loggingPath.toStdString());
  }
  ToolManager::getInstance()->setLoggingFolder(loggingPath.toStdString());
}

/** Called when the layout is changed: update the layout menu
 */
void MainWindow::layoutChangedSlot()
{
  // reset list of available layouts
  delete mLayoutActionGroup;
  mLayoutActionGroup = viewManager()->createLayoutActionGroup();

  mLayoutMenu->addActions(mLayoutActionGroup->actions());

  bool editable = viewManager()->isCustomLayout(viewManager()->getActiveLayout());
  mEditLayoutAction->setEnabled(editable);
  mDeleteLayoutAction->setEnabled(editable);
}

/**create and execute a dialog for determining layout.
 * Return layout data, or invalid layout data if cancelled.
 */
LayoutData MainWindow::executeLayoutEditorDialog(QString title, bool createNew)
{
  boost::shared_ptr<QDialog> dialog(new QDialog(NULL, Qt::Dialog));
  dialog->setWindowTitle(title);
  QVBoxLayout* layout = new QVBoxLayout(dialog.get());
  layout->setMargin(0);

  LayoutEditor* editor = new LayoutEditor(dialog.get());

  LayoutData data = viewManager()->getLayoutData(viewManager()->getActiveLayout());
//  std::cout << "AA:" << streamXml2String(data) << std::endl;
  if (createNew)
  {
    data.resetUid(viewManager()->generateLayoutUid());
  }
  editor->setLayoutData(data);
  layout->addWidget(editor);

  QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), dialog.get(), SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), dialog.get(), SLOT(reject()));
  layout->addWidget(buttonBox);

  if (!dialog->exec())
    return LayoutData();

  return editor->getLayoutData();
}

void MainWindow::newCustomLayoutSlot()
{
  LayoutData data = this->executeLayoutEditorDialog("New Custom Layout", true);
  if (data.getUid().isEmpty())
    return;
  viewManager()->setLayoutData(data);
  viewManager()->setActiveLayout(data.getUid());
}

void MainWindow::editCustomLayoutSlot()
{
  LayoutData data = this->executeLayoutEditorDialog("Edit Current Layout", false);
  if (data.getUid().isEmpty())
    return;
  viewManager()->setLayoutData(data);
}

void MainWindow::deleteCustomLayoutSlot()
{
  if (QMessageBox::question(this,
      "Delete current layout",
      "Do you really want to delete the current layout?",
      QMessageBox::Cancel | QMessageBox::Ok) != QMessageBox::Ok)
    return;
  viewManager()->deleteLayoutData(viewManager()->getActiveLayout());
  viewManager()->setActiveLayout(viewManager()->getAvailableLayouts().front()); // revert to existing state
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
  /*
  mWorkflowMenu->addAction(mPatientDataWorkflowAction);
  mWorkflowMenu->addAction(mImageRegistrationWorkflowAction);
  mWorkflowMenu->addAction(mPatientRegistrationWorkflowAction);
  mWorkflowMenu->addAction(mNavigationWorkflowAction);
  mWorkflowMenu->addAction(mUSAcquisitionWorkflowAction);*/
  mStateMachineManager->getWorkflow()->fillMenu(mWorkflowMenu);

  //data
  this->menuBar()->addMenu(mDataMenu);
  mDataMenu->addAction(mImportDataAction);
  mDataMenu->addAction(mDeleteDataAction);
  mDataMenu->addAction(mLoadPatientRegistrationFromFile);

  //tool
  this->menuBar()->addMenu(mToolMenu);
  mToolMenu->addAction(mConfigureToolsAction);
  mToolMenu->addAction(mInitializeToolsAction);
  mToolMenu->addAction(mTrackingToolsAction);
  //mToolMenu->addAction(mStopTrackingToolsAction);
  mToolMenu->addSeparator();
  mToolMenu->addAction(mSaveToolsPositionsAction);

  //layout
  this->menuBar()->addMenu(mLayoutMenu);
//  mLayoutMenu->addActions(mLayoutActionGroup->actions());
//  mLayoutSeparator = mLayoutMenu->addSeparator();
  mLayoutMenu->addAction(mNewLayoutAction);
  mLayoutMenu->addAction(mEditLayoutAction);
  mLayoutMenu->addAction(mDeleteLayoutAction);
  mLayoutMenu->addSeparator();
}

void MainWindow::createToolBars()
{
  mDataToolBar = addToolBar("Data");
  mDataToolBar->setObjectName("DataToolBar");
  mDataToolBar->addAction(mImportDataAction);

  mToolToolBar = addToolBar("Tools");
  mToolToolBar->setObjectName("ToolToolBar");
  mToolToolBar->addAction(mTrackingToolsAction);
  //mToolToolBar->addAction(mStopTrackingToolsAction);

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
    ssc::messageManager()->sendWarning("Could not determine what workflow state to deactivate.");
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
    ssc::messageManager()->sendWarning("Could not determine what workflow state to activate.");
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
  QString imagesPath = mPatientData->getActivePatientFolder()+"/Images";
  mShiftCorrectionWidget->init(imagesPath);
  //TODO: Finish ShiftCorrection
  //Don't show ShiftCorrection in release
  //mShiftCorrectionIndex = mContextDockWidget->addTab(mShiftCorrectionWidget, 
  //    QString("Shift correction"));
  mImageRegistrationIndex = mContextDockWidget->addTab(mImageRegistrationWidget,
      QString("Image Registration"));
  
  viewManager()->setRegistrationMode(ssc::rsIMAGE_REGISTRATED);

  ssc::ProbeRepPtr probeRep = repManager()->getProbeRep("ProbeRep_1");

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

    disconnect(mImageRegistrationWidget, SIGNAL(thresholdChanged(const int)),
            probeRep.get(), SLOT(setThresholdSlot(const int)));
  }
}
void MainWindow::activatePatientRegistrationState()
{
  mPatientRegistrationIndex = mContextDockWidget->addTab(mPatientRegistrationWidget,
      QString("Patient Registration"));
  
  viewManager()->setRegistrationMode(ssc::rsPATIENT_REGISTRATED);
  
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
                        "<p>Created using Qt, VTK, ITK, IGSTK, SSC.").arg(VERSION_NUMBER_VERBOSE));
}

void MainWindow::preferencesSlot()
{
  PreferencesDialog prefDialog(this);
  prefDialog.exec();
}
void MainWindow::quitSlot()
{
  ssc::messageManager()->sendInfo("quitSlot - never called?");
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
    ssc::messageManager()->sendWarning("Could not open "+registrationFilePath.toStdString()+".");
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
        ssc::messageManager()->sendError(""+registrationFilePath.toStdString()+" is not correctly formated");
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
    ssc::messageManager()->sendInfo("New patient registration is set.");
  }
}
void MainWindow::configureSlot()
{
//  QString configFile = mSettings->value("toolConfigFilePath").toString();
//  QFileInfo info(configFile);

  /*if (!info.exists() || info.isDir())
  {
    configFile = QFileDialog::getOpenFileName(this,
        tr("Select configuration file (*.xml)"),
        mSettings->value("toolConfigFilePath").toString(),
        tr("Configuration files (*.xml)"));
    mSettings->setValue("toolConfigFilePath", configFile);
    ssc::messageManager()->sendInfo("Tool configuration file is now selected: "+
                              configFile.toStdString());
  }*/
//  toolManager()->setConfigurationFile(configFile.toStdString());
//  toolManager()->setConfigurationFile(string_cast(mSettings->value("toolConfigFilePath").toString()));

/*  QString loggingPath = mPatientData->getActivePatientFolder()+"/Logs/";
  QDir loggingDir(loggingPath);
  if(!loggingDir.exists())
  {
    loggingDir.mkdir(loggingPath);
    ssc::messageManager()->sendInfo("Made a folder for logging: "+loggingPath.toStdString());
  }
  toolManager()->setLoggingFolder(loggingPath.toStdString());*/

  ssc::toolManager()->configure();
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
  ssc::messageManager()->sendInfo("Closing: Save geometry and window state");
  
  if (ssc::toolManager()->isTracking())
  {
    ssc::messageManager()->sendInfo("Closing: Stopping tracking");
    ssc::toolManager()->stopTracking();
  }
  QMainWindow::closeEvent(event);
}
}//namespace cx
