/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include <QtWidgets>

#include <QAction>

#include <iostream>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include "cxLogger.h"
#include "cxDoubleWidgets.h"
#include "cxEnumConverter.h"
#include "cxSettings.h"
#include "cxPreferencesDialog.h"
#include "cxDataLocations.h"
#include "cxTrackingService.h"
#include "cxStateService.h"
#include "cxFilePreviewWidget.h"
#include "cxToolImagePreviewWidget.h"
#include "cxToolConfigureWidget.h"
#include "cxToolFilterWidget.h"
#include "cxColorSelectButton.h"
#include "cxHelperWidgets.h"
#include "cxPatientModelService.h"
#include "cxDummyTool.h"
#include "cxImage.h"
#include "cxProfile.h"
#include "cxOperatingTableTab.h"
#include "cxLogicManager.h"
#include "cxVisServices.h"

namespace cx
{

VisualizationTab::VisualizationTab(PatientModelServicePtr patientModelService, QWidget *parent) :
		PreferenceTab(parent), mStereoTypeActionGroup(NULL)
{
	this->setObjectName("preferences_visualization_widget");
	mPatientModelService = patientModelService;
	mMainLayout = NULL;
	mStereoTypeComboBox = NULL;
	mStereoTypeActionGroup = NULL;
}


void VisualizationTab::init()
{
  double sphereRadius = settings()->value("View3D/sphereRadius").toDouble();
  mSphereRadius = DoubleProperty::initialize("SphereRadius", "Sphere Radius", "Radius of sphere markers in the 3D scene.", sphereRadius, DoubleRange(0.1,10,0.1), 1, QDomNode());

  double labelSize = settings()->value("View3D/labelSize").toDouble();
  mLabelSize = DoubleProperty::initialize("LabelSize", "Label Size", "Size of text labels in the 3D scene.", labelSize, DoubleRange(0.1,100,0.1), 1, QDomNode());

  SelectColorSettingButton* backgroundColorButton =
		  new SelectColorSettingButton("Background Color",
									   "backgroundColor",
									   "Set 3D view background color");

  SelectColorSettingButton* tool2DColor =
		  new SelectColorSettingButton("Tool Color 2D",
									   "View2D/toolColor",
									   "Set the color of the tool in 2D");
  SelectColorSettingButton* toolTipPointColor =
		  new SelectColorSettingButton("Tool Tip 2D/3D",
									   "View/toolTipPointColor",
									   "Set the color of the tool tip in 2D/3D");
  SelectColorSettingButton* toolOffsetPointColor =
		  new SelectColorSettingButton("Offset Point 2D/3D",
									   "View/toolOffsetPointColor",
									   "Set the color of the tool offset point in 2D/3D");
  SelectColorSettingButton* toolOffsetLineColor =
		  new SelectColorSettingButton("Offset Line 2D/3D",
									   "View/toolOffsetLineColor",
									   "Set the color of the tool offset line in 2D/3D");
  SelectColorSettingButton* toolCrossHairColor =
		  new SelectColorSettingButton("Crosshair 2D",
									   "View2D/toolCrossHairColor",
									   "Set the color of the tool 2D crosshair");

  bool showDataText = settings()->value("View/showDataText").value<bool>();
  mShowDataText = BoolProperty::initialize("Show Data Text", "",
                                                 "Show the name of each data set in the views.",
                                                 showDataText);
  bool showLabels = settings()->value("View/showLabels").value<bool>();
  mShowLabels = BoolProperty::initialize("Show Labels", "",
                                                 "Attach name labels to entities in the views.",
                                                 showLabels);

  bool toolCrosshair = settings()->value("View2D/showToolCrosshair").value<bool>();
  mToolCrosshair = BoolProperty::initialize("Tool 2D Crosshair", "",
										 "Show a crosshair centered on the tool in the orthogonal (ACS) views.",
										 toolCrosshair);


  bool showMetricNamesInCorner = settings()->value("View/showMetricNamesInCorner").value<bool>();
  mShowMetricNamesInCorner = BoolProperty::initialize("Corner Metrics", "",
												 "Show the metric data in the upper right corner of the view instead of in the scene.",
												 showMetricNamesInCorner);


  double annotationModelSize = settings()->value("View3D/annotationModelSize").toDouble();
  mAnnotationModelSize = DoubleProperty::initialize("AnnotationModelSize", "Annotation Model Size", "Size (0..1) of the annotation model in the 3D scene.", annotationModelSize, DoubleRange(0.01,1,0.01), 2, QDomNode());
  QStringList annotationModelRange;
  foreach(QString path, DataLocations::getRootConfigPaths())
  {
	  annotationModelRange << QDir(path+"/models/").entryList(QStringList()<<"*.stl");
  }
  annotationModelRange.prepend("<default>");
  QString annotationModel = settings()->value("View3D/annotationModel").toString();
  mAnnotationModel = StringProperty::initialize("AnnotationModel", "Annotation Model", "Name of annotation model in the 3D scene.", annotationModel, annotationModelRange, QDomNode());

  //Stereoscopic visualization (3D view)
  QGroupBox* stereoGroupBox = new QGroupBox("Stereoscopic visualization");
  mStereoTypeComboBox = new QComboBox();
  connect(mStereoTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(stereoTypeChangedSlot(int)));
  this->initStereoTypeComboBox();
  double eyeAngle = settings()->value("View3D/eyeAngle").toDouble();
  mEyeAngleAdapter = DoubleProperty::initialize("Eye angle (degrees)", "",
      "Separation between eyes in degrees",
	  eyeAngle, DoubleRange(0, 25, 0.1), 1);
  connect(mEyeAngleAdapter.get(), SIGNAL(valueWasSet()), this, SLOT(eyeAngleSlot()));

  double anyplaneViewOffset = settings()->value("Navigation/anyplaneViewOffset").toDouble();
  mAnyplaneViewOffset = DoubleProperty::initialize("AnyplaneViewOffset",
														 "View Offset",
														 "Position of virtual tool tip in anyplane view, % from top.",
														 anyplaneViewOffset, DoubleRange(-0.5,0.5,0.05), 2, QDomNode());
  mAnyplaneViewOffset->setInternal2Display(100);

  bool followTooltip = settings()->value("Navigation/followTooltip").value<bool>();
  mFollowTooltip = BoolProperty::initialize("Views Follow Tool", "",
												 "ACS Views follow the virtual tool tip",
												 followTooltip);
  double followTooltipBoundary = settings()->value("Navigation/followTooltipBoundary").toDouble();
  mFollowTooltipBoundary = DoubleProperty::initialize("FollowTooltipBoundary",
														 "Follow Tool Boundary",
														 "Boundary in ACS Views where follow tool tip is applied. % of view size",
														 followTooltipBoundary, DoubleRange(0.0,0.5,0.05), 2, QDomNode());
  mFollowTooltipBoundary->setInternal2Display(100);


  QStringList clinicalViews;
  for (unsigned i=0; i<mdCOUNT; ++i)
	  clinicalViews << enum2string<CLINICAL_VIEW>(CLINICAL_VIEW(i));
  mClinicalView = StringProperty::initialize("ClinicalView", "Clinical View",
											 "Type of clinical view",
											 enum2string<CLINICAL_VIEW>(mPatientModelService->getClinicalApplication()),
											 clinicalViews, QDomNode());


  QVBoxLayout* stereoLayout = new QVBoxLayout();
  stereoLayout->addWidget(mStereoTypeComboBox);
  stereoLayout->addWidget(new SpinBoxAndSliderGroupWidget(this, mEyeAngleAdapter));
  stereoGroupBox->setLayout(stereoLayout);

  QHBoxLayout* toolcolors = new QHBoxLayout;
  toolcolors->addWidget(tool2DColor);
  toolcolors->addWidget(toolTipPointColor);
  toolcolors->addWidget(toolOffsetPointColor);
  toolcolors->addWidget(toolOffsetLineColor);
  toolcolors->addWidget(toolCrossHairColor);

  //Layout
  mMainLayout = new QGridLayout;
  int counter = 0;
  mMainLayout->addWidget(backgroundColorButton, counter++, 0);
  mMainLayout->addWidget(sscCreateDataWidget(this, mClinicalView), counter++, 0);
  mMainLayout->addWidget(new SpinBoxGroupWidget(this, mSphereRadius), counter++, 0);
  mMainLayout->addWidget(sscCreateDataWidget(this, mShowDataText), counter++, 0);
  mMainLayout->addWidget(sscCreateDataWidget(this, mShowLabels), counter++, 0);
  mMainLayout->addWidget(sscCreateDataWidget(this, mToolCrosshair), counter++, 0);
  mMainLayout->addWidget(sscCreateDataWidget(this, mShowMetricNamesInCorner), counter++, 0);
  mMainLayout->addWidget(new SpinBoxGroupWidget(this, mLabelSize), counter++, 0);
  mMainLayout->addWidget(new SpinBoxGroupWidget(this, mAnnotationModelSize), counter++, 0);
  mMainLayout->addWidget(sscCreateDataWidget(this, mAnnotationModel), counter++, 0);
  mMainLayout->addWidget(sscCreateDataWidget(this, mAnyplaneViewOffset), counter++, 0);
  mMainLayout->addWidget(sscCreateDataWidget(this, mFollowTooltip), counter++, 0);
  mMainLayout->addWidget(sscCreateDataWidget(this, mFollowTooltipBoundary), counter++, 0);

  mMainLayout->addLayout(toolcolors, counter++, 0);
  mMainLayout->addWidget(stereoGroupBox, counter++, 0);

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
	mPatientModelService->setClinicalApplication(string2enum<CLINICAL_VIEW>(mClinicalView->getValue()));
  settings()->setValue("View3D/sphereRadius", mSphereRadius->getValue());
  settings()->setValue("View/showDataText", mShowDataText->getValue());
  settings()->setValue("View/showLabels", mShowLabels->getValue());
  settings()->setValue("View2D/showToolCrosshair", mToolCrosshair->getValue());
  settings()->setValue("View/showMetricNamesInCorner", mShowMetricNamesInCorner->getValue());
  settings()->setValue("View3D/labelSize", mLabelSize->getValue());
  settings()->setValue("View3D/annotationModelSize", mAnnotationModelSize->getValue());
  settings()->setValue("View3D/annotationModel", mAnnotationModel->getValue());
  settings()->setValue("Navigation/anyplaneViewOffset", mAnyplaneViewOffset->getValue());
  settings()->setValue("Navigation/followTooltip", mFollowTooltip->getValue());
  settings()->setValue("Navigation/followTooltipBoundary", mFollowTooltipBoundary->getValue());
}

//==============================================================================
// AutomationTab
//------------------------------------------------------------------------------
AutomationTab::AutomationTab(QWidget *parent) :
		PreferenceTab(parent)
{
	this->setObjectName("preferences_automation_widget");
	mAutoSelectActiveToolCheckBox = NULL;
	mAutoStartTrackingCheckBox = NULL;
	mAutoStartStreamingCheckBox = NULL;
	mAutoReconstructCheckBox = NULL;
	mAutoSaveCheckBox = NULL;
	mAutoLoadPatientCheckBox = NULL;
	mAutoShowNewDataCheckBox = NULL;
    mAutoDeleteDICOMDBCheckBox = NULL;
	mMainLayout = NULL;
}

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

