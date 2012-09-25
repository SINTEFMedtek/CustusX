
#include <QtGui>
#include <QAction>

#include <iostream>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include "sscMessageManager.h"
#include "sscDoubleWidgets.h"
#include "sscEnumConverter.h"
#include "cxSettings.h"
#include "cxPreferencesDialog.h"
#include "cxViewManager.h"
#include "cxDataLocations.h"
#include "cxToolManager.h"
#include "cxDataLocations.h"
#include "cxStateService.h"
#include "cxFilePreviewWidget.h"
#include "cxToolConfigureWidget.h"
#include "cxToolFilterWidget.h"
#include "cxColorSelectButton.h"
#include "cxLayoutEditorTab.h"
#include "cxViewWrapper3D.h"
#include "sscHelperWidgets.h"

namespace cx
{

//==============================================================================
// PreferencesTab
//------------------------------------------------------------------------------

PreferencesTab::PreferencesTab(QWidget *parent) :
    QWidget(parent)
    //settings()(settings())
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
  mGlobalPatientDataFolder = settings()->value("globalPatientDataFolder").toString();

  connect(stateService()->getApplication().get(), SIGNAL(activeStateChanged()), this, SLOT(applicationStateChangedSlot()));

  // patientDataFolder
  QLabel* patientDataFolderLabel = new QLabel(tr("Patient data folder:"));

  mPatientDataFolderComboBox = new QComboBox;
  mPatientDataFolderComboBox->addItem( mGlobalPatientDataFolder);

  QAction* browsePatientFolderAction = new QAction(QIcon(":/icons/open.png"), tr("B&rowse..."), this);
  connect(browsePatientFolderAction, SIGNAL(triggered()), this, SLOT(browsePatientDataFolderSlot()));
  QToolButton* browsePatientFolderButton = new QToolButton(this);
  browsePatientFolderButton->setDefaultAction(browsePatientFolderAction);
  
  // Choose application name
  QLabel* chooseApplicationLabel = new QLabel(tr("Choose application:"));
  mChooseApplicationComboBox = new QComboBox();
  setApplicationComboBox();
  connect(mChooseApplicationComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(currentApplicationChangedSlot(int)));
  this->applicationStateChangedSlot();
  
  // Layout
  QGridLayout *mainLayout = new QGridLayout;
  mainLayout->addWidget(patientDataFolderLabel, 0, 0);
  mainLayout->addWidget(mPatientDataFolderComboBox, 0, 1);
  mainLayout->addWidget(browsePatientFolderButton, 0, 2);
  
