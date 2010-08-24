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
#include "cxVolumePropertiesWidget.h"
#include "cxBrowserWidget.h"
#include "cxConsoleWidget.h"
#include "cxNavigationWidget.h"
//#include "cxTransferFunctionWidget.h"
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
#include "UsConfigGui.h"
namespace cx
{

  
MainWindow::MainWindow() :
  mCentralWidget(new QWidget(this)),
  mToggleWidgetActionGroup(NULL),
  mImageRegistrationWidget(new ImageRegistrationWidget(this)),
  mPatientRegistrationWidget(new PatientRegistrationWidget(this)),
  //mTransferFunctionWidget(new TransferFunctionWidget(this)),
  mShiftCorrectionWidget(new ShiftCorrectionWidget(this)),
  mBrowserWidget(new BrowserWidget(this)),
  mNavigationWidget(new NavigationWidget(this)),
  mImagePropertiesWidget(new ImagePropertiesWidget(this)),
  mToolPropertiesWidget(new ToolPropertiesWidget(this)),
  mMeshPropertiesWidget(new MeshPropertiesWidget(this)),
  mPointSamplingWidget(new PointSamplingWidget(this)),
  mReconstructionWidget(new ssc::ReconstructionWidget(this, ssc::XmlOptionFile(DataLocations::getXmlSettingsFile(),"CustusX").descend("usReconstruction"), DataLocations::getShaderPath() )),
  mRegistrationHistoryWidget(new RegistrationHistoryWidget(this)),
  mVolumePropertiesWidget(new VolumePropertiesWidget(this)),
  mProbePropertiesWidget(new UsConfigGui(this)),
  mConsoleWidget(new ConsoleWidget(this)),
  mCustomStatusBar(new CustomStatusBar()),
  mSettings(DataLocations::getSettings())
{
  connect(stateManager()->getApplication().get(), SIGNAL(activeStateChanged()), this, SLOT(onApplicationStateChangedSlot()));
  connect(stateManager()->getWorkflow().get(), SIGNAL(activeStateChanged()), this, SLOT(onWorkflowStateChangedSlot()));

  mLayoutActionGroup = NULL;
  this->updateWindowTitle();
  
  this->setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);
  //make sure the transferefunctionwidget it fully initialized
//  mTransferFunctionWidget->init();

  this->addAsDockWidget(mImagePropertiesWidget);
  this->addAsDockWidget(mToolPropertiesWidget);
  this->addAsDockWidget(mMeshPropertiesWidget);
  this->addAsDockWidget(mPointSamplingWidget);
  this->addAsDockWidget(mReconstructionWidget);
  this->addAsDockWidget(mRegistrationHistoryWidget);
  this->addAsDockWidget(mBrowserWidget);
  //this->addAsDockWidget(mTransferFunctionWidget);
  this->addAsDockWidget(mShiftCorrectionWidget);
  this->addAsDockWidget(mImageRegistrationWidget);
  this->addAsDockWidget(mPatientRegistrationWidget);
  this->addAsDockWidget(mNavigationWidget);
  this->addAsDockWidget(mVolumePropertiesWidget);
  this->addAsDockWidget(mProbePropertiesWidget);
  this->addAsDockWidget(mConsoleWidget);

  this->createActions();
  this->createToolBars();
  this->createMenus();
  this->createStatusBar();

  this->setCentralWidget(viewManager()->stealCentralWidget());
  
//  // Initialize settings if empty
//  if (!mSettings->contains("globalPatientDataFolder"))
//    mSettings->setValue("globalPatientDataFolder", QDir::homePath()+"/Patients");
//  if (!mSettings->contains("globalApplicationName"))
//    mSettings->setValue("globalApplicationName", "Lab");
//  if (!mSettings->contains("globalPatientNumber"))
//    mSettings->setValue("globalPatientNumber", 1);
//  //if (!mSettings->contains("applicationNames"))
//    mSettings->setValue("applicationNames", "Nevro,Lap,Vasc,Lung,Lab");
  
  
  if (!mSettings->contains("renderingInterval"))
    mSettings->setValue("renderingInterval", 33);
  if (!mSettings->contains("shadingOn"))
    mSettings->setValue("shadingOn", true);  

  //debugging
  connect(ssc::messageManager(), SIGNAL(emittedMessage(const QString&, int)),
          this, SLOT(loggingSlot(const QString&, int)));
  ssc::messageManager()->setCoutFlag(false);
  //

  connect(stateManager()->getPatientData().get(), SIGNAL(patientChanged()), this, SLOT(patientChangedSlot()));


  // initialize toolmanager config file
  ToolManager::getInstance()->setConfigurationFile(string_cast(DataLocations::getToolConfigFilePath()));

  connect(viewManager(), SIGNAL(activeLayoutChanged()), this, SLOT(layoutChangedSlot()));
  this->layoutChangedSlot();

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
  if(!mToggleWidgetActionGroup)
  {
    mToggleWidgetActionGroup = new QActionGroup(this);
    mToggleWidgetActionGroup->setExclusive(false);
  }

  QDockWidget* dockWidget = new QDockWidget(widget->windowTitle(), this);
  dockWidget->setObjectName(widget->objectName()+"DockWidget");
  dockWidget->setWidget(widget);
  this->addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
  dockWidget->setVisible(false); // default visibility

