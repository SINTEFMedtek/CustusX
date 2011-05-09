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
    BaseWidget(parent, "TemporalCalibrationWidget", "Temporal Calibration")
{
  mAlgorithm.reset(new TemporalCalibration);
  connect(stateManager()->getPatientData().get(), SIGNAL(patientChanged()), this, SLOT(patientChangedSlot()));

  QVBoxLayout* topLayout = new QVBoxLayout(this);

  topLayout->addWidget(new ssc::LabeledComboBoxWidget(this, ActiveToolConfigurationStringDataAdapter::New()));

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

void TemporalCalibrationWidget::patientChangedSlot()
{
  mFileSelectWidget->setFilename(stateManager()->getPatientData()->getActivePatientFolder() + "/US_Acq/");
}

void TemporalCalibrationWidget::selectData(QString filename)
{
  mAlgorithm->selectData(filename);
  mResult->setText("");
}

/**
 *
 */
void TemporalCalibrationWidget::calibrateSlot()
{
  if (mVerbose->isChecked())
    mAlgorithm->setDebugFile(stateManager()->getPatientData()->getActivePatientFolder()+"/Logs/temporal_calib.txt");
  else
    mAlgorithm->setDebugFile("");

  double shift = mAlgorithm->calibrate();
  mResult->setText(QString("Shift = %1 ms").arg(shift, 0, 'f', 0));
}




}//namespace cx