  mainLayout->addWidget(chooseApplicationLabel, 8, 0);
  mainLayout->addWidget(mChooseApplicationComboBox, 8, 1);
 
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

void GeneralTab::setApplicationComboBox()
{
  mChooseApplicationComboBox->blockSignals(true);
  mChooseApplicationComboBox->clear();
  QList<QAction*> actions = stateService()->getApplication()->getActionGroup()->actions();
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
  QList<QAction*> actions = stateService()->getApplication()->getActionGroup()->actions();
  for (int i=0; i<actions.size(); ++i)
  {
    if (actions[i]->isChecked())
      mChooseApplicationComboBox->setCurrentIndex(i);
  }

  mChooseApplicationComboBox->blockSignals(false);

}
  
void GeneralTab::currentApplicationChangedSlot(int index)
{
  QList<QAction*> actions = stateService()->getApplication()->getActionGroup()->actions();
  if (index<0 || index>=actions.size())
    return;
  actions[index]->trigger();
}
  
void GeneralTab::saveParametersSlot()
{
  // currentPatientDataFolder
  settings()->setValue("globalPatientDataFolder", mGlobalPatientDataFolder);
  
  settings()->sync();

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
  int renderingInterval = settings()->value("renderingInterval").toInt();
  
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
  double maxRenderSize = settings()->value("maxRenderSize").toDouble(&ok);
  if (!ok)
    maxRenderSize = 10 * Mb;
  mMaxRenderSize = ssc::DoubleDataAdapterXml::initialize("MaxRenderSize", "Max Render Size (Mb)", "Maximum size of volumes used in volume rendering. Applies to new volumes.", maxRenderSize, ssc::DoubleRange(1*Mb,300*Mb,1*Mb), 0, QDomNode());
  mMaxRenderSize->setInternal2Display(1.0/Mb);

  double stillUpdateRate = settings()->value("stillUpdateRate").value<double>();
  mStillUpdateRate = ssc::DoubleDataAdapterXml::initialize("StillUpdateRate", "Still Update Rate", "Still Update Rate in vtkRenderWindow. Restart needed.", stillUpdateRate, ssc::DoubleRange(0.0001, 20, 0.0001), 4, QDomNode());

  mSmartRenderCheckBox = new QCheckBox("Smart Render");
  mSmartRenderCheckBox->setChecked(settings()->value("smartRender", true).toBool());
  mSmartRenderCheckBox->setToolTip("Render only when scene has changed, plus once per second.");

  bool useGPU3DRender = settings()->value("useGPUVolumeRayCastMapper").toBool();
  mGPURenderCheckBox = new QCheckBox("Use GPU 3D Renderer");
  mGPURenderCheckBox->setChecked(useGPU3DRender);
  mGPURenderCheckBox->setToolTip("Use a GPU-based 3D renderer instead of the texture-based one, if available.");

  bool useGPU2DRender = settings()->value("useGPU2DRendering").toBool();
	mGPU2DRenderCheckBox = new QCheckBox("Use GPU 2D Renderer");
	mGPU2DRenderCheckBox->setChecked(useGPU2DRender);
	mGPU2DRenderCheckBox->setToolTip("Use a GPU-based 2D renderer instead of the software-based one, if available.");

#ifndef USE_GLX_SHARED_CONTEXT
	mGPU2DRenderCheckBox->setChecked(false);
	mGPU2DRenderCheckBox->setEnabled(false);
#endif

//  bool useGPU3DDepthPeeling = settings()->value("View3D/depthPeeling").toBool();
//	mGPU3DDepthPeelingCheckBox = new QCheckBox("Use GPU 3D depth peeling");
//	mGPU3DDepthPeelingCheckBox->setChecked(useGPU3DDepthPeeling);
//	mGPU3DDepthPeelingCheckBox->setToolTip("Use a GPU-based 3D depth peeling to correctly visualize translucent surfaces.");

  //Layout
  mMainLayout = new QGridLayout;
  mMainLayout->addWidget(renderingIntervalLabel, 0, 0);
  new ssc::SpinBoxGroupWidget(this, mMaxRenderSize, mMainLayout, 1);
  mMainLayout->addWidget(mRenderingIntervalSpinBox, 0, 1);
  mMainLayout->addWidget(mRenderingRateLabel, 0, 2);
  mMainLayout->addWidget(mSmartRenderCheckBox, 2, 0);
  mMainLayout->addWidget(mGPURenderCheckBox, 3, 0);
  mMainLayout->addWidget(mGPU2DRenderCheckBox, 4, 0);
  //mMainLayout->addWidget(mGPU3DDepthPeelingCheckBox, 5, 0);
  new ssc::SpinBoxGroupWidget(this, mStillUpdateRate, mMainLayout, 6);

  mTopLayout->addLayout(mMainLayout);
}

void PerformanceTab::renderingIntervalSlot(int interval)
{    
  mRenderingRateLabel->setText(QString("%1 fps").arg(1000.0/interval, 0, 'f', 1));
} 

void PerformanceTab::saveParametersSlot()
{
  settings()->setValue("renderingInterval", mRenderingIntervalSpinBox->value());
  settings()->setValue("useGPUVolumeRayCastMapper", mGPURenderCheckBox->isChecked());
  settings()->setValue("useGPU2DRendering", mGPU2DRenderCheckBox->isChecked());
  settings()->setValue("maxRenderSize",     mMaxRenderSize->getValue());
  settings()->setValue("smartRender",       mSmartRenderCheckBox->isChecked());
  settings()->setValue("stillUpdateRate",   mStillUpdateRate->getValue());
//  settings()->setValue("View3D/depthPeeling", mGPU3DDepthPeelingCheckBox->isChecked());
}

//==============================================================================
// View3DTab
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------



VisualizationTab::VisualizationTab(QWidget *parent) :
    PreferencesTab(parent), mStereoTypeActionGroup(NULL)
{}

void VisualizationTab::init()
{
  double sphereRadius = settings()->value("View3D/sphereRadius").toDouble();
  mSphereRadius = ssc::DoubleDataAdapterXml::initialize("SphereRadius", "Sphere Radius", "Radius of sphere markers in the 3D scene.", sphereRadius, ssc::DoubleRange(0.1,10,0.1), 1, QDomNode());

  double labelSize = settings()->value("View3D/labelSize").toDouble();
  mLabelSize = ssc::DoubleDataAdapterXml::initialize("LabelSize", "Label Size", "Size of text labels in the 3D scene.", labelSize, ssc::DoubleRange(0.1,100,0.1), 1, QDomNode());

  ColorSelectButton* backgroundColorButton = new ColorSelectButton("Background Color");
  backgroundColorButton->setColor(settings()->value("backgroundColor").value<QColor>());

  connect(backgroundColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(setBackgroundColorSlot(QColor)));

  bool showDataText = settings()->value("View/showDataText").value<bool>();
  mShowDataText = ssc::BoolDataAdapterXml::initialize("Show Data Text", "",
                                                 "Show the name of each data set in the views.",
                                                 showDataText);
  bool showLabels = settings()->value("View/showLabels").value<bool>();
  mShowLabels = ssc::BoolDataAdapterXml::initialize("Show Labels", "",
                                                 "Attach name labels to entities in the views.",
                                                 showLabels);

  double annotationModelSize = settings()->value("View3D/annotationModelSize").toDouble();
  mAnnotationModelSize = ssc::DoubleDataAdapterXml::initialize("AnnotationModelSize", "Annotation Model Size", "Size (0..1) of the annotation model in the 3D scene.", annotationModelSize, ssc::DoubleRange(0.01,1,0.01), 2, QDomNode());
  QStringList annotationModelRange;
  annotationModelRange = QDir(DataLocations::getRootConfigPath()+"/models/").entryList(QStringList()<<"*.stl");
  annotationModelRange.prepend("<default>");
  QString annotationModel = settings()->value("View3D/annotationModel").toString();
  mAnnotationModel = ssc::StringDataAdapterXml::initialize("AnnotationModel", "Annotation Model", "Name of annotation model in the 3D scene.", annotationModel, annotationModelRange, QDomNode());

  //Stereoscopic visualization (3D view)
  QGroupBox* stereoGroupBox = new QGroupBox("Stereoscopic visualization");
  mStereoTypeComboBox = new QComboBox();
  connect(mStereoTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(stereoTypeChangedSlot(int)));
  this->initStereoTypeComboBox();
  double eyeAngle = settings()->value("View3D/eyeAngle").toDouble();
  mEyeAngleAdapter = ssc::DoubleDataAdapterXml::initialize("Eye angle (degrees)", "",
      "Separation between eyes in degrees",
      eyeAngle, ssc::DoubleRange(0, 10, 0.1), 1);
  connect(mEyeAngleAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(eyeAngleSlot()));

  QVBoxLayout* stereoLayout = new QVBoxLayout();
  stereoLayout->addWidget(mStereoTypeComboBox);
  stereoLayout->addWidget(new ssc::SpinBoxAndSliderGroupWidget(this, mEyeAngleAdapter));
  stereoGroupBox->setLayout(stereoLayout);

  //Layout
  mMainLayout = new QGridLayout;
  mMainLayout->addWidget(backgroundColorButton, 0, 0);
  mMainLayout->addWidget(new ssc::SpinBoxGroupWidget(this, mSphereRadius));
  mMainLayout->addWidget(ssc::createDataWidget(this, mShowDataText));
  mMainLayout->addWidget(ssc::createDataWidget(this, mShowLabels));
  mMainLayout->addWidget(new ssc::SpinBoxGroupWidget(this, mLabelSize));
  mMainLayout->addWidget(new ssc::SpinBoxGroupWidget(this, mAnnotationModelSize));
  mMainLayout->addWidget(ssc::createDataWidget(this, mAnnotationModel));

  mMainLayout->addWidget(stereoGroupBox);

  QHBoxLayout* toptopLayout = new QHBoxLayout;
  toptopLayout->addLayout(mMainLayout);
  toptopLayout->addStretch();

  mTopLayout->addLayout(toptopLayout);
}

void VisualizationTab::initStereoTypeComboBox()
{
  if (mStereoTypeActionGroup)
    return;
  //Insert all actions into an action group
  mStereoTypeActionGroup = new QActionGroup(this);

  QAction* stereoFrameSequentialAction = new QAction("Frame-sequential", mStereoTypeActionGroup);
  QAction* stereoInterlacedAction = new QAction("Interlaced", mStereoTypeActionGroup);
  QAction* stereoDresdenAction = new QAction("Dresden", mStereoTypeActionGroup);
  QAction* stereoRedBlueAction = new QAction("Red/Blue", mStereoTypeActionGroup);
  stereoFrameSequentialAction->setData(QVariant(stFRAME_SEQUENTIAL));
  stereoInterlacedAction->setData(QVariant(stINTERLACED));
  stereoDresdenAction->setData(QVariant(stDRESDEN));
  stereoRedBlueAction->setData(QVariant(stRED_BLUE));

  connect(stereoFrameSequentialAction, SIGNAL(triggered()), this, SLOT(stereoFrameSequentialSlot()));
  connect(stereoInterlacedAction, SIGNAL(triggered()), this, SLOT(stereoInterlacedSlot()));
  connect(stereoDresdenAction, SIGNAL(triggered()), this, SLOT(stereoDresdenSlot()));
  connect(stereoRedBlueAction, SIGNAL(triggered()), this, SLOT(stereoRedBlueSlot()));

  mStereoTypeComboBox->blockSignals(true);
  mStereoTypeComboBox->insertItem(stFRAME_SEQUENTIAL, stereoFrameSequentialAction->text(), stereoFrameSequentialAction->data());
  mStereoTypeComboBox->insertItem(stINTERLACED, stereoInterlacedAction->text(), stereoInterlacedAction->data());
  mStereoTypeComboBox->insertItem(stDRESDEN, stereoDresdenAction->text(), stereoDresdenAction->data());
  mStereoTypeComboBox->insertItem(stRED_BLUE, stereoRedBlueAction->text(), stereoRedBlueAction->data());
  mStereoTypeComboBox->blockSignals(false);

  int stereoType = settings()->value("View3D/stereoType").toInt();
  mStereoTypeComboBox->setCurrentIndex(stereoType);
}
void VisualizationTab::stereoTypeChangedSlot(int index)
{
  QList<QAction*> actions = mStereoTypeActionGroup->actions();
  if (index<0 || index>=actions.size())
    return;
  actions[index]->trigger();
}
void VisualizationTab::stereoFrameSequentialSlot()
{
  settings()->setValue("View3D/stereoType", stFRAME_SEQUENTIAL);
}
void VisualizationTab::stereoInterlacedSlot()
{
  settings()->setValue("View3D/stereoType", stINTERLACED);
}
void VisualizationTab::stereoDresdenSlot()
{
  settings()->setValue("View3D/stereoType", stDRESDEN);
}
void VisualizationTab::stereoRedBlueSlot()
{
  settings()->setValue("View3D/stereoType", stRED_BLUE);
}

void VisualizationTab::eyeAngleSlot()
{
  settings()->setValue("View3D/eyeAngle", mEyeAngleAdapter->getValue());
}

void VisualizationTab::saveParametersSlot()
{
  settings()->setValue("View3D/sphereRadius", mSphereRadius->getValue());
  settings()->setValue("View/showDataText", mShowDataText->getValue());
  settings()->setValue("View/showLabels", mShowLabels->getValue());
  settings()->setValue("View3D/labelSize", mLabelSize->getValue());
  settings()->setValue("View3D/annotationModelSize", mAnnotationModelSize->getValue());
  settings()->setValue("View3D/annotationModel", mAnnotationModel->getValue());
}

void VisualizationTab::setBackgroundColorSlot(QColor color)
{
  settings()->setValue("backgroundColor", color);
}

//==============================================================================
// AutomationTab
//------------------------------------------------------------------------------
AutomationTab::AutomationTab(QWidget *parent) :
    PreferencesTab(parent)
{}

void AutomationTab::init()
{
  bool autoStartTracking = settings()->value("Automation/autoStartTracking").toBool();
  mAutoStartTrackingCheckBox = new QCheckBox("Auto Start Tracking");
  mAutoStartTrackingCheckBox->setChecked(autoStartTracking);

  bool autoStartStreaming = settings()->value("Automation/autoStartStreaming").toBool();
  mAutoStartStreamingCheckBox = new QCheckBox("Auto Start Streaming");
  mAutoStartStreamingCheckBox->setChecked(autoStartStreaming);

  bool autoReconstruct = settings()->value("Automation/autoReconstruct").toBool();
  mAutoReconstructCheckBox = new QCheckBox("Auto Reconstruct");
  mAutoReconstructCheckBox->setChecked(autoReconstruct);

  bool autoSelectDominantTool = settings()->value("Automation/autoSelectDominantTool").toBool();
  mAutoSelectDominantToolCheckBox = new QCheckBox("Auto Select Active Tool");
  mAutoSelectDominantToolCheckBox->setChecked(autoSelectDominantTool);

  //Layout
  mMainLayout = new QVBoxLayout;
  mMainLayout->addWidget(mAutoStartTrackingCheckBox);
  mMainLayout->addWidget(mAutoStartStreamingCheckBox);
  mMainLayout->addWidget(mAutoReconstructCheckBox);
  mMainLayout->addWidget(mAutoSelectDominantToolCheckBox);

  mTopLayout->addLayout(mMainLayout);

}

void AutomationTab::saveParametersSlot()
{
  settings()->setValue("Automation/autoStartTracking", mAutoStartTrackingCheckBox->isChecked());
  settings()->setValue("Automation/autoStartStreaming", mAutoStartStreamingCheckBox->isChecked());
  settings()->setValue("Automation/autoReconstruct", mAutoReconstructCheckBox->isChecked());
  settings()->setValue("Automation/autoSelectDominantTool", mAutoSelectDominantToolCheckBox->isChecked());
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
  mAcquisitionNameLineEdit = new QLineEdit(settings()->value("Ultrasound/acquisitionName").toString());
  acqNameLayout->addWidget(mAcquisitionNameLineEdit);

  bool bw = settings()->value("Ultrasound/8bitAcquisitionData").toBool();

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
  settings()->setValue("Ultrasound/acquisitionName", mAcquisitionNameLineEdit->text());
  settings()->setValue("Ultrasound/8bitAcquisitionData", m8bitRadioButton->isChecked());
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

  connect(stateService()->getApplication().get(), SIGNAL(activeStateChanged()), this, SLOT(applicationChangedSlot()));

  connect(settings(), SIGNAL(valueChangedFor(QString)), this, SLOT(globalConfigurationFileChangedSlot(QString)));

  connect(mToolConfigureGroupBox, SIGNAL(toolSelected(QString)), mFilePreviewWidget, SLOT(previewFileSlot(QString)));
  connect(mToolFilterGroupBox, SIGNAL(toolSelected(QString)), mFilePreviewWidget, SLOT(previewFileSlot(QString)));

  this->applicationChangedSlot();
}

ToolConfigTab::~ToolConfigTab()
{}

void ToolConfigTab::init()
{
  QGroupBox* filepreviewGroupBox = new QGroupBox(this);
  filepreviewGroupBox->setTitle("Toolfile preview");
  QVBoxLayout* filepreviewLayout = new QVBoxLayout();
  filepreviewGroupBox->setLayout(filepreviewLayout);
  filepreviewLayout->addWidget(mFilePreviewWidget);

  //layout
  QGridLayout* layout = new QGridLayout;
  mTopLayout->addLayout(layout);

  layout->addWidget(mToolConfigureGroupBox, 0, 0, 1, 1);
  layout->addWidget(mToolFilterGroupBox, 0, 1, 1, 1);
  layout->addWidget(filepreviewGroupBox, 1, 0, 1, 2);

  mToolConfigureGroupBox->setCurrentlySelectedCofiguration(DataLocations::getToolConfigFilePath());
}

void ToolConfigTab::saveParametersSlot()
{
  QString newConfigFile = mToolConfigureGroupBox->requestSaveConfigurationSlot();

  if(newConfigFile.isEmpty())
    newConfigFile = mToolConfigureGroupBox->getCurrenctlySelectedConfiguration();

  // currentToolConfigFile
  QFile configFile(newConfigFile);
  QFileInfo info(configFile);
  if(!configFile.exists())
    return;

  settings()->setValue("toolConfigFile", info.fileName());
}

void ToolConfigTab::applicationChangedSlot()
{
  ssc::CLINICAL_APPLICATION clinicalApplication = string2enum<ssc::CLINICAL_APPLICATION>(stateService()->getApplication()->getActiveStateName());
  mToolConfigureGroupBox->setClinicalApplicationSlot(clinicalApplication);
  mToolFilterGroupBox->setClinicalApplicationSlot(clinicalApplication);
  mToolFilterGroupBox->setTrackingSystemSlot(ssc::tsPOLARIS);
}

void ToolConfigTab::globalConfigurationFileChangedSlot(QString key)
{
  if(key != "toolConfigFile")
    return;

  mToolConfigureGroupBox->setCurrentlySelectedCofiguration(DataLocations::getToolConfigFilePath());
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
  mTabWidget = new QStackedWidget;

  mButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel);

