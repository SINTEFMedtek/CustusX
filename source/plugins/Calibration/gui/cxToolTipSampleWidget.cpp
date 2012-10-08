/*
 * cxToolTipSampleWidget.cpp
 *
 *  \date May 4, 2011
 *      \author christiana
 */

#include <cxToolTipSampleWidget.h>

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
//#include "cxStateService.h"
#include "cxPatientData.h"
#include "cxPatientService.h"

namespace cx
{

ToolTipSampleWidget::ToolTipSampleWidget(QWidget* parent) :
    BaseWidget(parent, "ToolTipSampleWidget", "ToolTip Sample"),
    mSampleButton(new QPushButton("Sample")),
    mSaveToFileNameLabel(new QLabel("<font color=red> No file selected </font>")),
    mSaveFileButton(new QPushButton("Save to...")),
    mTruncateFile(false)
{
  QVBoxLayout* toplayout = new QVBoxLayout(this);

  mCoordinateSystems = SelectCoordinateSystemStringDataAdapter::New();
  mTools = SelectToolStringDataAdapter::New();
  mData = SelectDataStringDataAdapter::New();

  mCoordinateSystemComboBox = new ssc::LabeledComboBoxWidget(this, mCoordinateSystems);
  mToolComboBox = new ssc::LabeledComboBoxWidget(this, mTools);
  mDataComboBox = new ssc::LabeledComboBoxWidget(this, mData);

  toplayout->addWidget(new QLabel("<b>Select coordinate system to sample in: </b>"));
  toplayout->addWidget(mCoordinateSystemComboBox);
  toplayout->addWidget(mToolComboBox);
  toplayout->addWidget(mDataComboBox);
  toplayout->addWidget(this->createHorizontalLine());
  toplayout->addWidget(mSampleButton);
  toplayout->addWidget(this->createHorizontalLine());
  toplayout->addWidget(mSaveFileButton);
  toplayout->addWidget(mSaveToFileNameLabel);
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

void ToolTipSampleWidget::saveFileSlot()
{
  QString configPath = DataLocations::getRootConfigPath();
  if(patientService()->getPatientData()->isPatientValid())
    configPath = patientService()->getPatientData()->getActivePatientFolder();

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

  ssc::messageManager()->playSampleSound();
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
  ssc::CoordinateSystem retval(ssc::csCOUNT);

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


}