  bool autoSelectActiveTool = settings()->value("Automation/autoSelectActiveTool").toBool();
  mAutoSelectActiveToolCheckBox = new QCheckBox("Auto Select Active Tool");
  mAutoSelectActiveToolCheckBox->setToolTip(""
	  "Automatically select an active tool when a tool becomes visible");
  mAutoSelectActiveToolCheckBox->setChecked(autoSelectActiveTool);

  bool autoSave = settings()->value("Automation/autoSave").toBool();
  mAutoSaveCheckBox = new QCheckBox("Auto Save");
  mAutoSaveCheckBox->setToolTip(""
	  "Save patient after major events,\n"
	  "such as workflow step change, registration, reconstruction.");
  mAutoSaveCheckBox->setChecked(autoSave);

  bool autoShow = settings()->value("Automation/autoShowNewData").toBool();
  mAutoShowNewDataCheckBox = new QCheckBox("Auto Show New Data");
  mAutoShowNewDataCheckBox->setToolTip(""
	  "Show new data in the first view.\n"
	  "Occors after load data and reconstruct.");
  mAutoShowNewDataCheckBox->setChecked(autoShow);

  bool autoLoadPatient = settings()->value("Automation/autoLoadRecentPatient").toBool();
  mAutoLoadPatientCheckBox = new QCheckBox("Auto Load Recent Patient");
  mAutoLoadPatientCheckBox->setToolTip("Load the last saved patient if within a chosen number of hours.");
  mAutoLoadPatientCheckBox->setChecked(autoLoadPatient);

