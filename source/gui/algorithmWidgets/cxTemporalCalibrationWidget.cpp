/*
 * cxTemporalCalibrationWidget.cpp
 *
 *  Created on: May 4, 2011
 *      Author: christiana
 */

#include <cxTemporalCalibrationWidget.h>


#include <QtGui>
#include <QVBoxLayout>
#include "boost/bind.hpp"
#include "sscToolManager.h"
#include "sscLabeledComboBoxWidget.h"
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include "sscDoubleWidgets.h"
#include "sscTypeConversions.h"
#include "cxPatientData.h"
#include "cxStateMachineManager.h"
#include "cxRecordSessionWidget.h"
#include "cxSettings.h"
#include "cxToolDataAdapters.h"
#include "cxDoubleDataAdapterTemporalCalibration.h"
#include "sscUtilHelpers.h"
#include "sscVolumeHelpers.h"
#include "vtkImageCorrelation.h"
typedef vtkSmartPointer<vtkImageCorrelation> vtkImageCorrelationPtr;
typedef vtkSmartPointer<vtkDoubleArray> vtkDoubleArrayPtr;


namespace cx
{

typedef unsigned char uchar;


TemporalCalibrationWidget::TemporalCalibrationWidget(QWidget* parent) :
    BaseWidget(parent, "TemporalCalibrationWidget", "Temporal Calibration"),
    mRecordSessionWidget(new RecordSessionWidget(this, "temporal_calib")),
		mInfoLabel(new QLabel(""))
{
  mAlgorithm.reset(new TemporalCalibration);
  connect(stateManager()->getPatientData().get(), SIGNAL(patientChanged()), this, SLOT(patientChangedSlot()));

  mAcquisition.reset(new USAcquisition());
  connect(mAcquisition.get(), SIGNAL(ready(bool,QString)), mRecordSessionWidget, SLOT(setReady(bool,QString)));
  connect(mAcquisition.get(), SIGNAL(saveDataCompleted(QString)), this, SLOT(selectData(QString)));
  mAcquisition->checkIfReadySlot();

  connect(mRecordSessionWidget, SIGNAL(newSession(QString)), mAcquisition.get(), SLOT(saveSession(QString)));
  connect(mRecordSessionWidget, SIGNAL(started()), mAcquisition.get(), SLOT(startRecord()));
  connect(mRecordSessionWidget, SIGNAL(stopped()), mAcquisition.get(), SLOT(stopRecord()));
  mRecordSessionWidget->setDescriptionVisibility(false);

  QVBoxLayout* topLayout = new QVBoxLayout(this);

  // add recording widgets
  topLayout->addWidget(mInfoLabel);
  topLayout->addWidget(mRecordSessionWidget);
  topLayout->addWidget(new ssc::LabeledComboBoxWidget(this, ActiveToolConfigurationStringDataAdapter::New()));
  topLayout->addWidget(new ssc::SpinBoxGroupWidget(this, DoubleDataAdapterTimeCalibration::New()));

  topLayout->addWidget(this->createHorizontalLine());

  // add calibration widgets
  mFileSelectWidget = new ssc::FileSelectWidget(this);
  connect(mFileSelectWidget, SIGNAL(fileSelected(QString)), this, SLOT(selectData(QString)));
  topLayout->addWidget(mFileSelectWidget);

  mVerbose = new QCheckBox("Save data to temporal_calib.txt");
  topLayout->addWidget(mVerbose);

  QPushButton* calibrateButton = new QPushButton("Calibrate");
  connect(calibrateButton, SIGNAL(clicked()), this, SLOT(calibrateSlot()));
  topLayout->addWidget(calibrateButton);

  mResult = new QLineEdit;
  mResult->setReadOnly(true);
  topLayout->addWidget(mResult);

  topLayout->addStretch();

  this->patientChangedSlot();
}

TemporalCalibrationWidget::~TemporalCalibrationWidget()
{}

QString TemporalCalibrationWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Temporal Calibration.</h3>"
      "<p><i>Calibrate the time shift between the tracking system and the video acquisition source.</i></br>"
      "</html>";
}

void TemporalCalibrationWidget::showEvent(QShowEvent* event)
{
  mFileSelectWidget->refresh();
}

void TemporalCalibrationWidget::patientChangedSlot()
{
//  std::cout << "TemporalCalibrationWidget::patientChangedSlot() "  << std::endl;
  QString filename = stateManager()->getPatientData()->getActivePatientFolder() + "/US_Acq/";
  mFileSelectWidget->setPath(filename);
//  this->selectData(filename);
}

void TemporalCalibrationWidget::selectData(QString filename)
{
//  std::cout << "TemporalCalibrationWidget::selectData " << filename << std::endl;
  mAlgorithm->selectData(filename);
  mFileSelectWidget->setFilename(filename);
  mResult->setText("");
}

/**
 *
 */
void TemporalCalibrationWidget::calibrateSlot()
{
  if (mVerbose->isChecked())
    mAlgorithm->setDebugFolder(stateManager()->getPatientData()->getActivePatientFolder()+"/Logs/");
  else
    mAlgorithm->setDebugFolder("");

  double shift = mAlgorithm->calibrate();
  ssc::messageManager()->sendSuccess(QString("Completed temporal calibration, found shift %1 ms").arg(shift,0,'f',0));
  mResult->setText(QString("Shift = %1 ms").arg(shift, 0, 'f', 0));
}


}//namespace cx


