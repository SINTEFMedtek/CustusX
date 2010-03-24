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
#include <QTimer>
#include "sscTypeConversions.h"
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
#include "cxPatientRegistrationWidget.h"
#include "cxView3D.h"
#include "cxView2D.h"
#include "cxPreferencesDialog.h"

namespace cx
{

FileCopied::FileCopied(const std::string& absolutefilePath, const std::string& relativefilePath, ssc::DataPtr data) :
  mFilePath(absolutefilePath),
  mRelativeFilePath(relativefilePath),
  mData(data)
{}
  
void FileCopied::areFileCopiedSlot()
{
  //messageMan()->sendInfo("Check if file is copied: "+mFilePath);
  
  QFile file(QString::fromStdString(mFilePath));
  
  bool correctCopy = false;
  QFileInfo fileInfo(mFilePath.c_str());
  QString fileType = fileInfo.suffix();
  
  if (!file.exists() || !file.open(QIODevice::NotOpen|QIODevice::ReadOnly))
  {
    messageManager()->sendWarning("File is not copied: "+mFilePath+" Cannot open file.");
    file.close();
    QTimer::singleShot(5000, this, SLOT(areFileCopiedSlot()));// Wait another 5 seconds
  }
  else if(fileType.compare("mhd", Qt::CaseInsensitive) == 0 ||
          fileType.compare("mha", Qt::CaseInsensitive) == 0)
  {
    bool foundDimSize = false;
    bool foundElementType = false;
    // Parse file to check if copied correctly
    QTextStream stream(&file);
    QString sLine;
    QRegExp rx("([\\d\\.]+)\\s+([\\d\\.]+)\\s+([\\d\\.]+)");
    QRegExp rxDimSize("DimSize =");
    QRegExp rxElementType("ElementType = ");
    int numElements = 0;
    bool end = false;
    int elementSize = 1;
    
    while ( !end && !stream.atEnd() )
    {
      sLine = stream.readLine(); // line of text excluding '\n'
      if(sLine.isEmpty())
        end = true;
      //messageMan()->sendInfo("line: "+sLine.toStdString());
      {
        rxDimSize.indexIn(sLine);
        if ( rxDimSize.pos() != -1 )
        {
          rx.indexIn(sLine);
          rx.pos();
          QStringList list = rx.capturedTexts();
          numElements = list[1].toInt() * list[2].toInt() * list[3].toInt();
          foundDimSize = true;
        }
        rxElementType.indexIn(sLine);
        if ( rxElementType.pos() != -1 )
        {
          rx.indexIn(sLine);
          rx.pos();
          QString elementType = rx.cap();
          messageManager()->sendInfo("ElementType: "+elementType.toStdString());
          if(elementType=="MET_USHORT") //16 bit
            elementSize = 2;
          else if(elementType=="MET_SHORT")
            elementSize = 2;
          else // 8 bit
            elementSize = 1;
          foundElementType = true;
        }
        if(foundDimSize && foundElementType)
          end = true;
      }
    }
    if(!file.flush())
      messageManager()->sendWarning("Flush error");
    file.close();
    
    if (!foundDimSize)
    {
      messageManager()->sendWarning("File is not copied correctly: "+mFilePath+" Parts missing");
    }
    else
    {
      QRegExp rxFileype;
      QString rawFilepath = QString::fromStdString(mFilePath);
      if(rawFilepath.endsWith(".mhd"))
        rxFileype.setPattern("\\.mhd");
      else if(rawFilepath.endsWith(".mha"))
        rxFileype.setPattern("\\.mha");
      rawFilepath = rawFilepath.replace(rxFileype, ".raw");
      QFile rawFile(rawFilepath);
      rawFile.open(QIODevice::ReadOnly);
      
      //Test if raw file is large enough
      if(rawFile.bytesAvailable() < (numElements * elementSize))
        messageManager()->sendWarning("File is not copied correctly: "+rawFilepath.toStdString()+" Parts missing");
      else
        correctCopy = true;
      
    }
  }
  else if(fileType.compare("stl", Qt::CaseInsensitive) == 0)
  {
    //TODO: Check intergity of file
    correctCopy = true;
  }
  else if(fileType.compare("vtk", Qt::CaseInsensitive) == 0)
  {
    //TODO: Check intergity of file
    correctCopy = true;
  }
  
  if(!correctCopy)
  {
    messageManager()->sendWarning("File(s) not copied correctly - wait another 5 seconds");
    QTimer::singleShot(5000, this, SLOT(areFileCopiedSlot()));
  }
  else
  {
    messageManager()->sendInfo("File copied correctly: "+mFilePath);
    mData->setFilePath(mRelativeFilePath); // Update file path
    
    //Save patient, to avoid problems
    emit fileCopiedCorrectly();
  }
}
  
MainWindow::MainWindow() :
  mCurrentWorkflowState(PATIENT_DATA),
  mViewManager(ViewManager::getInstance()),
  mDataManager(DataManager::getInstance()),
  mToolManager(ToolManager::getInstance()),
  mRepManager(RepManager::getInstance()),
  mRegistrationManager(RegistrationManager::getInstance()),
  mCentralWidget(new QWidget(this)),
  mContextDockWidget(new ContextDockWidget(this)),
  mImageRegistrationWidget(new ImageRegistrationWidget(mContextDockWidget)),
  mPatientRegistrationWidget(new PatientRegistrationWidget(mContextDockWidget)),
  mTransferFunctionWidget(new TransferFunctionWidget(mContextDockWidget)),
  mBrowserWidget(new BrowserWidget(mContextDockWidget)),
  mNavigationWidget(new NavigationWidget(mContextDockWidget)),
  mCustomStatusBar(new CustomStatusBar()),
  mImageRegistrationIndex(-1),
  mPatientRegistrationIndex(-1),
  mNavigationIndex(-1),
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
  
