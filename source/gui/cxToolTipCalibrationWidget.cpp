#include "cxToolTipCalibrationWidget.h"

#include <QPushButton>
#include <QTextStream>
#include <QFileDialog>
#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "sscToolManager.h"
#include "sscDataManager.h"
#include "sscVector3D.h"
#include "sscDefinitionStrings.h"
#include "sscLabeledComboBoxWidget.h"
#include "cxDataLocations.h"

namespace cx
{
ToolTipCalibrationWidget::ToolTipCalibrationWidget(QWidget* parent) :
  WhatsThisWidget(parent),
  mCalibrateButton(new QPushButton("Calibrate", this)),
  mTestButton(new QPushButton("Test calibration", this)),
  mSampleButton(new QPushButton("Sample")),
  mSaveFileButton(new QPushButton("Save to..."))
{
  this->setObjectName("ToolTipCalibrationWidget");
  this->setWindowTitle("Tool Tip");

  mSaveToFileNameLabel = new QLabel(DataLocations::getRootConfigPath()+"/SampledPoints.txt", this);

  connect(mSaveFileButton, SIGNAL(clicked()), this, SLOT(saveFileSlot()));

  mCoordinateSystems = SelectCoordinateSystemStringDataAdapter::New();
  connect(mCoordinateSystems.get(), SIGNAL(changed()), this, SLOT(coordChangedSlot()));

  mTools = SelectToolStringDataAdapter::New();
  connect(mTools.get(), SIGNAL(changed()), this, SLOT(referenceObjectChanged()));

  mData = SelectDataStringDataAdapter::New();
  connect(mData.get(), SIGNAL(changed()), this, SLOT(referenceObjectChanged()));

  mToCoordinateSystem.mId = ssc::csTOOL;
  mToCoordinateSystem.mRefObject = ssc::toolManager()->getDominantTool()->getUid();

  QVBoxLayout* toptopLayout = new QVBoxLayout(this);
  QGridLayout* topLayout = new QGridLayout();
  toptopLayout->addLayout(topLayout);
  toptopLayout->addStretch();

  connect(mCalibrateButton, SIGNAL(clicked()), this, SLOT(calibrateSlot()));
  connect(mTestButton, SIGNAL(clicked()), this, SLOT(testSlot()));
  connect(mSampleButton, SIGNAL(clicked()), this, SLOT(sampleSlot()));

  topLayout->addWidget(this->createCalibrateGroupBox(), 1,0);
  topLayout->addWidget(this->createTestGroupBox(), 2,0);
  topLayout->addWidget(this->createSampleGroupBox(), 3,0);

  this->coordChangedSlot();
}

ToolTipCalibrationWidget::~ToolTipCalibrationWidget()
{
}

QString ToolTipCalibrationWidget::defaultWhatsThis() const
{
  return "<html>"
    "<h3>Tool tip calibration.</h3>"
    "<p><i>Calibrates a tool by sampling it when pointing at a known point on another frame.</i></p>"
    "</html>";
}

void ToolTipCalibrationWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
}

void ToolTipCalibrationWidget::hideEvent(QCloseEvent* event)
{
  QWidget::closeEvent(event);
}

void ToolTipCalibrationWidget::calibrateSlot()
{
  ssc::messageManager()->sendDebug("Calibration slot not implemented yet.");
}

void ToolTipCalibrationWidget::testSlot()
{
  ssc::messageManager()->sendDebug("Test slot not implemented yet.");
}

void ToolTipCalibrationWidget::sampleSlot()
{
  QFile samplingFile(mSaveToFileNameLabel->text());

  if(!samplingFile.open(QIODevice::WriteOnly | QIODevice::Append))
  {
    ssc::messageManager()->sendError("Could not open "+samplingFile.fileName());
    return;
  }

  ssc::Vector3D toolPoint = ssc::CoordinateSystemHelpers::getDominantToolTipPoint(mToCoordinateSystem, false);

  QString sampledPoint = qstring_cast(toolPoint);

  QTextStream streamer(&samplingFile);
//  streamer << QDateTime::currentDateTime().toString();
  streamer << sampledPoint;
  streamer << endl;

  ssc::messageManager()->sendInfo("Sampled point in "+qstring_cast(mToCoordinateSystem.mId)+" ("+mToCoordinateSystem.mRefObject+") space, result: "+sampledPoint);
}

void ToolTipCalibrationWidget::coordChangedSlot()
{
  mToCoordinateSystem.mId = string2enum<ssc::COORDINATE_SYSTEM>(mCoordinateSystems->getValue());

  switch (mToCoordinateSystem.mId)
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

//  ssc::messageManager()->sendDebug(qstring_cast(mToCoordinateSystem.mId)+" space selected, with "+mToCoordinateSystem.mRefObject+" as reference object. (1)");
}

void ToolTipCalibrationWidget::referenceObjectChanged()
{
  QString uid = "";
  if(static_cast<SelectToolStringDataAdapter*>(this->sender()) && mTools->getTool())
    uid = mTools->getTool()->getUid();

  if(static_cast<SelectDataStringDataAdapter*>(this->sender()) && mData->getData())
    uid = mData->getData()->getUid();

  mToCoordinateSystem.mRefObject = uid;
//  ssc::messageManager()->sendDebug(qstring_cast(mToCoordinateSystem.mId)+" space selected, with "+mToCoordinateSystem.mRefObject+" as reference object.(2)");
}

void ToolTipCalibrationWidget::saveFileSlot()
{
  QString configPath = DataLocations::getRootConfigPath();

  QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                             configPath+"/SampledPoints.txt",
                             tr("Text (*.txt)"));
  if(fileName.isEmpty())
    return;

  mSaveToFileNameLabel->setText(fileName);
}

QGroupBox* ToolTipCalibrationWidget::createCalibrateGroupBox()
{
  QGroupBox* retval = new QGroupBox("Calibrate tool", this);
  QVBoxLayout* toplayout = new QVBoxLayout(retval);

  toplayout->addWidget(mCalibrateButton);
  toplayout->addStretch();

  return retval;
}

QGroupBox* ToolTipCalibrationWidget::createTestGroupBox()
{
  QGroupBox* retval = new QGroupBox("Test calibration", this);
  QVBoxLayout* toplayout = new QVBoxLayout(retval);

  toplayout->addWidget(mTestButton);
  toplayout->addStretch();

  return retval;
}

QGroupBox* ToolTipCalibrationWidget::createSampleGroupBox()
{
  QGroupBox* retval = new QGroupBox("Sample points", this);
  QVBoxLayout* toplayout = new QVBoxLayout(retval);

  mCoordinateSystemComboBox = new ssc::LabeledComboBoxWidget(this, mCoordinateSystems);
  mToolComboBox = new ssc::LabeledComboBoxWidget(this, mTools);
  mDataComboBox = new ssc::LabeledComboBoxWidget(this, mData);


  toplayout->addWidget(mSaveFileButton);
  toplayout->addWidget(mSaveToFileNameLabel);
//  toplayout->addSpacing(15);
  toplayout->addWidget(this->createHorizontalLine());
  toplayout->addWidget(new QLabel("<b>Select coordinate system to sample in: </b>", retval));
  toplayout->addWidget(mCoordinateSystemComboBox);
  toplayout->addWidget(mToolComboBox);
  toplayout->addWidget(mDataComboBox);
//  toplayout->addSpacing(15);
  toplayout->addWidget(this->createHorizontalLine());
  toplayout->addWidget(mSampleButton);
  toplayout->addStretch();

  return retval;
}

}//namespace cx