  mToggleWidgetActionGroup->addAction(dockWidget->toggleViewAction());
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
  StateManager::destroyInstance();
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
  mClearPatientAction = new QAction(tr("&Clear Patient"), this);
  
  connect(mNewPatientAction, SIGNAL(triggered()), this, SLOT(newPatientSlot()));
  connect(mLoadFileAction, SIGNAL(triggered()), this, SLOT(loadPatientFileSlot()));
  connect(mSaveFileAction, SIGNAL(triggered()), this, SLOT(savePatientFileSlot()));
  connect(mSaveFileAction, SIGNAL(triggered()), this, SLOT(saveDesktopSlot()));
  connect(mClearPatientAction, SIGNAL(triggered()), this, SLOT(clearPatientSlot()));

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
//  connect(ssc::dataManager(), SIGNAL(activeImageChanged(std::string)),
//          mTransferFunctionWidget, SLOT(activeImageChangedSlot()));
//
  //TODO what to do instead?
  //connect(this, SIGNAL(deleteCurrentImage()), mContextDockWidget, SLOT(deleteCurrentImageSlot()));

  mSaveDesktopAction = new QAction(tr("Save desktop"), this);
  mSaveDesktopAction->setToolTip("Save desktop for workflow step");
  connect(mSaveDesktopAction, SIGNAL(triggered()), this, SLOT(saveDesktopSlot()));

  mResetDesktopAction = new QAction(tr("Reset desktop"), this);
  mResetDesktopAction->setToolTip("Reset desktop for workflow step");
  connect(mResetDesktopAction, SIGNAL(triggered()), this, SLOT(resetDesktopSlot()));
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
  stateManager()->getPatientData()->newPatient(choosenDir);
}

void MainWindow::clearPatientSlot()
{
  stateManager()->getPatientData()->clearPatient();
  ssc::messageManager()->sendWarning("Cleared current patient data");
}

void MainWindow::savePatientFileSlot()
{
  if(stateManager()->getPatientData()->getActivePatientFolder().isEmpty())
  {
    ssc::messageManager()->sendWarning("No patient selected, select or create patient before saving!");
    this->newPatientSlot();
    return;
  }

  stateManager()->getPatientData()->savePatient();
}

void MainWindow::onApplicationStateChangedSlot()
{
  this->updateWindowTitle();
}

void MainWindow::updateWindowTitle()
{
  QString appName;
  if (stateManager()->getApplication())
    appName = stateManager()->getApplication()->getActiveStateName();

  QString versionName;
#ifdef VERSION_NUMBER_VERBOSE
  versionName = QString("%1").arg(VERSION_NUMBER_VERBOSE);
#else
#endif

  this->setWindowTitle("CustusX "+versionName + " - " + appName);
}


void MainWindow::onWorkflowStateChangedSlot()
{
  Desktop desktop = stateManager()->getActiveDesktop();
  this->restoreState(desktop.mMainWindowState);
  viewManager()->setActiveLayout(desktop.mLayoutUid);

  std::cout << "LayoutUid: " << desktop.mLayoutUid << std::endl;
}

void MainWindow::saveDesktopSlot()
{
  Desktop desktop;
  //std::cout << "QByteArray" << QString(this->saveState().toBase64()) << std::endl;
  desktop.mMainWindowState = this->saveState();
  desktop.mLayoutUid = viewManager()->getActiveLayout();
  stateManager()->saveDesktop(desktop);
}

void MainWindow::resetDesktopSlot()
{
  stateManager()->resetDesktop();
  this->onWorkflowStateChangedSlot();
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

  stateManager()->getPatientData()->loadPatient(choosenDir);
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

  stateManager()->getPatientData()->importData(fileName);
}

void MainWindow::patientChangedSlot()
{
  mReconstructionWidget->selectData(stateManager()->getPatientData()->getActivePatientFolder()+"/US_Acq/");
  mReconstructionWidget->reconstructer()->setOutputBasePath(stateManager()->getPatientData()->getActivePatientFolder());
  mReconstructionWidget->reconstructer()->setOutputRelativePath("Images");

  QString loggingPath = stateManager()->getPatientData()->getActivePatientFolder()+"/Logs/";
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
  mFileMenu->addAction(mClearPatientAction);
  mFileMenu->addSeparator();
  mFileMenu->addAction(mDebugModeAction);

  // View
  QMenu* popupMenu = this->createPopupMenu();
  popupMenu->setTitle("Window");
  this->menuBar()->addMenu(popupMenu);
  
  //workflow
  this->menuBar()->addMenu(mWorkflowMenu);
  stateManager()->getWorkflow()->fillMenu(mWorkflowMenu);

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

  mWorkflowToolBar = addToolBar("Workflow");
  mWorkflowToolBar->setObjectName("WorkflowToolBar");
  stateManager()->getWorkflow()->fillToolBar(mWorkflowToolBar);

  mDesktopToolBar = addToolBar("Desktop");
  mDesktopToolBar->setObjectName("DesktopToolBar");
  mDesktopToolBar->addAction(mSaveDesktopAction);
  mDesktopToolBar->addAction(mResetDesktopAction);

}
void MainWindow::createStatusBar()
{
  //TODO, not working as intended
  this->setStatusBar(mCustomStatusBar);
}
  
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
