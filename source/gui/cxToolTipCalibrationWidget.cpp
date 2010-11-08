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
  if(!refTool || (ssc::similar(refTool->getReferencePoint(), ssc::Vector3D(0.000,0.000,0.000))))
    return;

  ssc::CoordinateSystem to = ssc::CoordinateSystemHelpers::getCoordinateSystem(refTool);
  ssc::Vector3D sampledPoint = ssc::CoordinateSystemHelpers::getDominantToolTipPoint(to);

  ssc::ToolPtr tool = ssc::toolManager()->getDominantTool();

  ToolTipCalibrationCalculator calc(tool, refTool, sampledPoint);
  ssc::Transform3D calibration = calc.get_calibration_sMt();

  QMessageBox msgBox;
  msgBox.setText("Do you want to overwrite "+tool->getName()+"s calibration file?");
  msgBox.setInformativeText("This cannot be undone.");
  msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::Ok);
  int ret = msgBox.exec();

  if(ret == QMessageBox::Ok)
    tool->setCalibration_sMt(calibration);

  ssc::messageManager()->sendInfo("Calibration:\n"+qstring_cast(calibration));
}

void ToolTipCalibrateWidget::testCalibrationSlot()
{
  ssc::ToolPtr selectedTool = mTools->getTool();
  if(!selectedTool || (ssc::similar(selectedTool->getReferencePoint(), ssc::Vector3D(0.000,0.000,0.000))))
    return;

  ssc::CoordinateSystem to = ssc::CoordinateSystemHelpers::getCoordinateSystem(selectedTool);
  ssc::Vector3D sampledPoint = ssc::CoordinateSystemHelpers::getDominantToolTipPoint(to);

  ToolTipCalibrationCalculator calc(ssc::toolManager()->getDominantTool(), selectedTool, sampledPoint);
  ssc::Vector3D delta_selectedTool = calc.get_delta_ref();

  ssc::messageManager()->sendInfo("Delta:\n"+qstring_cast(delta_selectedTool));

  mDeltaLabel->setText("<b>Delta:</b> "+qstring_cast(delta_selectedTool)+" <br> <b>Length:</b>  "+qstring_cast(delta_selectedTool.length()));
}

void ToolTipCalibrateWidget::toolSelectedSlot()
{
  QString text("Ref. point: <UNDEFINED POINT>");
  mCalibrateButton->setEnabled(false);
  mTestButton->setEnabled(false);

  if(mTools->getTool())
  {
    ToolPtr tool = boost::dynamic_pointer_cast<Tool>(mTools->getTool());
    if(tool && !(ssc::similar(tool->getReferencePoint(), ssc::Vector3D(0.000,0.000,0.000))))
    {
      text = "Ref. point: "+qstring_cast(tool->getReferencePoint());
      mCalibrateButton->setEnabled(true);
      mTestButton->setEnabled(true);
    }
  }

  mReferencePointLabel->setText(text);
}
  //------------------------------------------------------------------------------
ToolTipSampleWidget::ToolTipSampleWidget(QWidget* parent) :
    WhatsThisWidget(parent),
    mSampleButton(new QPushButton("Sample")),
    mSaveToFileNameLabel(new QLabel("<font color=red> No file selected </font>")),
    mSaveFileButton(new QPushButton("Save to..."))
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

  QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                             configPath+"/SampledPoints.txt",
                             tr("Text (*.txt)"));
  if(fileName.isEmpty())
    return;

  mSaveToFileNameLabel->setText(fileName);
}

void ToolTipSampleWidget::sampleSlot()
{
  QFile samplingFile(mSaveToFileNameLabel->text());

  if(!samplingFile.open(QIODevice::WriteOnly | QIODevice::Append))
  {
    ssc::messageManager()->sendError("Could not open "+samplingFile.fileName());
    return;
  }

  ssc::CoordinateSystem to = this->getSelectedCoordinateSystem();
  ssc::Vector3D toolPoint = ssc::CoordinateSystemHelpers::getDominantToolTipPoint(to, false);

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
    retval = ssc::CoordinateSystemHelpers::getCoordinateSystem(mData->getData());
    break;
  case ssc::csTOOL:
    retval = ssc::CoordinateSystemHelpers::getCoordinateSystem(mTools->getTool());
    break;
  case ssc::csSENSOR:
    retval = ssc::CoordinateSystemHelpers::getCoordinateSystem(mTools->getTool());
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
  return get_sampledPoint_ref() - get_referencePoint_ref(); //or the other way around?
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
  ssc::CoordinateSystem csT = ssc::CoordinateSystemHelpers::getCoordinateSystem(mTool);
  ssc::CoordinateSystem csRef = ssc::CoordinateSystemHelpers::getCoordinateSystem(mRef);

  ssc::Transform3D refMt = ssc::CoordinateSystemHelpers::get_toMfrom(csT, csRef);

  ssc::Vector3D P_ref = refMt.coord(mP_t);

  return P_ref;
}

ssc::Vector3D ToolTipCalibrationCalculator::get_referencePoint_ref()
{
  return mRef->getReferencePoint();
}

ssc::Transform3D ToolTipCalibrationCalculator::get_sMt_new()
{
  ssc::Transform3D delta_ref = ssc::createTransformTranslate(get_delta_ref());
  ssc::Transform3D sMt_old = mTool->getCalibration_sMt();

  return sMt_old * delta_ref;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
}//namespace cx
