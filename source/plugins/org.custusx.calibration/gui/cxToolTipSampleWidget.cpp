/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include <cxToolTipSampleWidget.h>

#include <QPushButton>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include "cxTypeConversions.h"
#include "cxLogger.h"
#include "cxVector3D.h"
#include "cxDefinitionStrings.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxProfile.h"
#include "cxSelectDataStringProperty.h"
#include "cxSpaceProvider.h"
#include "cxPatientModelService.h"
#include "cxReporter.h"
#include "cxVisServices.h"
#include "cxStringPropertySelectTool.h"
#include "cxStringPropertySelectCoordinateSystem.h"

namespace cx
{

ToolTipSampleWidget::ToolTipSampleWidget(VisServicesPtr services, QWidget* parent) :
	BaseWidget(parent, "tool_tip_sample_widget", "ToolTip Sample"),
	mServices(services),
    mSampleButton(new QPushButton("Sample")),
    mSaveToFileNameLabel(new QLabel("<font color=red> No file selected </font>")),
    mSaveFileButton(new QPushButton("Save to...")),
	mTruncateFile(false)
{
  QVBoxLayout* toplayout = new QVBoxLayout(this);

  this->setToolTip("Sample the tool tip position");
  mCoordinateSystems = StringPropertySelectCoordinateSystem::New(services->tracking());
  mTools = StringPropertySelectTool::New(mServices->tracking());
	mData = StringPropertySelectData::New(mServices->patient());

  mCoordinateSystemComboBox = new LabeledComboBoxWidget(this, mCoordinateSystems);
  mToolComboBox = new LabeledComboBoxWidget(this, mTools);
  mDataComboBox = new LabeledComboBoxWidget(this, mData);

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

void ToolTipSampleWidget::saveFileSlot()
{
  QString configPath = profile()->getPath();
  if(mServices->patient()->isPatientValid())
	configPath = mServices->patient()->getActivePatientFolder();

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

  CoordinateSystem to = this->getSelectedCoordinateSystem();
  Vector3D toolPoint = mServices->spaceProvider()->getActiveToolTipPoint(to, false);

  if(!samplingFile.open(QIODevice::WriteOnly | (mTruncateFile ? QIODevice::Truncate : QIODevice::Append)))
  {
    reportWarning("Could not open "+samplingFile.fileName());
    report("Sampled point: "+qstring_cast(toolPoint));
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

  reporter()->playSampleSound();
  report("Sampled point in "+qstring_cast(to.mId)+" ("+to.mRefObject+") space, result: "+sampledPoint);
}

void ToolTipSampleWidget::coordinateSystemChanged()
{
  switch (string2enum<COORDINATE_SYSTEM>(mCoordinateSystems->getValue()))
  {
  case csDATA:
    mDataComboBox->show();
    mToolComboBox->hide();
    break;
  case csTOOL:
    mToolComboBox->show();
    mDataComboBox->hide();
    break;
  case csSENSOR:
    mToolComboBox->show();
    mDataComboBox->hide();
    break;
  default:
    mDataComboBox->hide();
    mToolComboBox->hide();
    break;
  };
}

CoordinateSystem ToolTipSampleWidget::getSelectedCoordinateSystem()
{
  CoordinateSystem retval(csCOUNT);

  retval.mId = string2enum<COORDINATE_SYSTEM>(mCoordinateSystems->getValue());

  switch (retval.mId)
  {
  case csDATA:
	retval = mServices->spaceProvider()->getD(mData->getData());
    break;
  case csTOOL:
	retval = mServices->spaceProvider()->getT(mTools->getTool());
    break;
  case csSENSOR:
	retval = mServices->spaceProvider()->getT(mTools->getTool());
    break;
  default:
    retval.mRefObject = "";
    break;
  };

  return retval;
}
//------------------------------------------------------------------------------


}