  bool autoDeleteDICOMDB = settings()->value("Automation/autoDeleteDICOMDatabase").toBool();
  mAutoDeleteDICOMDBCheckBox = new QCheckBox("Auto Delete DICOM Database");
  mAutoDeleteDICOMDBCheckBox->setToolTip("Delete the DICOM database on shutdown.");
  mAutoDeleteDICOMDBCheckBox->setChecked(autoDeleteDICOMDB);

  double autoLoadPatientWithinHours = settings()->value("Automation/autoLoadRecentPatientWithinHours").toDouble();
  mAutoLoadPatientWithinHours = DoubleProperty::initialize("Auto load within hours", "Auto load within hours", "Load the last patient if within this number of hours (and auto load is enabled)", autoLoadPatientWithinHours, DoubleRange(0.1,1000,0.1), 1, QDomNode());


  //Layout
  mMainLayout = new QVBoxLayout;
  mMainLayout->addWidget(mAutoStartTrackingCheckBox);
  mMainLayout->addWidget(mAutoStartStreamingCheckBox);
  mMainLayout->addWidget(mAutoReconstructCheckBox);
  mMainLayout->addWidget(mAutoSelectActiveToolCheckBox);
  mMainLayout->addWidget(mAutoSaveCheckBox);
  mMainLayout->addWidget(mAutoShowNewDataCheckBox);
  mMainLayout->addWidget(mAutoLoadPatientCheckBox);
  mMainLayout->addWidget(mAutoDeleteDICOMDBCheckBox);
  mMainLayout->addWidget(new SpinBoxGroupWidget(this, mAutoLoadPatientWithinHours));