  // Restore saved window states
  restoreGeometry(mSettings->value("mainWindow/geometry").toByteArray());
  restoreState(mSettings->value("mainWindow/windowState").toByteArray());

  //debugging
  connect(messageManager(), SIGNAL(emittedMessage(const QString&, int)),
          this, SLOT(loggingSlot(const QString&, int)));

  this->changeState(PATIENT_DATA, PATIENT_DATA);
  
  // TODO: Find a better way to do this
  //if we remove this section some items stack in the upper left corner,
  //probably some items missing a parent, check it out.
  mImageRegistrationIndex = mContextDockWidget->addTab(mImageRegistrationWidget,
                                                       QString("Image Registration"));
  mContextDockWidget->removeTab(mImageRegistrationIndex);
  mPatientRegistrationIndex = mContextDockWidget->addTab(mPatientRegistrationWidget,
                                                         QString("Patient Registration"));
  mContextDockWidget->removeTab(mPatientRegistrationIndex);
  mNavigationIndex = mContextDockWidget->addTab(mNavigationWidget,
                                                         QString("Navigation"));
  mContextDockWidget->removeTab(mNavigationIndex);
  
  // Don't show the Widget before all elements are initialized
  this->show();
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
  mQuitAction = new QAction(tr("&Quit"), this);
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
  
  std::vector<ViewManager::LayoutType> layouts = ViewManager::getInstance()->availableLayouts();
  for (unsigned i=0; i<layouts.size(); ++i)
    addLayoutAction(layouts[i]);

  connect(mViewManager, SIGNAL(layoutChanged()), this, SLOT(layoutChangedSlot()));
  layoutChangedSlot();

  //context widgets
  this->addDockWidget(Qt::LeftDockWidgetArea, mContextDockWidget);
  connect(mContextDockWidget, SIGNAL(currentImageChanged(ssc::ImagePtr)),
          mViewManager, SLOT(currentImageChangedSlot(ssc::ImagePtr)));
  connect(mContextDockWidget, SIGNAL(currentImageChanged(ssc::ImagePtr)),
          mImageRegistrationWidget, SLOT(currentImageChangedSlot(ssc::ImagePtr)));
  connect(mContextDockWidget, SIGNAL(currentImageChanged(ssc::ImagePtr)),
          mPatientRegistrationWidget, SLOT(currentImageChangedSlot(ssc::ImagePtr)));
  connect(mContextDockWidget, SIGNAL(currentImageChanged(ssc::ImagePtr)),
          mTransferFunctionWidget, SLOT(currentImageChangedSlot(ssc::ImagePtr)));
  
  connect(this, SIGNAL(deleteCurrentImage()),
          mContextDockWidget, SLOT(deleteCurrentImageSlot()));
}

/** Called when the layout is changed: update the layout menu
 */
