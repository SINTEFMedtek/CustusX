#include "cxMainWindow.h"

#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QFileDialog>
#include <QStatusBar>
#include <QFileInfo>
#include <QDomDocument>
#include <QTextStream>
#include <QSettings>
#include <QDateTime>
#include "cxDataManager.h"
#include "cxViewManager.h"
#include "cxRepManager.h"
#include "cxToolManager.h"
#include "cxMessageManager.h"
#include "cxRegistrationManager.h"
#include "cxCustomStatusBar.h"
#include "cxContextDockWidget.h"
#include "cxTransferFunctionWidget.h"
#include "cxImageRegistrationWidget.h"
#include "cxPatientRegistrationWidget.h"
#include "cxView3D.h"
#include "cxView2D.h"
#include "cxPreferencesDialog.h"

namespace cx
{
MainWindow::MainWindow() :
  mCurrentWorkflowState(PATIENT_DATA),
  mViewManager(ViewManager::getInstance()),
  mDataManager(DataManager::getInstance()),
  mToolManager(ToolManager::getInstance()),
  mRepManager(RepManager::getInstance()),
  mMessageManager(MessageManager::getInstance()),
  mRegistrationManager(RegistrationManager::getInstance()),
  mCentralWidget(new QWidget(this)),
  mContextDockWidget(new ContextDockWidget(this)),
  mImageRegistrationWidget(new ImageRegistrationWidget(mContextDockWidget)),
  mPatientRegistrationWidget(new PatientRegistrationWidget(mContextDockWidget)),
  mTransferFunctionWidget(new TransferFunctionWidget(mContextDockWidget)),
  mCustomStatusBar(new CustomStatusBar()),
  mImageRegistrationIndex(-1),
  mPatientRegistrationIndex(-1),
  mSettings(new QSettings()),
  mActivePatientFolder("")
{  
  this->createActions();
  this->createToolBars();
  this->createMenus();
  this->createStatusBar();

  this->setCentralWidget(mViewManager->stealCentralWidget());
  this->resize(QSize(1000,1000));
  
  // Initialize settings if empty
  if (!mSettings->contains("globalPatientDataFolder"))
    mSettings->setValue("globalPatientDataFolder", QDir::homePath());
  //if (!mSettings->contains("mainWindow/importDataFolder"))
  //  mSettings->setValue("mainWindow/importDataFolder", ".");
  if (!mSettings->contains("toolConfigFilePath"))
    mSettings->setValue("toolConfigFilePath", QDir::homePath());
  
  if (!mSettings->contains("globalApplicationName"))
    mSettings->setValue("globalApplicationName", "Nevro");
  if (!mSettings->contains("globalPatientNumber"))
    mSettings->setValue("globalPatientNumber", 1);
  
  //debugging
  connect(mMessageManager, SIGNAL(emittedMessage(const QString&, int)),
          this, SLOT(printSlot(const QString&, int)));

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
  
  // Don't show the Widget before all elements are initialized
  this->show();
}
MainWindow::~MainWindow()
{}
void MainWindow::createActions()
{
  //TODO: add shortcuts and tooltips
	
  // File
  mNewPatientAction = new QAction(tr("Create new patient"), this);
  mSaveFileAction = new QAction(tr("Save Patient file"), this);
  mLoadFileAction = new QAction(tr("Load Patient file"), this);
  
  connect(mNewPatientAction, SIGNAL(triggered()),
          this, SLOT(newPatientSlot()));
  connect(mLoadFileAction, SIGNAL(triggered()),
          this, SLOT(loadPatientFileSlot()));
  connect(mSaveFileAction, SIGNAL(triggered()),
          this, SLOT(savePatientFileSlot()));

  // Application
  mAboutAction = new QAction(tr("A&bout"), this);  // About burde gitt About CustusX, det gj√∏r det ikke av en eller annen grunn???
  mAboutAction->setShortcut(tr("Ctrl+A"));
  mAboutAction->setStatusTip(tr("Show the application's About box"));
  mPreferencesAction = new QAction(tr("P&references"), this);
  mPreferencesAction->setShortcut(tr("Ctrl+P"));
  mPreferencesAction->setStatusTip(tr("Show the preferences dialog"));
  mQuitAction = new QAction(tr("Q&uit"), this);
  mQuitAction->setShortcut(tr("Ctrl+Q"));
  mQuitAction->setStatusTip(tr("Exit the application"));
  
  connect(mAboutAction, SIGNAL(triggered()), this, SLOT(aboutSlot()));
  connect(mPreferencesAction, SIGNAL(triggered()), this, SLOT(preferencesSlot()));
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
  mImportDataAction->setShortcut(tr("Ctrl+Is"));
  mImportDataAction->setStatusTip(tr("Import image data"));

  connect(mImportDataAction, SIGNAL(triggered()),
          this, SLOT(importDataSlot()));

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
          mToolManager, SLOT(initialize()));
  connect(mStartTrackingToolsAction, SIGNAL(triggered()),
          mToolManager, SLOT(startTracking()));
  connect(mStopTrackingToolsAction, SIGNAL(triggered()),
          mToolManager, SLOT(stopTracking()));
  connect(mSaveToolsPositionsAction, SIGNAL(triggered()), 
          mToolManager, SLOT(saveToolsSlot()));