  this->addTab(new GeneralTab, tr("General"));
  this->addTab(new PerformanceTab, tr("Performance"));
  this->addTab(new AutomationTab, tr("Automation"));
  this->addTab(new VisualizationTab, tr("Visualization"));
  this->addTab(new VideoTab, tr("Video"));
  this->addTab(new ToolConfigTab, tr("Tool Configuration"));
  this->addTab(new LayoutEditorTab, tr("Layout editor"));
  this->addTab(new DebugTab, tr("Debug"));

  QPushButton* applyButton = mButtonBox->button(QDialogButtonBox::Apply);

  connect(mButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(mButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(applyButton, SIGNAL(clicked()), this, SLOT(applySlot()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  QHBoxLayout *tabLayout = new QHBoxLayout;
  QFrame* frame = new QFrame;
  frame->setLineWidth(3);
  frame->setFrameShape(QFrame::Panel);
  frame->setFrameShadow(QFrame::Sunken);
  frame->setLayout(new QVBoxLayout);

  tabLayout->addWidget(mToolBar);
  tabLayout->addWidget(frame);
  frame->layout()->addWidget(mTabWidget);
  mainLayout->addLayout(tabLayout);
  mainLayout->addWidget(mButtonBox);
  setLayout(mainLayout);

  mTabWidget->setCurrentIndex(0);

  mButtonBox->button(QDialogButtonBox::Ok)->setFocus();
}

PreferencesDialog::~PreferencesDialog()
{}

void PreferencesDialog::selectTabSlot()
{
  QAction* action = dynamic_cast<QAction*>(sender());
  if (!action)
    return;
  int val = action->data().toInt();
  mTabWidget->setCurrentIndex(val);
}

void PreferencesDialog::applySlot()
{
  emit applied();
}

void PreferencesDialog::addTab(PreferencesTab* widget, QString name)
{
  widget->init();
  connect(mButtonBox, SIGNAL(accepted()), widget, SLOT(saveParametersSlot()));
  connect(this, SIGNAL(applied()), widget, SLOT(saveParametersSlot()));

  QAction* action = new QAction(name, mActionGroup);
  action->setData(mTabWidget->count());
  action->setCheckable(true);
  if (!mTabWidget->count())
    action->setChecked(true);
  connect(action, SIGNAL(triggered()), this, SLOT(selectTabSlot()));
  QToolButton* button = new QToolButton(this);

  button->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

  button->setDefaultAction(action);
  mToolBar->addWidget(button);

  mTabWidget->addWidget(widget);
}

//==============================================================================
// UltrasoundTab
//------------------------------------------------------------------------------
DebugTab::DebugTab(QWidget *parent) :
    PreferencesTab(parent)
{}

void DebugTab::init()
{
	mIGSTKDebugLoggingCheckBox = new QCheckBox("IGSTK Debug Logging");
	mIGSTKDebugLoggingCheckBox->setChecked(settings()->value("IGSTKDebugLogging", true).toBool());
	mIGSTKDebugLoggingCheckBox->setToolTip("Enables a large amount of logging in IGSTK (need restart)");

	mManualToolPhysicalPropertiesCheckBox = new QCheckBox("Debug manual tool");
	mManualToolPhysicalPropertiesCheckBox->setChecked(settings()->value("giveManualToolPhysicalProperties", true).toBool());
	mManualToolPhysicalPropertiesCheckBox->setToolTip("give manual tool the properties of the first physical tool. \nUse to simulate f.ex. probes with manual tool. (need restart)");

	//Layout
	mMainLayout = new QGridLayout;
	int i=0;
	mMainLayout->addWidget(mIGSTKDebugLoggingCheckBox, i++, 0);
	mMainLayout->addWidget(mManualToolPhysicalPropertiesCheckBox, i++, 0);

	mTopLayout->addLayout(mMainLayout);
}

void DebugTab::saveParametersSlot()
{
	settings()->setValue("IGSTKDebugLogging", mIGSTKDebugLoggingCheckBox->isChecked());
	settings()->setValue("giveManualToolPhysicalProperties", mManualToolPhysicalPropertiesCheckBox->isChecked());
}

}//namespace cx