void MainWindow::layoutChangedSlot()
{
  ViewManager::LayoutType type = mViewManager->currentLayout();
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
  ViewManager::LayoutType type = static_cast<ViewManager::LayoutType>(action->data().toInt());
  mViewManager->changeLayout(type);
}

/** Add one layout as an action to the layout menu.
 */
QAction* MainWindow::addLayoutAction(ViewManager::LayoutType layout)
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
//  QList<QAction*> layouts = mLayoutActionGroup->actions();
//  for (int i=0; i<layouts.size(); ++i)
//    mLayoutMenu->addActions(layouts[i]);
  mLayoutMenu->addActions(mLayoutActionGroup->actions());
//  mLayoutMenu->addAction(m3D_1x1_LayoutAction);
//  mLayoutMenu->addAction(m3DACS_2x2_LayoutAction);
//  mLayoutMenu->addAction(m3DACS_1x3_LayoutAction);
//  mLayoutMenu->addAction(mACSACS_2x3_LayoutAction);
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

}
void MainWindow::createStatusBar()
{
  //TODO, not working as intended
  this->setStatusBar(mCustomStatusBar);
}
/**
 * Xml version 1.0: Knows about the nodes: \n
 * \<managers\> \n
 *   \<datamanager\> \n
 *     \<image\> \n
 *        \<uid\> //an images unique id \n
 *        \<name\> //an images name \n
 *        \<transferfunctions\> //an images transferefunction \n
 *            \<alpha\> //a transferefunctions alpha values \n
 *            \<color\> //a transferefunctions color values
 */
/**
 * Xml version 2.0: Knows about the nodes: \n
 * \<patient\> \n
 *  \<active_patient\> //relative path to this patients folder \n
 *  \<managers\> \n
 *     \<datamanager\> \n
 *       \<image\> \n
 *         \<uid\> //an images unique id \n
 *         \<name\> //an images name \n
 *         \<transferfunctions\> //an images transferefunction \n
 *            \<alpha\> //a transferefunctions alpha values \n
 *            \<color\> //a transferefunctions color values \n
 */