  mTopLayout->addLayout(mMainLayout);

}

void AutomationTab::saveParametersSlot()
{
  settings()->setValue("Automation/autoStartTracking", mAutoStartTrackingCheckBox->isChecked());
  settings()->setValue("Automation/autoStartStreaming", mAutoStartStreamingCheckBox->isChecked());
  settings()->setValue("Automation/autoReconstruct", mAutoReconstructCheckBox->isChecked());
  settings()->setValue("Automation/autoSelectActiveTool", mAutoSelectActiveToolCheckBox->isChecked());
  settings()->setValue("Automation/autoSave", mAutoSaveCheckBox->isChecked());
  settings()->setValue("Automation/autoShowNewData", mAutoShowNewDataCheckBox->isChecked());
  settings()->setValue("Automation/autoLoadRecentPatient", mAutoLoadPatientCheckBox->isChecked());
  settings()->setValue("Automation/autoLoadRecentPatientWithinHours", mAutoLoadPatientWithinHours->getValue());
  settings()->setValue("Automation/autoDeleteDICOMDatabase", mAutoDeleteDICOMDBCheckBox->isChecked());
}

//==============================================================================
// UltrasoundTab
//------------------------------------------------------------------------------
VideoTab::VideoTab(QWidget *parent) :
		PreferenceTab(parent)
{
	this->setObjectName("preferences_video_widget");
	mAcquisitionNameLineEdit = NULL;
	mMainLayout = NULL;
	m24bitRadioButton = NULL;
	m8bitRadioButton = NULL;
	mCompressCheckBox = NULL;

}