  //layout
  mLayoutActionGroup = new QActionGroup(this);
  mLayoutActionGroup->setExclusive(true);
  m3D_1x1_LayoutAction = new QAction(tr("3D_1X1"), mLayoutActionGroup);
  m3DACS_2x2_LayoutAction = new QAction(tr("3DACS_2X2"), mLayoutActionGroup);
  m3DACS_1x3_LayoutAction = new QAction(tr("3DACS_1X3"), mLayoutActionGroup);
  mACSACS_2x3_LayoutAction = new QAction(tr("ACSACS_2X3"), mLayoutActionGroup);
  
  m3D_1x1_LayoutAction->setCheckable(true);
  m3DACS_2x2_LayoutAction->setCheckable(true);
  m3DACS_1x3_LayoutAction->setCheckable(true);
  mACSACS_2x3_LayoutAction->setCheckable(true);

  m3DACS_2x2_LayoutAction->setChecked(true);

  connect(m3D_1x1_LayoutAction, SIGNAL(triggered()),
      mViewManager, SLOT(setLayoutTo_3D_1X1()));
  connect(m3DACS_2x2_LayoutAction, SIGNAL(triggered()),
      mViewManager, SLOT(setLayoutTo_3DACS_2X2()));
  connect(m3DACS_1x3_LayoutAction, SIGNAL(triggered()),
      mViewManager, SLOT(setLayoutTo_3DACS_1X3()));
  connect(mACSACS_2x3_LayoutAction, SIGNAL(triggered()),
      mViewManager, SLOT(setLayoutTo_ACSACS_2X3()));
  
  connect(mContextDockWidget, SIGNAL(currentImageChanged(ssc::ImagePtr)),
          mViewManager, SLOT(currentImageChangedSlot(ssc::ImagePtr)));

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
  mCustusXMenu = new QMenu(tr("CustusX"), this);;
	mFileMenu = new QMenu(tr("File"), this);;
  mViewMenu = new QMenu(tr("View"), this);;
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
	
  // View
  this->menuBar()->addMenu(mViewMenu);
  mViewMenu->addAction(mToggleContextDockWidgetAction);
  
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

  //tool
  this->menuBar()->addMenu(mToolMenu);
  mToolMenu->addAction(mConfigureToolsAction);
  mToolMenu->addAction(mInitializeToolsAction);
  mToolMenu->addAction(mStartTrackingToolsAction);
  mToolMenu->addAction(mStopTrackingToolsAction);
  mToolMenu->addSeparator();
  mToolMenu->addAction(mSaveToolsPositionsAction);

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
  mDataToolBar->addAction(mImportDataAction);