void MainWindow::generateSaveDoc(QDomDocument& doc)
{
  doc.appendChild(doc.createProcessingInstruction("xml version =", "'2.0'"));

  QDomElement patientNode = doc.createElement("patient");
  QDomElement activePatientNode = doc.createElement("active_patient");
  activePatientNode.appendChild(doc.createTextNode(mActivePatientFolder.toStdString().c_str()));
  patientNode.appendChild(activePatientNode);
  doc.appendChild(patientNode);

  QDomElement managerNode = doc.createElement("managers");
  patientNode.appendChild(managerNode);

  mDataManager->addXml(managerNode);
  ssc::ToolManager::getInstance()->addXml(managerNode);

  //TODO Implement
  /*
  messageMan()->getXml(doc); //TODO
  mViewManager->getXml(doc); //TODO
  mRepManager->getXml(doc); //TODO
  mRegistrationManager->getXml(doc);*/

  messageManager()->sendInfo("Xml file ready to be written to disk.");
}
void MainWindow::readLoadDoc(QDomDocument& doc)
{
  //Get all the nodes
  QDomNode patientNode = doc.namedItem("patient");
  QDomNode managerNode = patientNode.namedItem("managers");
  QDomNode dataManagerNode = managerNode.namedItem("datamanager");

  //Evaluate the xml nodes and load what's needed
  if(!patientNode.isNull())
  {
    QDomElement activePatientNode = patientNode.namedItem("active_patient").toElement();
    if(!activePatientNode.isNull())
    {
      mActivePatientFolder = activePatientNode.text();
      messageManager()->sendInfo("Active patient loaded to be "
                                +mActivePatientFolder.toStdString());
    }
  }
  if (!dataManagerNode.isNull())
  {    
    QString absolutePatientPath = mSettings->value("globalPatientDataFolder").toString()+"/"+mActivePatientFolder;
    mDataManager->parseXml(dataManagerNode, absolutePatientPath);
  }

  QDomNode toolmanager = managerNode.namedItem("toolManager");
  ssc::ToolManager::getInstance()->parseXml(toolmanager);
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
  mImageRegistrationIndex = mContextDockWidget->addTab(mBrowserWidget,
      QString("Browser"));
  mImageRegistrationIndex = mContextDockWidget->addTab(mTransferFunctionWidget,
      QString("Transfer functions"));
}
void MainWindow::deactivatePatientDataState()
{}
void MainWindow::activateImageRegistationState()
{
  mImageRegistrationIndex = mContextDockWidget->addTab(mImageRegistrationWidget,
      QString("Image Registration"));
  
  mViewManager->setRegistrationMode(ssc::rsIMAGE_REGISTRATED);

  ssc::ProbeRepPtr probeRep = mRepManager->getProbeRep("ProbeRep_1");
/*  LandmarkRepPtr landmarkRep = mRepManager->getLandmarkRep("LandmarkRep_1");
  mViewManager->get3DView("View3D_1")->addRep(landmarkRep);
  mViewManager->get3DView("View3D_1")->addRep(probeRep);
  */
  connect(mImageRegistrationWidget, SIGNAL(thresholdChanged(int)),
          probeRep.get(), SLOT(setThresholdSlot(int)));

  mCurrentWorkflowState = IMAGE_REGISTRATION;
}
void MainWindow::deactivateImageRegistationState()
{
  if(mImageRegistrationIndex != -1)
  {
    mViewManager->setRegistrationMode(ssc::rsNOT_REGISTRATED);
    mContextDockWidget->removeTab(mImageRegistrationIndex);
    mImageRegistrationIndex = -1;
    
    ssc::ProbeRepPtr probeRep = mRepManager->getProbeRep("ProbeRep_1");
/*    LandmarkRepPtr landmarkRep = mRepManager->getLandmarkRep("LandmarkRep_1");
    mViewManager->get3DView("View3D_1")->removeRep(landmarkRep);
    mViewManager->get3DView("View3D_1")->removeRep(probeRep);*/

    disconnect(mImageRegistrationWidget, SIGNAL(thresholdChanged(const int)),
            probeRep.get(), SLOT(setThresholdSlot(const int)));
  }
}
void MainWindow::activatePatientRegistrationState()
{
  mPatientRegistrationIndex = mContextDockWidget->addTab(mPatientRegistrationWidget,
      QString("Patient Registration"));
  
  mViewManager->setRegistrationMode(ssc::rsPATIENT_REGISTRATED);
//  LandmarkRepPtr landmarkRep = mRepManager->getLandmarkRep("LandmarkRep_1");
//  mViewManager->get3DView("View3D_1")->addRep(landmarkRep);
  
  mCurrentWorkflowState = PATIENT_REGISTRATION;
}
void MainWindow::deactivatePatientRegistrationState()
{
  if(mPatientRegistrationIndex != -1)
  {
    mViewManager->setRegistrationMode(ssc::rsNOT_REGISTRATED);
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
  
void MainWindow::createPatientFolders(QString choosenDir)
{
  if(!choosenDir.endsWith(".cx3"))
    choosenDir.append(".cx3");
  
  // Set active patient folder. Use path relative to the globalPatientDataFolder
  QString patientDatafolder = mSettings->value("globalPatientDataFolder").toString();
  QDir patientDataDir(patientDatafolder);
  mActivePatientFolder = patientDataDir.relativeFilePath(choosenDir);
  messageManager()->sendInfo("Selected a patient to work with.");
  
  // Create folders
  if(!QDir().exists(choosenDir))
  {
    QDir().mkdir(choosenDir);
    messageManager()->sendInfo("Made a new patient folder: "+choosenDir.toStdString());
  }
  
  QString newDir = choosenDir;
  newDir.append("/Images"); 
  if(!QDir().exists(newDir))
  {
    QDir().mkdir(newDir);
    messageManager()->sendInfo("Made a new image folder: "+newDir.toStdString());
  }
  
  newDir = choosenDir;
  newDir.append("/Surfaces"); 
  if(!QDir().exists(newDir))
  {
    QDir().mkdir(newDir);
    messageManager()->sendInfo("Made a new surface folder: "+newDir.toStdString());
  }
  
  newDir = choosenDir;
  newDir.append("/Logs"); 
  if(!QDir().exists(newDir))
  {
    QDir().mkdir(newDir);
    messageManager()->sendInfo("Made a new logging folder: "+newDir.toStdString());
  }
  this->savePatientFileSlot();
}
  
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
  messageManager()->sendInfo("quitSlot - never called?");
  //TODO
}  
  
void MainWindow::newPatientSlot()
{  
  QString patientDatafolder = mSettings->value("globalPatientDataFolder").toString();
  QString name = QDateTime::currentDateTime().toString("yyyyMMdd'T'hhmmss") + "_";
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
  
  createPatientFolders(choosenDir);
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
  
  QFile file(choosenDir+"/custusdoc.xml");
  if(file.open(QIODevice::ReadOnly))
  {    
    QDomDocument doc;
    QString emsg;
    int eline, ecolumn;
    // Read the file
    if (!doc.setContent(&file, false, &emsg, &eline, &ecolumn))
    {
      messageManager()->sendError("Could not parse XML file :"
                                 +file.fileName().toStdString()+" because: "
                                 +emsg.toStdString()+"");
    }
    else
    {
      //Read the xml
      this->readLoadDoc(doc);
    }
    file.close();
  }
  else //User have creted the directory create xml file and folders
  {
    createPatientFolders(choosenDir);
  }
}
void MainWindow::savePatientFileSlot()
{
  
  if(mActivePatientFolder.isEmpty())
  {
    messageManager()->sendWarning("No patient selected, select or create patient before saving!");
    this->newPatientSlot();
    return;
  }
  
  //Gather all the information that needs to be saved
  QDomDocument doc;
  this->generateSaveDoc(doc);

  QString activePatientDir = mSettings->value("globalPatientDataFolder").toString();
  activePatientDir += "/"+mActivePatientFolder;
  QFile file(activePatientDir + "/custusdoc.xml");
  if(file.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    QTextStream stream(&file);
    stream << doc.toString();
    file.close();
    messageManager()->sendInfo("Created "+file.fileName().toStdString());
  }
  else
  {
    messageManager()->sendError("Could not open "+file.fileName().toStdString()
                               +" Error: "+file.errorString().toStdString());
  }

  //Write the data to file, fx modified images... etc...
  //TODO Implement when we know what we want to save here...
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
  messageManager()->sendInfo("Importing data...");
  QString fileName = QFileDialog::getOpenFileName( this,
                                  QString(tr("Select data file")),
                                  mSettings->value("globalPatientDataFolder").toString(),
                                  tr("Image/Mesh (*.mhd *.mha *.stl *.vtk)"));
  if(fileName.isEmpty())
  {
    messageManager()->sendInfo("Import cancelled");
    return;
  }

  QString globalPatientFolderPath = mSettings->value("globalPatientDataFolder").toString();
  QString patientsImageFolder = globalPatientFolderPath+"/"+mActivePatientFolder+"/Images/";
  QString patientsSurfaceFolder = globalPatientFolderPath+"/"+mActivePatientFolder+"/Surfaces/";

  QDir dir;
  if(!dir.exists(patientsImageFolder))
  {
    dir.mkpath(patientsImageFolder);
    messageManager()->sendInfo("Made new directory: "+patientsImageFolder.toStdString());
  }
  if(!dir.exists(patientsSurfaceFolder))
  {
    dir.mkpath(patientsSurfaceFolder);
    messageManager()->sendInfo("Made new directory: "+patientsSurfaceFolder.toStdString());
  }

  QFileInfo fileInfo(fileName);
  QString fileType = fileInfo.suffix();
  QString pathToNewFile = patientsImageFolder+fileInfo.fileName();
  QFile fromFile(fileName);

  //Need to wait for the copy to finish...
  
  // Read files before copy
  ssc::DataPtr data;
  
  if(fileType.compare("mhd", Qt::CaseInsensitive) == 0 ||
     fileType.compare("mha", Qt::CaseInsensitive) == 0)
  {
    //ssc::ImagePtr image = mDataManager->loadImage(fileName.toStdString(), ssc::rtMETAIMAGE);
    data = mDataManager->loadImage(fileName.toStdString(), ssc::rtMETAIMAGE);
  }else if(fileType.compare("stl", Qt::CaseInsensitive) == 0)
  {
    data = mDataManager->loadMesh(fileName.toStdString(), ssc::mrtSTL);
    pathToNewFile = patientsSurfaceFolder+fileInfo.fileName();
  }else if(fileType.compare("vtk", Qt::CaseInsensitive) == 0)
  {
    data = mDataManager->loadMesh(fileName.toStdString(), ssc::mrtPOLYDATA);
    pathToNewFile = patientsSurfaceFolder+fileInfo.fileName();
  }
  data->setName(fileInfo.fileName().toStdString());
  
  QDir patientDataDir(mSettings->value("globalPatientDataFolder").toString()
                      +"/"+mActivePatientFolder);
  FileCopied *fileCopied = new FileCopied(pathToNewFile.toStdString(), 
                                          patientDataDir.relativeFilePath(pathToNewFile).toStdString(), 
                                          data);
  connect(fileCopied, SIGNAL(fileCopiedCorrectly()), 
          this, SLOT(savePatientFileSlot()));
  QTimer::singleShot(5000, fileCopied, SLOT(areFileCopiedSlot()));// Wait 5 seconds
    
  //Copy file
  if(fileName != pathToNewFile) //checks if we need to copy
  {
    QFile toFile(pathToNewFile);
    if(fromFile.copy(toFile.fileName()))
    {
      //messageMan()->sendInfo("File copied to new location: "+pathToNewFile.toStdString());
    }else
    {
      messageManager()->sendError("First copy failed!");
      return;
    }
    if(!toFile.flush())
      messageManager()->sendWarning("Failed to copy file"+toFile.fileName().toStdString());
    if(!toFile.exists())
      messageManager()->sendWarning("File not copied");
    //make sure we also copy the .raw file in case if mhd/mha
    if(fileType.compare("mhd", Qt::CaseInsensitive) == 0)
    {
      //presuming the other file is a raw file
      //TODO: what if it's not?
      QString originalRawFile = fileName.replace(".mhd", ".raw");
      QString newRawFile = pathToNewFile.replace(".mhd", ".raw");
      fromFile.setFileName(originalRawFile);
      toFile.setFileName(newRawFile);
      
      if(fromFile.copy(toFile.fileName()))
      {
        //messageMan()->sendInfo("File copied to new location: "+newRawFile.toStdString());
      }
      else
      {
        messageManager()->sendError("Second copy failed!");
        return;
      }
      if(!toFile.flush())
        messageManager()->sendWarning("Failed to copy file"+toFile.fileName().toStdString());
      if(!toFile.exists())
        messageManager()->sendWarning("File not copied");
      
    }else if(fileType.compare("mha", Qt::CaseInsensitive) == 0)
    {
      //presuming the other file is a raw file
      //TODO: what if it's not?
      QString originalRawFile = fileName.replace(".mha", ".raw");
      QString newRawFile = pathToNewFile.replace(".mha", ".raw");
      fromFile.setFileName(originalRawFile);
      toFile.setFileName(newRawFile);
      
      if(fromFile.copy(toFile.fileName()))
      {
        //messageMan()->sendInfo("File copied to new location: "+newRawFile.toStdString());
      }
      else
      {
        messageManager()->sendError("Second copy failed!");
        return;
      }
    }
  }
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
    mRegistrationManager->setManualPatientRegistration(patientRegistration);
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
  mToolManager->setConfigurationFile(configFile.toStdString());

  QString loggingPath = mSettings->value("globalPatientDataFolder").toString()
                        +"/"+mActivePatientFolder+"/Logs/";
  QDir loggingDir(loggingPath);
  if(!loggingDir.exists())
  {
    loggingDir.mkdir(loggingPath);
    messageManager()->sendInfo("Made a folder for logging: "+loggingPath.toStdString());
  }
  mToolManager->setLoggingFolder(loggingPath.toStdString());

  mToolManager->configure();
  
  if(mToolManager->isConfigured())
  {
    View3D* view = mViewManager->get3DView("View3D_1");

    ToolRep3DMap* toolRep3DMap = RepManager::getInstance()->getToolRep3DReps();
    ToolRep3DMap::iterator repIt = toolRep3DMap->begin();
    ssc::ToolManager::ToolMapPtr configuredTools = ToolManager::getInstance()->getConfiguredTools();
    ssc::ToolManager::ToolMap::iterator toolIt = configuredTools->begin();
    while((toolIt != configuredTools->end()) && (repIt != toolRep3DMap->end()))
    {
      if(toolIt->second->getType() != ssc::Tool::TOOL_REFERENCE)
      {
        repIt->second->setTool(toolIt->second);
        view->addRep(repIt->second);
        repIt++;
      }
      toolIt++;
    }
  }
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