void VideoTab::init()
{
  QVBoxLayout* toplayout = new QVBoxLayout;
  QHBoxLayout* acqNameLayout = new QHBoxLayout;
  toplayout->addLayout(acqNameLayout);

  acqNameLayout->addWidget(new QLabel("Name prefix"));
  mAcquisitionNameLineEdit = new QLineEdit(settings()->value("Ultrasound/acquisitionName").toString());
  mAcquisitionNameLineEdit->setToolTip("Prefix to name for grabbed video.");
  acqNameLayout->addWidget(mAcquisitionNameLineEdit);

  bool bw = settings()->value("Ultrasound/8bitAcquisitionData").toBool();

  m24bitRadioButton = new QRadioButton("Save acquisition as 24bit", this);
  m24bitRadioButton->setChecked(!bw);
  m24bitRadioButton->setToolTip("Convert color video to 24 bit color. If the video already is 8 bit grayscale it will not be converted to 24 bit");
  m8bitRadioButton = new QRadioButton("Save acquisition as 8bit", this);
  m8bitRadioButton->setChecked(bw);
  m8bitRadioButton->setToolTip("Convert acquired video to 8 bit grayscale");

	mCompressCheckBox = new QCheckBox("Compress acquisition data");
	mCompressCheckBox->setChecked(settings()->value("Ultrasound/CompressAcquisition", true).toBool());
	mCompressCheckBox->setToolTip("Store the US Acquisition data as compressed MHD");

  toplayout->addSpacing(5);
  toplayout->addWidget(m24bitRadioButton);
  toplayout->addWidget(m8bitRadioButton);
  toplayout->addWidget(mCompressCheckBox);

  mTopLayout->addLayout(toplayout);

}

void VideoTab::saveParametersSlot()
{
  settings()->setValue("Ultrasound/acquisitionName", mAcquisitionNameLineEdit->text());
  settings()->setValue("Ultrasound/8bitAcquisitionData", m8bitRadioButton->isChecked());
  settings()->setValue("Ultrasound/CompressAcquisition", mCompressCheckBox->isChecked());
}

//==============================================================================
// ToolConfigTab
//------------------------------------------------------------------------------

ToolConfigTab::ToolConfigTab(StateServicePtr stateService, TrackingServicePtr trackingService, QWidget* parent) :
	PreferenceTab(parent),
	mFilePreviewWidget(new FilePreviewWidget(this)),
	mImagePreviewWidget(new ToolImagePreviewWidget(trackingService, this)),
	mStateService(stateService)
{
	this->setObjectName("preferences_tool_config_widget");
  mToolConfigureGroupBox = new ToolConfigureGroupBox(trackingService, stateService, this);
  mToolFilterGroupBox  = new ToolFilterGroupBox(trackingService, this);
  mToolFilterGroupBox->setTrackingSystemSelector(mToolConfigureGroupBox->getTrackingSystemSelector());

  connect(stateService.get(), &StateService::applicationStateChanged, this, &ToolConfigTab::applicationChangedSlot);

  connect(settings(), SIGNAL(valueChangedFor(QString)), this, SLOT(globalConfigurationFileChangedSlot(QString)));

  connect(mToolConfigureGroupBox, SIGNAL(toolSelected(QString)), mFilePreviewWidget, SLOT(previewFileSlot(QString)));
  connect(mToolFilterGroupBox, SIGNAL(toolSelected(QString)), mFilePreviewWidget, SLOT(previewFileSlot(QString)));

  connect(mToolConfigureGroupBox, SIGNAL(toolSelected(QString)), mImagePreviewWidget, SLOT(previewFileSlot(QString)));
  connect(mToolFilterGroupBox, SIGNAL(toolSelected(QString)), mImagePreviewWidget, SLOT(previewFileSlot(QString)));

  this->applicationChangedSlot();
}

