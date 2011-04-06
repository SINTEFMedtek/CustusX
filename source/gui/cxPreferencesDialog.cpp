
#include <QtGui>

#include <iostream>
#include "sscMessageManager.h"
#include "sscDoubleWidgets.h"
#include "sscEnumConverter.h"
#include "cxPreferencesDialog.h"
#include "cxViewManager.h"
#include "cxDataLocations.h"
#include "cxToolManager.h"
#include "cxDataLocations.h"
#include "cxStateMachineManager.h"
#include "cxFilePreviewWidget.h"
#include "cxToolConfigureWidget.h"
#include "cxToolFilterWidget.h"

namespace cx
{

//==============================================================================
// PreferencesTab
//------------------------------------------------------------------------------

PreferencesTab::PreferencesTab(QWidget *parent) :
    QWidget(parent),
    mSettings(DataLocations::getSettings())
{
  mTopLayout = new QVBoxLayout;

  QVBoxLayout* vtopLayout = new QVBoxLayout;
  vtopLayout->addLayout(mTopLayout);
  vtopLayout->setMargin(0);
  vtopLayout->addStretch();
  this->setLayout(vtopLayout);
}
//==============================================================================
// GeneralTab
//------------------------------------------------------------------------------

GeneralTab::GeneralTab(QWidget *parent) :
    PreferencesTab(parent)
{}

void GeneralTab::init()
{
  mGlobalPatientDataFolder = mSettings->value("globalPatientDataFolder").toString();

  connect(stateManager()->getApplication().get(), SIGNAL(activeStateChanged()), this, SLOT(applicationStateChangedSlot()));

  // patientDataFolder
  QLabel* patientDataFolderLabel = new QLabel(tr("Patient data folder:"));

  mPatientDataFolderComboBox = new QComboBox;
  mPatientDataFolderComboBox->addItem( mGlobalPatientDataFolder);

  QAction* browsePatientFolderAction = new QAction(QIcon(":/icons/open.png"), tr("B&rowse..."), this);
  connect(browsePatientFolderAction, SIGNAL(triggered()), this, SLOT(browsePatientDataFolderSlot()));
  QToolButton* browsePatientFolderButton = new QToolButton(this);
  browsePatientFolderButton->setDefaultAction(browsePatientFolderAction);
  
//  QLabel *toolConfigFilesLabel = new QLabel(tr("Tool configuration files:"));
//  mToolConfigFilesComboBox = new QComboBox;
//  connect( mToolConfigFilesComboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(currentToolConfigFilesIndexChangedSlot(const QString &)) );
  
  // Choose application name
  QLabel* chooseApplicationLabel = new QLabel(tr("Choose application:"));
  mChooseApplicationComboBox = new QComboBox();
  setApplicationComboBox();
  connect(mChooseApplicationComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(currentApplicationChangedSlot(int)));
//  mCurrentToolConfigFile = mSettings->value("toolConfigFile").toString();
  this->applicationStateChangedSlot();
  
  // Layout
  QGridLayout *mainLayout = new QGridLayout;
  mainLayout->addWidget(patientDataFolderLabel, 0, 0);
  mainLayout->addWidget(mPatientDataFolderComboBox, 0, 1);
  mainLayout->addWidget(browsePatientFolderButton, 0, 2);
  
  mainLayout->addWidget(chooseApplicationLabel, 8, 0);
  mainLayout->addWidget(mChooseApplicationComboBox, 8, 1);
  
//  mainLayout->addWidget(toolConfigFilesLabel, 9, 0);
//  mainLayout->addWidget(mToolConfigFilesComboBox, 9, 1);
 
  mTopLayout->addLayout(mainLayout);
}

GeneralTab::~GeneralTab()
{}

void GeneralTab::browsePatientDataFolderSlot()
{
  mGlobalPatientDataFolder = QFileDialog::getExistingDirectory(this, 
                                                     tr("Find Patient Data Folder"), 
                                                     mGlobalPatientDataFolder,
                                                     QFileDialog::ShowDirsOnly);
  if( !mGlobalPatientDataFolder.isEmpty() ) {
    mPatientDataFolderComboBox->addItem( mGlobalPatientDataFolder );
    mPatientDataFolderComboBox->setCurrentIndex( mPatientDataFolderComboBox->currentIndex() + 1 );
  }
}

//void GeneralTab::currentToolConfigFilesIndexChangedSlot(const QString & newToolConfigFile)
//{
//  mCurrentToolConfigFile = newToolConfigFile;
//}

//void GeneralTab::setToolConfigComboBox()
//{
//  mToolConfigFilesComboBox->blockSignals(true);
//	QDir dir(DataLocations::getApplicationToolConfigPath());
//    dir.setFilter(QDir::Files);
//
//    QStringList nameFilters;
//    nameFilters << "*.xml";
//    dir.setNameFilters(nameFilters);
//
//    QStringList list = dir.entryList();
//
//    mToolConfigFilesComboBox->clear();
//    mToolConfigFilesComboBox->addItems( list );
//
//    int currentIndex = mToolConfigFilesComboBox->findText( mCurrentToolConfigFile );
//    mToolConfigFilesComboBox->setCurrentIndex( currentIndex );
//    mToolConfigFilesComboBox->blockSignals(false);
//}

void GeneralTab::setApplicationComboBox()
{
  mChooseApplicationComboBox->blockSignals(true);
  mChooseApplicationComboBox->clear();
  QList<QAction*> actions = stateManager()->getApplication()->getActionGroup()->actions();
  for (int i=0; i<actions.size(); ++i)
  {
    mChooseApplicationComboBox->insertItem(i, QIcon(), actions[i]->text(), actions[i]->data());
    if (actions[i]->isChecked())
      mChooseApplicationComboBox->setCurrentIndex(i);
  }

  mChooseApplicationComboBox->blockSignals(false);
}

void GeneralTab::applicationStateChangedSlot()
{
  mChooseApplicationComboBox->blockSignals(true);
  QList<QAction*> actions = stateManager()->getApplication()->getActionGroup()->actions();
  for (int i=0; i<actions.size(); ++i)
  {
    if (actions[i]->isChecked())
      mChooseApplicationComboBox->setCurrentIndex(i);
  }

  mChooseApplicationComboBox->blockSignals(false);

//  this->setToolConfigComboBox();

  // this hack ensures that the tool folder is reinitialized when changing application.
  // TODO: move to application state
//  if (mToolConfigFilesComboBox->currentIndex()<0)
//    mToolConfigFilesComboBox->setCurrentIndex(0);
}
  
void GeneralTab::currentApplicationChangedSlot(int index)
{
  QList<QAction*> actions = stateManager()->getApplication()->getActionGroup()->actions();
  if (index<0 || index>=actions.size())
    return;
  actions[index]->trigger();
}
  
void GeneralTab::saveParametersSlot()
{
  // currentPatientDataFolder
  mSettings->setValue("globalPatientDataFolder", mGlobalPatientDataFolder);
  
  // currentToolConfigFile
//  mSettings->setValue("toolConfigFile", mCurrentToolConfigFile);
  
  mSettings->sync();

  // update toolmanager config file
//  ToolManager::getInstance()->setConfigurationFile(DataLocations::getToolConfigFilePath());

  emit savedParameters();
}
  
//==============================================================================
// PerformanceTab
//------------------------------------------------------------------------------
PerformanceTab::PerformanceTab(QWidget *parent) :
    PreferencesTab(parent)
{
}

void PerformanceTab::init()
{
  int renderingInterval = mSettings->value("renderingInterval").toInt();
  
  QLabel* renderingIntervalLabel = new QLabel(tr("Rendering interval"));
  
  mRenderingIntervalSpinBox = new QSpinBox;
  mRenderingIntervalSpinBox->setSuffix("ms");
  mRenderingIntervalSpinBox->setMinimum(16);
  mRenderingIntervalSpinBox->setMaximum(1000);
  mRenderingIntervalSpinBox->setValue(renderingInterval);
  connect(mRenderingIntervalSpinBox, SIGNAL(valueChanged(int)), this, SLOT(renderingIntervalSlot(int)));
  
  mRenderingRateLabel = new QLabel("");
  this->renderingIntervalSlot(renderingInterval);

  double Mb = pow(10.0,6);
  bool ok = true;
  double maxRenderSize = mSettings->value("maxRenderSize").toDouble(&ok);
  if (!ok)
    maxRenderSize = 10 * Mb;
  mMaxRenderSize = ssc::DoubleDataAdapterXml::initialize("MaxRenderSize", "Max Render Size (Mb)", "Maximum size of volumes used in volume rendering. Applies to new volumes.", maxRenderSize, ssc::DoubleRange(1*Mb,300*Mb,1*Mb), 0, QDomNode());
  mMaxRenderSize->setInternal2Display(1.0/Mb);

  mSmartRenderCheckBox = new QCheckBox("Smart Render");
  mSmartRenderCheckBox->setChecked(viewManager()->getSmartRender());

  bool useGPURender = mSettings->value("useGPUVolumeRayCastMapper").toBool();
  mGPURenderCheckBox = new QCheckBox("Use GPU 3D Renderer");
  mGPURenderCheckBox->setChecked(useGPURender);

  //Layout
  mMainLayout = new QGridLayout;
  mMainLayout->addWidget(renderingIntervalLabel, 0, 0);
  new ssc::SpinBoxGroupWidget(this, mMaxRenderSize, mMainLayout, 1);
  mMainLayout->addWidget(mRenderingIntervalSpinBox, 0, 1);
  mMainLayout->addWidget(mRenderingRateLabel, 0, 2);
  mMainLayout->addWidget(mSmartRenderCheckBox, 2, 0);
  mMainLayout->addWidget(mGPURenderCheckBox, 3, 0);

  mTopLayout->addLayout(mMainLayout);

  connect(this, SIGNAL(renderingIntervalChanged(int)), viewManager(), SLOT(renderingIntervalChangedSlot(int)));
}

void PerformanceTab::renderingIntervalSlot(int interval)
{    
  mRenderingRateLabel->setText(QString("%1 fps").arg(1000.0/interval, 0, 'f', 1));
} 

void PerformanceTab::saveParametersSlot()
{
  int renderingInterval = mSettings->value("renderingInterval").toInt();
  
  if(renderingInterval != mRenderingIntervalSpinBox->value())
  {
    mSettings->setValue("renderingInterval", mRenderingIntervalSpinBox->value());
    emit renderingIntervalChanged(mRenderingIntervalSpinBox->value());
  }

  mSettings->setValue("useGPUVolumeRayCastMapper", mGPURenderCheckBox->isChecked());
  mSettings->setValue("maxRenderSize", mMaxRenderSize->getValue());

  viewManager()->setSmartRender(mSmartRenderCheckBox->isChecked());
}

//==============================================================================
// View3DTab
//------------------------------------------------------------------------------
VisualizationTab::VisualizationTab(QWidget *parent) :
    PreferencesTab(parent)
{}

void VisualizationTab::init()
{
  double sphereRadius = DataLocations::getSettings()->value("View3D/sphereRadius").toDouble();
  mSphereRadius = ssc::DoubleDataAdapterXml::initialize("SphereRadius", "Sphere Radius", "Radius of sphere markers in the 3D scene.", sphereRadius, ssc::DoubleRange(0.1,10,0.1), 1, QDomNode());

  QPushButton* backgroundColorButton = new QPushButton("Background Color", this);
  connect(backgroundColorButton, SIGNAL(clicked()), this, SLOT(setBackgroundColorSlot()));

  //Layout
  mMainLayout = new QGridLayout;
  mMainLayout->addWidget(backgroundColorButton, 0, 0);
  mMainLayout->addWidget(new ssc::SpinBoxGroupWidget(this, mSphereRadius));

  QHBoxLayout* toptopLayout = new QHBoxLayout;
  toptopLayout->addLayout(mMainLayout);
  toptopLayout->addStretch();

  mTopLayout->addLayout(toptopLayout);

}

void VisualizationTab::saveParametersSlot()
{
  mSettings->setValue("View3D/sphereRadius", mSphereRadius->getValue());
}

void VisualizationTab::setBackgroundColorSlot()
{
  QColor orgval = mSettings->value("backgroundColor").value<QColor>();
  QColor result = QColorDialog::getColor( orgval, this);
  mSettings->setValue("backgroundColor", result);
}

//==============================================================================
// AutomationTab
//------------------------------------------------------------------------------
AutomationTab::AutomationTab(QWidget *parent) :
    PreferencesTab(parent)
{}

void AutomationTab::init()
{
  bool autoStartTracking = mSettings->value("Automation/autoStartTracking").toBool();
  mAutoStartTrackingCheckBox = new QCheckBox("Auto Start Tracking");
  mAutoStartTrackingCheckBox->setChecked(autoStartTracking);

  bool autoStartStreaming = mSettings->value("Automation/autoStartStreaming").toBool();
  mAutoStartStreamingCheckBox = new QCheckBox("Auto Start Streaming");
  mAutoStartStreamingCheckBox->setChecked(autoStartStreaming);

  bool autoReconstruct = mSettings->value("Automation/autoReconstruct").toBool();
  mAutoReconstructCheckBox = new QCheckBox("Auto Reconstruct");
  mAutoReconstructCheckBox->setChecked(autoReconstruct);

  //Layout
  mMainLayout = new QVBoxLayout;
  mMainLayout->addWidget(mAutoStartTrackingCheckBox);
  mMainLayout->addWidget(mAutoStartStreamingCheckBox);
  mMainLayout->addWidget(mAutoReconstructCheckBox);

  mTopLayout->addLayout(mMainLayout);

}

void AutomationTab::saveParametersSlot()
{
  mSettings->setValue("Automation/autoStartTracking", mAutoStartTrackingCheckBox->isChecked());
  mSettings->setValue("Automation/autoStartStreaming", mAutoStartStreamingCheckBox->isChecked());
  mSettings->setValue("Automation/autoReconstruct", mAutoReconstructCheckBox->isChecked());
}

//==============================================================================
// UltrasoundTab
//------------------------------------------------------------------------------
VideoTab::VideoTab(QWidget *parent) :
    PreferencesTab(parent)
{}

void VideoTab::init()
{
  QVBoxLayout* toplayout = new QVBoxLayout;
  QHBoxLayout* acqNameLayout = new QHBoxLayout;
  toplayout->addLayout(acqNameLayout);

  acqNameLayout->addWidget(new QLabel("Description"));
  mAcquisitionNameLineEdit = new QLineEdit(mSettings->value("Ultrasound/acquisitionName").toString());
  acqNameLayout->addWidget(mAcquisitionNameLineEdit);

  bool bw = DataLocations::getSettings()->value("Ultrasound/8bitAcquisitionData").toBool();

  m24bitRadioButton = new QRadioButton("Save acquisition as 24bit", this);
  m24bitRadioButton->setChecked(!bw);
  m8bitRadioButton = new QRadioButton("Save acquisition as 8bit", this);
  m8bitRadioButton->setChecked(bw);

  toplayout->addSpacing(5);
  toplayout->addWidget(m24bitRadioButton);
  toplayout->addWidget(m8bitRadioButton);

  mTopLayout->addLayout(toplayout);

}

void VideoTab::saveParametersSlot()
{
  mSettings->setValue("Ultrasound/acquisitionName", mAcquisitionNameLineEdit->text());
  DataLocations::getSettings()->setValue("Ultrasound/8bitAcquisitionData", m8bitRadioButton->isChecked());
}

//==============================================================================
// ToolConfigTab
//------------------------------------------------------------------------------

ToolConfigTab::ToolConfigTab(QWidget* parent) :
    PreferencesTab(parent),
    mFilePreviewWidget(new FilePreviewWidget(this))
{
  mToolConfigureGroupBox = new ToolConfigureGroupBox(this);
  mToolFilterGroupBox  = new ToolFilterGroupBox(this);

  connect(stateManager()->getApplication().get(), SIGNAL(activeStateChanged()), this, SLOT(applicationChangedSlot()));

  connect(mToolConfigureGroupBox, SIGNAL(toolSelected(QString)), mFilePreviewWidget, SLOT(previewFileSlot(QString)));
  connect(mToolFilterGroupBox, SIGNAL(toolSelected(QString)), mFilePreviewWidget, SLOT(previewFileSlot(QString)));

  this->applicationChangedSlot();
}

ToolConfigTab::~ToolConfigTab()
{}

void ToolConfigTab::init()
{
  //layout
  QGridLayout* layout = new QGridLayout;
  mTopLayout->addLayout(layout);

  layout->addWidget(mToolConfigureGroupBox, 0, 0, 1, 1);
  layout->addWidget(mToolFilterGroupBox, 0, 1, 1, 1);
  layout->addWidget(mFilePreviewWidget, 1, 0, 1, 2);
}

void ToolConfigTab::saveParametersSlot()
{
  mToolConfigureGroupBox->requestSaveConfigurationSlot();

  // currentToolConfigFile
  mSettings->setValue("toolConfigFile", mToolConfigureGroupBox->getCurrenctlySelectedConfiguration());

  mSettings->sync();

  // update toolmanager config file
  ToolManager::getInstance()->setConfigurationFile(DataLocations::getToolConfigFilePath());
}

void ToolConfigTab::applicationChangedSlot()
{
  ssc::CLINICAL_APPLICATION clinicalApplication = string2enum<ssc::CLINICAL_APPLICATION>(stateManager()->getApplication()->getActiveStateName());
  mToolConfigureGroupBox->setClinicalApplicationSlot(clinicalApplication);
  mToolFilterGroupBox->setClinicalApplicationSlot(clinicalApplication);
  mToolFilterGroupBox->setTrackingSystemSlot(ssc::tsPOLARIS);
}

//==============================================================================
// PreferencesDialog
//------------------------------------------------------------------------------

PreferencesDialog::PreferencesDialog(QWidget *parent) :
  QDialog(parent)
{
  mActionGroup = new QActionGroup(this);
  mActionGroup->setExclusive(true);
  mToolBar =  new QToolBar;
  mToolBar->setOrientation(Qt::Vertical);
  tabWidget = new QStackedWidget;

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

  this->addTab(new GeneralTab, tr("General"));
  this->addTab(new PerformanceTab, tr("Performance"));
  this->addTab(new VisualizationTab, tr("Visualization"));
  this->addTab(new AutomationTab, tr("Automation"));
  this->addTab(new VideoTab, tr("Video"));
  this->addTab(new ToolConfigTab, tr("Tool Configuration"));

  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  QHBoxLayout *tabLayout = new QHBoxLayout;
  QFrame* frame = new QFrame;
  frame->setLineWidth(3);
  frame->setFrameShape(QFrame::Panel);
  frame->setFrameShadow(QFrame::Sunken);
  frame->setLayout(new QVBoxLayout);

  tabLayout->addWidget(mToolBar);
  tabLayout->addWidget(frame);
  frame->layout()->addWidget(tabWidget);
  mainLayout->addLayout(tabLayout);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  tabWidget->setCurrentIndex(0);

  buttonBox->button(QDialogButtonBox::Ok)->setFocus();
}

PreferencesDialog::~PreferencesDialog()
{}

void PreferencesDialog::addTab(PreferencesTab* widget, QString name)
{
  widget->init();
  connect(buttonBox, SIGNAL(accepted()), widget, SLOT(saveParametersSlot()));

  QAction* action = new QAction(name, mActionGroup);
  action->setData(tabWidget->count());
  action->setCheckable(true);
  if (!tabWidget->count())
    action->setChecked(true);
  connect(action, SIGNAL(triggered()), this, SLOT(selectTabSlot()));
  QToolButton* button = new QToolButton(this);

  button->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

  button->setDefaultAction(action);
  mToolBar->addWidget(button);

  tabWidget->addWidget(widget);
}

void PreferencesDialog::selectTabSlot()
{
  QAction* action = dynamic_cast<QAction*>(sender());
  if (!action)
    return;
  int val = action->data().toInt();
  tabWidget->setCurrentIndex(val);
}

}//namespace cx
