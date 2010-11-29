#include "cxToolTipCalibrationWidget.h"

#include <QPushButton>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "sscToolManager.h"
#include "sscDataManager.h"
#include "sscVector3D.h"
#include "sscDefinitionStrings.h"
#include "sscLabeledComboBoxWidget.h"
#include "cxDataLocations.h"
#include "cxTool.h"
#include "cxStateMachineManager.h"
#include "cxPatientData.h"

namespace cx
{
ToolTipCalibrationWidget::ToolTipCalibrationWidget(QWidget* parent) :
  WhatsThisWidget(parent),
  mCalibrationWidget(new ToolTipCalibrateWidget(this)),
  mSampleWidget(new ToolTipSampleWidget(this))
{
  this->setObjectName("ToolTipCalibrationWidget");
  this->setWindowTitle("Tool Tip");

  QVBoxLayout* toptopLayout = new QVBoxLayout(this);
  QGridLayout* topLayout = new QGridLayout();

  QGroupBox* calibrateGroupBox = new QGroupBox("Calibrate dominant tool", this);
  calibrateGroupBox->setLayout(new QVBoxLayout());
  calibrateGroupBox->layout()->addWidget(mCalibrationWidget);
  QGroupBox* sampleGroupBox = new QGroupBox("Sample dominant tool", this);
  sampleGroupBox->setLayout(new QVBoxLayout());
  sampleGroupBox->layout()->addWidget(mSampleWidget);

  toptopLayout->addLayout(topLayout);
  toptopLayout->addStretch();
  topLayout->addWidget(calibrateGroupBox, 1, 0);
  topLayout->addWidget(sampleGroupBox, 2, 0);
}

ToolTipCalibrationWidget::~ToolTipCalibrationWidget()
{
}

QString ToolTipCalibrationWidget::defaultWhatsThis() const
{
  return "<html>"
    "<h3>Tool tip calibration.</h3>"
    "<p><i>Calibrates a tool by sampling it when pointing at a known point on another frame.</i></br>"
    "By using the test button you can test your calibration by pointing at a known reference point.</br>"
    "You can sample the dominant(active) tools tooltip in any coordinate system and get the results written to file.</p>"
    "</html>";
}

void ToolTipCalibrationWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
}

void ToolTipCalibrationWidget::hideEvent(QHideEvent* event)
{
  QWidget::hideEvent(event);
}
//------------------------------------------------------------------------------
ToolTipCalibrateWidget::ToolTipCalibrateWidget(QWidget* parent) :
    WhatsThisWidget(parent),
    mCalibrateButton(new QPushButton("Calibrate")),
    mReferencePointLabel(new QLabel("Ref. point:")),
    mTestButton(new QPushButton("Test calibration")),
    mCalibrationLabel(new QLabel("Calibration: \n")),
    mDeltaLabel(new QLabel("Delta:"))
{
  this->setObjectName("ToolTipCalibrateWidget");
  this->setWindowTitle("Tool Tip Calibration");

  QVBoxLayout* toplayout = new QVBoxLayout(this);

  mTools = SelectToolStringDataAdapter::New();
  mCalibrateToolComboBox = new ssc::LabeledComboBoxWidget(this, mTools);

  toplayout->addWidget(new QLabel("<b>Select a tool with a known referece point:</b>"));
  toplayout->addWidget(mCalibrateToolComboBox);
  toplayout->addWidget(mReferencePointLabel);
  toplayout->addWidget(mCalibrateButton);
  toplayout->addWidget(mCalibrationLabel);
  toplayout->addWidget(this->createHorizontalLine());
  toplayout->addWidget(mTestButton);
  toplayout->addWidget(mDeltaLabel);
  toplayout->addStretch();

  connect(mCalibrateButton, SIGNAL(clicked()), this, SLOT(calibrateSlot()));
  connect(mTestButton, SIGNAL(clicked()), this, SLOT(testCalibrationSlot()));

  connect(mTools.get(), SIGNAL(changed()), this, SLOT(toolSelectedSlot()));

  //setting default state
  this->toolSelectedSlot();
}

ToolTipCalibrateWidget::~ToolTipCalibrateWidget()
{}

QString ToolTipCalibrateWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Tool tip calibration.</h3>"
      "<p><i>Calibrates a tool by sampling it when pointing at a known point on another frame.</i></br>"
      "By using the test button you can test your calibration by pointing at a known reference point.</br></p>"
      "</html>";
}

void ToolTipCalibrateWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
}

void ToolTipCalibrateWidget::hideEvent(QHideEvent* event)
{
  QWidget::hideEvent(event);
}