  mToolToolBar = addToolBar("Tools");
  mToolToolBar->addAction(mStartTrackingToolsAction);
  mToolToolBar->addAction(mStopTrackingToolsAction);

}
void MainWindow::createStatusBar()
{
  //TODO, not working as intended
  this->setStatusBar(mCustomStatusBar);
}
void MainWindow::generateSaveDoc(QDomDocument& doc)
{
  doc.appendChild(doc.createProcessingInstruction("xml version =", "'1.0'"));
  QDomElement managerNode = doc.createElement("managers");
  doc.appendChild(managerNode);

  mDataManager->addXml(managerNode);

  //TODO Implement
  /*mToolManager->getXml(doc); //TODO
  mMessageManager->getXml(doc); //TODO
  mViewManager->getXml(doc); //TODO
  mRepManager->getXml(doc); //TODO
  mRegistrationManager->getXml(doc);*/

}
void MainWindow::readLoadDoc(QDomDocument& doc)
{
  //Get all the nodes
  QDomNode managerNode = doc.namedItem("managers");
  QDomNode dataManagerNode = managerNode.namedItem("datamanager");

  //Evaluate the xml nodes and load what's needed
  if (!dataManagerNode.isNull())
  {
    mDataManager->parseXml(dataManagerNode);
  }
  else
    mMessageManager->sendWarning("cx::MainWindow::readLoadDoc(): No DataManager node");
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
  
  ssc::ProbeRepPtr probeRep = mRepManager->getProbeRep("ProbeRep_1");
  LandmarkRepPtr landmarkRep = mRepManager->getLandmarkRep("LandmarkRep_1");
  mViewManager->get3DView("View3D_1")->addRep(landmarkRep);
  mViewManager->get3DView("View3D_1")->addRep(probeRep);
  
  mCurrentWorkflowState = IMAGE_REGISTRATION;
}
void MainWindow::deactivateImageRegistationState()
{
  if(mImageRegistrationIndex != -1)
  {
    mContextDockWidget->removeTab(mImageRegistrationIndex);
    mImageRegistrationIndex = -1;
    
    ssc::ProbeRepPtr probeRep = mRepManager->getProbeRep("ProbeRep_1");
    LandmarkRepPtr landmarkRep = mRepManager->getLandmarkRep("LandmarkRep_1");
    mViewManager->get3DView("View3D_1")->removeRep(landmarkRep);
    mViewManager->get3DView("View3D_1")->removeRep(probeRep);
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
//  QMessageBox::about(this, tr("About CustusX"),
//                     tr("<b>CustusX</b> is an application for IGS"));
}
void MainWindow::preferencesSlot()
{
  PreferencesDialog prefDialog(this);
  prefDialog.exec();
}
void MainWindow::quitSlot()
{
  //TODO
}  
  
void MainWindow::newPatientSlot()
{  
  QString patientDatafolder = mSettings->value("globalPatientDataFolder").toString();
  QString name = QDateTime::currentDateTime().toString("yyyyMMdd'T'hhmmss");
  name += "_";
  name += mSettings->value("globalApplicationName").toString();
  name += "_";
  name += mSettings->value("globalPatientNumber").toString();
  
  QString choosenDir = patientDatafolder;
  choosenDir += "/";
  choosenDir += name;
  // Open file dialog, get patient data folder
  choosenDir = QFileDialog::getSaveFileName(this, 
                                            tr("Select directory to save file in"),
                                            choosenDir);
  if (choosenDir == QString::null)
    return; // On cancel
  
  // Update global patient number
  int patientNumber = mSettings->value("globalPatientNumber").toInt();
  mSettings->setValue("globalPatientNumber", ++patientNumber);
  
  if (!choosenDir.endsWith(".cx3"))
    choosenDir.append(".cx3");
  
  // Set active patient folder. Use path relative to the globalPatientDataFolder
  QDir patientDataDir(patientDatafolder);
  mActivePatientFolder = patientDataDir.relativeFilePath(choosenDir);
  
  // Create folders
  if(!QDir().exists(choosenDir))
    QDir().mkdir(choosenDir);
  
  QString newDir = choosenDir;
  newDir.append("/Images"); 
  if(!QDir().exists(newDir))
    QDir().mkdir(newDir);
  
  newDir = choosenDir;
  newDir.append("/Logs"); 
  if(!QDir().exists(newDir))
    QDir().mkdir(newDir);
}
  
void MainWindow::loadPatientFileSlot()
{
  // Open file dialog
  QString choosenDir = QFileDialog::getExistingDirectory(this, tr("Open directory"),
                                                         mSettings->value("globalPatientDataFolder").toString(),
                                                         QFileDialog::ShowDirsOnly);
  if (choosenDir == QString::null)
    return; // On cancel
  
  // Set active patient folder, relative to globalPatientDataFolder
  QDir patientDataDir(mSettings->value("globalPatientDataFolder").toString());
  mActivePatientFolder = patientDataDir.relativeFilePath(choosenDir);
  
  QFile file(choosenDir + "/custusdoc.xml");
  if(file.open(QIODevice::ReadOnly))
  {    
    QDomDocument doc;
    QString emsg;
    int eline, ecolumn;
    // Read the file
    if (!doc.setContent(&file, false, &emsg, &eline, &ecolumn))
    {
      std::cout << "ERROR! MainWindow::loadPatientFileSlot(): Could not parse XML file:";
      std::cout << emsg.toStdString() << "line: "<< eline << "col: " << ecolumn;
      std::cout << std::endl;
      throw "Could not parse XML file";
    }
    file.close();

    //Read the xml
    this->readLoadDoc(doc);
  }
}
void MainWindow::savePatientFileSlot()
{
  // Open file dialog, get patient data folder
  /*QString dir = QFileDialog::getSaveFileName(this, 
                                             tr("Select directory to save file in"),
                                             mSettings->value("globalPatientDataFolder").toString()
                                             );
  if (dir == QString::null)
    return; // On cancel
  if (!dir.endsWith(".cx3"))
    dir.append(".cx3");
  if(!QDir().exists(dir))
    QDir().mkdir(dir);*/
  
  if(mActivePatientFolder.isEmpty())
  {
    mMessageManager->sendWarning("cx::MainWindow::savePatientFileSlot(): No Patient created!");
    return;
  }
  
  //Gather all the information that needs to be saved
  QDomDocument* doc(new QDomDocument());
  this->generateSaveDoc(*doc);

  QString activePatientDir = mSettings->value("globalPatientDataFolder").toString();
  activePatientDir += mActivePatientFolder;
  QFile file(activePatientDir + "/custusdoc.xml");
  if(file.open(QIODevice::WriteOnly))
  {
    QTextStream stream(&file);
    stream << doc->toString();
    file.close();
  }

  //Write the data to file
  //TODO Implement when we know what we want to save here...
  /*if(this->write(dir))
  {
    QFile file(dir + "/custusdoc.xml");
    if(file.open(QIODevice::WriteOnly))
    {
      QTextStream stream(&file);
      stream << doc->toString();
      file.close();
    }
  }*/
  delete doc;
  //TODO: The user should be notified if something bad happens
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
void MainWindow::importDataSlot()
{
  mMessageManager->sendInfo("Importing data...");
  QString fileName = QFileDialog::getOpenFileName( this,
                                  QString(tr("Select data file")),
                                  mSettings->value("globalPatientDataFolder").toString() );
  if(fileName.isEmpty())
  {
    mMessageManager->sendInfo("Import cancelled");
    return;
  }

  QDir dir;
  QFileInfo fileInfo(fileName);
  QString fileType = fileInfo.suffix();
  QString pathToImageFolder = mSettings->value("globalPatientDataFolder")
                             .toString()+"/Images/";
  if(!dir.exists(pathToImageFolder))
  {
    dir.mkpath(pathToImageFolder);
    mMessageManager->sendInfo("Made new directory: "+pathToImageFolder.toStdString());
  }
  QString pathToNewFile = pathToImageFolder+fileInfo.fileName();

  if(QFile::copy(fileName, pathToNewFile))
  {
    mMessageManager->sendInfo("File copied to new location: "+pathToNewFile.toStdString());
  }else
  {
    mMessageManager->sendError("Copy failed!");
  }
  //make sure we also copy the .raw file in case if mhd/hdr
  if(fileType.compare("mhd", Qt::CaseInsensitive) == 0)
  {
    QString fileName2 = fileName.replace(".mhd", ".raw");
    QString pathToNewFile2 = pathToNewFile.replace(".mhd", ".raw");
    QFile newFile(pathToNewFile2);
    if(QFile::copy(fileName2, pathToNewFile2))
    {
      //TODO FIX
      newFile.waitForReadyRead(-1);
      mMessageManager->sendInfo("File copied to new location: "+pathToNewFile2.toStdString());
    }
    else
      mMessageManager->sendError("Copy failed!");
  }else if(fileType.compare("hdr", Qt::CaseInsensitive) == 0)
  {
    QString fileName2 = fileName.replace(".mhd", ".raw");
    QString pathToNewFile2 = pathToNewFile.replace(".hdr", ".raw");
    if(QFile::copy(fileName2, pathToNewFile2))
    {
      QFile newFile(pathToNewFile2);
      //TODO FIX
      newFile.waitForReadyRead(-1);
      mMessageManager->sendInfo("File copied to new location: "+pathToNewFile2.toStdString());
    }
    else
      mMessageManager->sendError("Copy failed!");
  }

  fileName = pathToNewFile;
  std::cout << fileName.toStdString() << std::endl;

  if(fileType.compare("mhd", Qt::CaseInsensitive) == 0 ||
     fileType.compare("hdr", Qt::CaseInsensitive) == 0)
  {
    ssc::ImagePtr image = mDataManager->loadImage(fileName.toStdString(), ssc::rtMETAIMAGE);
    mMessageManager->sendInfo("Meta data imported.");
  }else if(fileType.compare("stl", Qt::CaseInsensitive) == 0)
  {
    mDataManager->loadMesh(fileName.toStdString(), ssc::mrtSTL);
    mMessageManager->sendInfo("STL data imported.");
  }else if(fileType.compare("vtk", Qt::CaseInsensitive) == 0)
  {
    mDataManager->loadMesh(fileName.toStdString(), ssc::mrtPOLYDATA);
    mMessageManager->sendInfo("Vtk data imported.");
  }
}
void MainWindow::configureSlot()
{
  QString configFile = mSettings->value("toolConfigFilePath").toString();

  if(mSettings->value("toolManager/toolConfigFilePath").toString() ==
      QDir::homePath())
  {
    QString configFile = QFileDialog::getOpenFileName(this,
        tr("Select configuration file (*.xml)"),
        mSettings->value("toolConfigFilePath").toString(),
        tr("Configuration files (*.xml)"));
    mSettings->setValue("toolConfigFilePath", configFile);
    mMessageManager->sendInfo("Tool configuration file is now selected: "+
                              configFile.toStdString());
  }
  mToolManager->setConfigurationFile(configFile.toStdString());

  QString loggingPath = mSettings->value("globalPatientDataFolder").toString()+"/Logs";
  QDir loggingDir(loggingPath);
  if(!loggingDir.exists())
  {
    loggingDir.mkdir(loggingPath);
    mMessageManager->sendInfo("Made a folder for logging: "+loggingPath.toStdString());
  }
  mToolManager->setLoggingFolder(loggingPath.toStdString());

  mToolManager->configure();
}
void MainWindow::printSlot(const QString& message, int timeout)
{
  //TODO REMOVE just for debugging
  std::cout << message.toStdString() << std::endl;
}
}//namespace cx