ToolConfigTab::~ToolConfigTab()
{}

void ToolConfigTab::init()
{
  QGroupBox* filepreviewGroupBox = new QGroupBox(this);
//  filepreviewGroupBox->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
//  mFilePreviewWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

  filepreviewGroupBox->setTitle("Toolfile preview");
  QHBoxLayout* filepreviewLayout = new QHBoxLayout();
  filepreviewGroupBox->setLayout(filepreviewLayout);
//  filepreviewLayout->setMargin(0);
  filepreviewLayout->addWidget(mFilePreviewWidget);

  QGroupBox* imagepreviewGroupBox = new QGroupBox(this);
  imagepreviewGroupBox->setTitle("Tool image preview");
  QVBoxLayout* imagepreviewLayout = new QVBoxLayout();
  imagepreviewGroupBox->setLayout(imagepreviewLayout);
  imagepreviewLayout->setMargin(0);
  imagepreviewLayout->addWidget(mImagePreviewWidget);

  //layout
  QGridLayout* layout = new QGridLayout;
  mTopLayout->addLayout(layout);

  layout->addWidget(mToolConfigureGroupBox, 0, 0, 1, 2);
  layout->addWidget(mToolFilterGroupBox, 0, 2, 1, 2);
  layout->addWidget(filepreviewGroupBox, 1, 0, 1, 3);
  layout->addWidget(imagepreviewGroupBox, 1, 3, 1, 1);
//  layout->setRowStretch(0, 1);
//  layout->setRowStretch(2, 1);

  mToolConfigureGroupBox->setCurrentlySelectedCofiguration(profile()->getToolConfigFilePath());
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

//  profile()->setToolConfigFilePath(info.fileName());
  settings()->setValue("toolConfigFile", info.fileName());
}

void ToolConfigTab::applicationChangedSlot()
{
  mToolFilterGroupBox->setClinicalApplicationSlot(mStateService->getApplicationStateName());
}

void ToolConfigTab::globalConfigurationFileChangedSlot(QString key)
{
  if(key != "toolConfigFile")
    return;

  mToolConfigureGroupBox->setCurrentlySelectedCofiguration(profile()->getToolConfigFilePath());
}

//==============================================================================
// PreferencesDialog
//------------------------------------------------------------------------------

PreferencesDialog::PreferencesDialog(ViewServicePtr viewService, PatientModelServicePtr patientModelService, StateServicePtr stateService, TrackingServicePtr trackingService, QWidget *parent) :
  QDialog(parent)
{
  mActionGroup = new QActionGroup(this);
  mActionGroup->setExclusive(true);
  mToolBar =  new QToolBar;
  mToolBar->setOrientation(Qt::Vertical);
  mTabWidget = new QStackedWidget;

  mButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel);
  VisServicesPtr services = VisServices::create(logicManager()->getPluginContext());

  this->addTab(new GeneralTab(viewService, patientModelService), tr("General"));
  this->addTab(new PerformanceTab, tr("Performance"));
  this->addTab(new AutomationTab, tr("Automation"));
  this->addTab(new VisualizationTab(patientModelService), tr("Visualization"));
  this->addTab(new VideoTab, tr("Video"));
  this->addTab(new ToolConfigTab(stateService, trackingService), tr("Tool Configuration"));
  this->addTab(new OperatingTableTab(services), tr("Table"));
  this->addTab(new DebugTab(patientModelService, trackingService), tr("Debug"));

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