void ToolTipCalibrateWidget::calibrateSlot()
{
  ssc::ToolPtr refTool = mTools->getTool();
  //Todo, we only allow the reference point with id 1 to be used to calibrate
  //this could be done more dynamic.
  if(!refTool || !refTool->hasReferencePointWithId(1))
    return;

  ssc::ToolPtr tool = ssc::toolManager()->getDominantTool();
  ssc::CoordinateSystem to = ssc::CoordinateSystemHelpers::getT(tool);
  ssc::Vector3D P_t = ssc::CoordinateSystemHelpers::getDominantToolTipPoint(to);

  ToolTipCalibrationCalculator calc(tool, refTool, P_t);
  ssc::Transform3D calibration = calc.get_calibration_sMt();

  QMessageBox msgBox;
  msgBox.setText("Do you want to overwrite "+tool->getName()+"s calibration file?");
  msgBox.setInformativeText("This cannot be undone.");
  msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::Ok);
  int ret = msgBox.exec();

  if(ret == QMessageBox::Ok)
  {
    tool->setCalibration_sMt(calibration);
    mCalibrationLabel->setText("Calibration:\n"+qstring_cast(calibration));
  }
}

void ToolTipCalibrateWidget::testCalibrationSlot()
{
  ssc::ToolPtr selectedTool = mTools->getTool();
  if(!selectedTool || !selectedTool->hasReferencePointWithId(1))
    return;

  ssc::CoordinateSystem to = ssc::CoordinateSystemHelpers::getT(ssc::toolManager()->getDominantTool());
  ssc::Vector3D sampledPoint = ssc::CoordinateSystemHelpers::getDominantToolTipPoint(to);

  ToolTipCalibrationCalculator calc(ssc::toolManager()->getDominantTool(), selectedTool, sampledPoint);
  ssc::Vector3D delta_selectedTool = calc.get_delta_ref();

  mDeltaLabel->setText("<b>Delta:</b> "+qstring_cast(delta_selectedTool)+" <br> <b>Length:</b>  "+qstring_cast(delta_selectedTool.length()));

  ssc::messageManager()->sendInfo("Delta: "+qstring_cast(delta_selectedTool)+" Length:   "+qstring_cast(delta_selectedTool.length()));
}

void ToolTipCalibrateWidget::toolSelectedSlot()
{
  QString text("Ref. point: <UNDEFINED POINT>");
  mCalibrateButton->setEnabled(false);
  mTestButton->setEnabled(false);

  if(mTools->getTool())
  {
    ToolPtr tool = boost::dynamic_pointer_cast<Tool>(mTools->getTool());
    if(tool && tool->hasReferencePointWithId(1))
    {
      text = "Ref. point: "+qstring_cast(tool->getReferencePoints()[1]);
      mCalibrateButton->setEnabled(true);
      mTestButton->setEnabled(true);
    }
    if(tool)
    {
      mCalibrationLabel->setText("Calibration:\n"+qstring_cast(tool->getCalibration_sMt()));
    }
  }

  mReferencePointLabel->setText(text);
}
  //------------------------------------------------------------------------------
ToolTipSampleWidget::ToolTipSampleWidget(QWidget* parent) :
    WhatsThisWidget(parent),
    mSampleButton(new QPushButton("Sample")),
    mSaveToFileNameLabel(new QLabel("<font color=red> No file selected </font>")),
    mSaveFileButton(new QPushButton("Save to...")),
    mTruncateFile(false)
{
  this->setObjectName("ToolTipSampleWidget");
  this->setWindowTitle("Tool Tip Sampling");

  QVBoxLayout* toplayout = new QVBoxLayout(this);

  mCoordinateSystems = SelectCoordinateSystemStringDataAdapter::New();
  mTools = SelectToolStringDataAdapter::New();
  mData = SelectDataStringDataAdapter::New();

  mCoordinateSystemComboBox = new ssc::LabeledComboBoxWidget(this, mCoordinateSystems);
  mToolComboBox = new ssc::LabeledComboBoxWidget(this, mTools);
  mDataComboBox = new ssc::LabeledComboBoxWidget(this, mData);

  toplayout->addWidget(mSaveFileButton);
  toplayout->addWidget(mSaveToFileNameLabel);
  toplayout->addWidget(this->createHorizontalLine());
  toplayout->addWidget(new QLabel("<b>Select coordinate system to sample in: </b>"));
  toplayout->addWidget(mCoordinateSystemComboBox);
  toplayout->addWidget(mToolComboBox);
  toplayout->addWidget(mDataComboBox);
  toplayout->addWidget(this->createHorizontalLine());
  toplayout->addWidget(mSampleButton);
  toplayout->addStretch();

  connect(mSaveFileButton, SIGNAL(clicked()), this, SLOT(saveFileSlot()));
  connect(mSampleButton, SIGNAL(clicked()), this, SLOT(sampleSlot()));
  connect(mCoordinateSystems.get(), SIGNAL(changed()), this, SLOT(coordinateSystemChanged()));

  //setting initial state
  this->coordinateSystemChanged();
}

ToolTipSampleWidget::~ToolTipSampleWidget()
{}

QString ToolTipSampleWidget::defaultWhatsThis() const
{
  return "<html>"
     "<h3>Tool tip sampling.</h3>"
     "<p>You can sample the dominant(active) tools tooltip in any coordinate system and get the results written to file.</p>"
     "</html>";
}

void ToolTipSampleWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
}

void ToolTipSampleWidget::hideEvent(QHideEvent* event)
{
  QWidget::hideEvent(event);
}

