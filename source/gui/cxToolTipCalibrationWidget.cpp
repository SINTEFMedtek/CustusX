#include "cxToolTipCalibrationWidget.h"

#include <QPushButton>
#include <QTextStream>
#include <QLineEdit>
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
  mFilenameBox(new QLineEdit("", this))
{
  this->setObjectName("ToolTipCalibrationWidget");
  this->setWindowTitle("Tool Tip");

  mCoordinateSystems = SelectCoordinateSystemStringDataAdapter::New();
  connect(mCoordinateSystems.get(), SIGNAL(changed()), this, SLOT(coordChangedSlot()));

  mToCoord.mId = ssc::csTOOL;
  mToCoord.mRefObject = ssc::toolManager()->getDominantTool()->getUid();

  QVBoxLayout* toptopLayout = new QVBoxLayout(this);
  QGridLayout* topLayout = new QGridLayout();
  toptopLayout->addLayout(topLayout);
  toptopLayout->addStretch();

  connect(mCalibrateButton, SIGNAL(clicked()), this, SLOT(calibrateSlot()));
  connect(mTestButton, SIGNAL(clicked()), this, SLOT(testSlot()));
  connect(mSampleButton, SIGNAL(clicked()), this, SLOT(sampleSlot()));

  topLayout->addWidget(mCalibrateButton, 1,0);
  topLayout->addWidget(mTestButton, 2,0);
  topLayout->addWidget(this->createSampleGroupBox(), 3,0);
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
  QString configPath = DataLocations::getRootConfigPath();
  QString filePath(configPath+"/"+mFilenameBox->text());
  QFile samplingFile(filePath);

  if(!samplingFile.open(QIODevice::WriteOnly | QIODevice::Append))
  {
    ssc::messageManager()->sendError("Could not open "+samplingFile.fileName());
    return;
  }

  ssc::Vector3D toolPoint = ssc::CoordinateSystemHelpers::getDominantToolTipPoint(mToCoord, false);

  QString sampledPoint = qstring_cast(toolPoint);

  QTextStream streamer(&samplingFile);
  streamer << sampledPoint;
  streamer << endl;

  ssc::messageManager()->sendInfo("Sampled point in "+qstring_cast(mToCoord.mId)+" ("+mToCoord.mRefObject+") space, result: "+sampledPoint);
}

void ToolTipCalibrationWidget::coordChangedSlot()
{
  mToCoord.mId = string2enum<ssc::COORDINATE_SYSTEM>(mCoordinateSystems->getValue());

  switch (mToCoord.mId)
  {
  case ssc::csDATA:
//    mSelectData->show();
    //mToCoord.mRefObject = ssc::dataManager()->getActiveImage()->getUid();
    break;
  case ssc::csTOOL:
    //mToCoord.mRefObject = ssc::toolManager()->getDominantTool()->getUid();
//    mSelectTool->show();
    break;
  default:
//    mSelectData->hide();
//    mSelectTool->hide();
    break;
  };

  ssc::messageManager()->sendDebug(qstring_cast(mToCoord.mId)+" space selected, with "+mToCoord.mRefObject+" as reference object.");
}

QGroupBox* ToolTipCalibrationWidget::createSampleGroupBox()
{
  QGroupBox* retval = new QGroupBox("Sample points", this);
  QVBoxLayout* toplayout = new QVBoxLayout(retval);

  QString defaultFileName("SampledPoints.txt");
  mFilenameBox->setText(defaultFileName);

  toplayout->addWidget(mSampleButton);
  toplayout->addWidget(mFilenameBox);
  toplayout->addWidget(new ssc::LabeledComboBoxWidget(this, mCoordinateSystems));
  toplayout->addStretch();

  return retval;
}

}//namespace cx