void PreferencesDialog::addTab(PreferenceTab* widget, QString name)
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
DebugTab::DebugTab(PatientModelServicePtr patientModelService, TrackingServicePtr trackingService, QWidget *parent) :
	PreferenceTab(parent),
	mIGSTKDebugLoggingCheckBox(NULL),
	mManualToolPhysicalPropertiesCheckBox(NULL),
	mRenderSpeedLoggingCheckBox(NULL),
	mMainLayout(NULL),
	mPatientModelService(patientModelService),
	mTrackingService(trackingService)
{
	this->setObjectName("preferences_debug_widget");
}

void DebugTab::init()
{
	mIGSTKDebugLoggingCheckBox = new QCheckBox("IGSTK Debug Logging");
	mIGSTKDebugLoggingCheckBox->setChecked(settings()->value("IGSTKDebugLogging", true).toBool());
	mIGSTKDebugLoggingCheckBox->setToolTip("Enables a large amount of logging in IGSTK (need restart)");

	mManualToolPhysicalPropertiesCheckBox = new QCheckBox("Debug manual tool");
	mManualToolPhysicalPropertiesCheckBox->setChecked(settings()->value("giveManualToolPhysicalProperties", true).toBool());
	mManualToolPhysicalPropertiesCheckBox->setToolTip("give manual tool the properties of the first physical tool. \nUse to simulate f.ex. probes with manual tool. (need restart)");

	QPushButton* runDebugToolButton = new QPushButton("Run Debug Tool", this);
	runDebugToolButton->setToolTip("Start a dummy tool that runs in a deterministic pattern inside the bounding box of the first found volume.");
	connect(runDebugToolButton, SIGNAL(clicked()), this, SLOT(runDebugToolSlot()));

	mRenderSpeedLoggingCheckBox = new QCheckBox("Render Speed Logging");
	mRenderSpeedLoggingCheckBox->setChecked(settings()->value("renderSpeedLogging", true).toBool());
	mRenderSpeedLoggingCheckBox->setToolTip("Dump render speed statistics to the console");

	//Layout
	mMainLayout = new QGridLayout;
	int i=0;
	mMainLayout->addWidget(mIGSTKDebugLoggingCheckBox, i++, 0);
	mMainLayout->addWidget(mManualToolPhysicalPropertiesCheckBox, i++, 0);
	mMainLayout->addWidget(runDebugToolButton, i++, 0);
	mMainLayout->addWidget(mRenderSpeedLoggingCheckBox, i++, 0);

	mTopLayout->addLayout(mMainLayout);
}

void DebugTab::runDebugToolSlot()
{
	if (!mPatientModelService->getDatas().size())
		return;

	cx::ImagePtr image = mPatientModelService->getDataOfType<Image>().begin()->second;
	cx::DoubleBoundingBox3D bb_r = transform(image->get_rMd(), image->boundingBox());

	mPatientModelService->setCenter(bb_r.center());

	cx::DummyToolPtr dummyTool(new cx::DummyTool());
	dummyTool->setType(Tool::TOOL_POINTER);
	dummyTool->setToolPositionMovement(dummyTool->createToolPositionMovementTranslationOnly(bb_r));
	report(QString("Running debug tool inside box %1").arg(qstring_cast(bb_r)));
	mTrackingService->runDummyTool(dummyTool);
}

void DebugTab::saveParametersSlot()
{
	settings()->setValue("IGSTKDebugLogging", mIGSTKDebugLoggingCheckBox->isChecked());
	settings()->setValue("giveManualToolPhysicalProperties", mManualToolPhysicalPropertiesCheckBox->isChecked());
	settings()->setValue("renderSpeedLogging", mRenderSpeedLoggingCheckBox->isChecked());
}

}//namespace cx