void ToolTipSampleWidget::saveFileSlot()
{
  QString configPath = DataLocations::getRootConfigPath();
  if(stateManager()->getPatientData()->isPatientValid())
    configPath = stateManager()->getPatientData()->getActivePatientFolder();

  QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                             configPath+"/SampledPoints.txt",
                             tr("Text (*.txt)"));
  if(fileName.isEmpty())
    return;
  else if(QFile::exists(fileName))
    mTruncateFile = true;

  mSaveToFileNameLabel->setText(fileName);
}

void ToolTipSampleWidget::sampleSlot()
{
  QFile samplingFile(mSaveToFileNameLabel->text());

  ssc::CoordinateSystem to = this->getSelectedCoordinateSystem();
  ssc::Vector3D toolPoint = ssc::CoordinateSystemHelpers::getDominantToolTipPoint(to, false);

  if(!samplingFile.open(QIODevice::WriteOnly | (mTruncateFile ? QIODevice::Truncate : QIODevice::Append)))
  {
    ssc::messageManager()->sendWarning("Could not open "+samplingFile.fileName());
    ssc::messageManager()->sendInfo("Sampled point: "+qstring_cast(toolPoint));
    return;
  }
  else
  {
    if(mTruncateFile)
      mTruncateFile = false;
  }

  QString sampledPoint = qstring_cast(toolPoint);

  QTextStream streamer(&samplingFile);
  streamer << sampledPoint;
  streamer << endl;

  ssc::messageManager()->sendInfo("Sampled point in "+qstring_cast(to.mId)+" ("+to.mRefObject+") space, result: "+sampledPoint);
}

void ToolTipSampleWidget::coordinateSystemChanged()
{
  switch (string2enum<ssc::COORDINATE_SYSTEM>(mCoordinateSystems->getValue()))
  {
  case ssc::csDATA:
    mDataComboBox->show();
    mToolComboBox->hide();
    break;
  case ssc::csTOOL:
    mToolComboBox->show();
    mDataComboBox->hide();
    break;
  case ssc::csSENSOR:
    mToolComboBox->show();
    mDataComboBox->hide();
    break;
  default:
    mDataComboBox->hide();
    mToolComboBox->hide();
    break;
  };
}

ssc::CoordinateSystem ToolTipSampleWidget::getSelectedCoordinateSystem()
{
  ssc::CoordinateSystem retval;

  retval.mId = string2enum<ssc::COORDINATE_SYSTEM>(mCoordinateSystems->getValue());

  switch (retval.mId)
  {
  case ssc::csDATA:
    retval = ssc::CoordinateSystemHelpers::getD(mData->getData());
    break;
  case ssc::csTOOL:
    retval = ssc::CoordinateSystemHelpers::getT(mTools->getTool());
    break;
  case ssc::csSENSOR:
    retval = ssc::CoordinateSystemHelpers::getT(mTools->getTool());
    break;
  default:
    retval.mRefObject = "";
    break;
  };

  return retval;
}
//------------------------------------------------------------------------------
ToolTipCalibrationCalculator::ToolTipCalibrationCalculator(ssc::ToolPtr tool, ssc::ToolPtr ref, ssc::Vector3D p_t) :
    mTool(tool), mRef(ref), mP_t(p_t)
{}

ToolTipCalibrationCalculator::~ToolTipCalibrationCalculator()
{}

ssc::Vector3D ToolTipCalibrationCalculator::get_delta_ref()
{
  return get_referencePoint_ref() - get_sampledPoint_ref();
}

ssc::Transform3D ToolTipCalibrationCalculator::get_calibration_sMt()
{
  return this->get_sMt_new();
}

ssc::Vector3D ToolTipCalibrationCalculator::get_sampledPoint_t()
{
  return mP_t;
}

ssc::Vector3D ToolTipCalibrationCalculator::get_sampledPoint_ref()
{
  ssc::CoordinateSystem csT = ssc::CoordinateSystemHelpers::getT(mTool); //from
  ssc::CoordinateSystem csRef = ssc::CoordinateSystemHelpers::getT(mRef); //to

  ssc::Transform3D refMt = ssc::CoordinateSystemHelpers::get_toMfrom(csT, csRef);

  ssc::Vector3D P_ref = refMt.coord(mP_t);

  return P_ref;
}

ssc::Vector3D ToolTipCalibrationCalculator::get_referencePoint_ref()
{
  return mRef->getReferencePoints()[1];
}

ssc::Transform3D ToolTipCalibrationCalculator::get_sMt_new()
{
  ssc::Transform3D sMt_old = mTool->getCalibration_sMt();

  ssc::CoordinateSystem csT = ssc::CoordinateSystemHelpers::getT(mTool); //to
  ssc::CoordinateSystem csRef = ssc::CoordinateSystemHelpers::getT(mRef); //from
  ssc::Transform3D tMref = ssc::CoordinateSystemHelpers::get_toMfrom(csRef, csT);

  ssc::Vector3D delta_t = tMref.vector(this->get_delta_ref());
  ssc::Transform3D T_delta_t = ssc::createTransformTranslate(delta_t);

  return sMt_old * T_delta_t;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
}//namespace cx
