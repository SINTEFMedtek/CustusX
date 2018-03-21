/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include <cxTemporalCalibrationWidget.h>


#include <QtWidgets>

#include <QVBoxLayout>
#include "boost/bind.hpp"
#include "cxTrackingService.h"
#include "cxLabeledComboBoxWidget.h"
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include "cxDoubleWidgets.h"
#include "cxTypeConversions.h"
#include "cxRecordSessionWidget.h"
#include "cxSettings.h"
#include "cxToolProperty.h"
#include "cxDoublePropertyTemporalCalibration.h"
#include "cxUtilHelpers.h"
#include "cxVolumeHelpers.h"
#include "vtkImageCorrelation.h"
#include "cxLogger.h"
#include "cxPatientModelService.h"
#include "cxVisServices.h"

typedef vtkSmartPointer<vtkImageCorrelation> vtkImageCorrelationPtr;
typedef vtkSmartPointer<vtkDoubleArray> vtkDoubleArrayPtr;


namespace cx
{

typedef unsigned char uchar;


TemporalCalibrationWidget::TemporalCalibrationWidget(VisServicesPtr services, AcquisitionServicePtr acquisitionService, QWidget* parent) :
	BaseWidget(parent, "temporal_calibration_widget", "Temporal Calibration"),
	mInfoLabel(new QLabel(""))
{
	mServices = services;

	this->setToolTip("Temporal calibration from a vertical periodic movement of an US probe");
  mAlgorithm.reset(new TemporalCalibration);
  connect(mServices->patient().get(), SIGNAL(patientChanged()), this, SLOT(patientChangedSlot()));

  connect(acquisitionService.get(), SIGNAL(saveDataCompleted(QString)), this, SLOT(selectData(QString)));

  cx::AcquisitionService::TYPES context(cx::AcquisitionService::tTRACKING | cx::AcquisitionService::tUS);
  mRecordSessionWidget = new RecordSessionWidget(acquisitionService, this, context, "temp_cal");

  QVBoxLayout* topLayout = new QVBoxLayout(this);

  // add recording widgets
  QLabel* acqLabel = new QLabel("<b>Acquisition</b>");
  topLayout->addWidget(acqLabel);
  acqLabel->setToolTip(this->toolTip());
  topLayout->addWidget(mInfoLabel);
  topLayout->addWidget(mRecordSessionWidget);
  topLayout->addWidget(new LabeledComboBoxWidget(this, StringPropertyActiveProbeConfiguration::New(mServices->tracking())));
  topLayout->addWidget(new SpinBoxGroupWidget(this, DoublePropertyTimeCalibration::New(mServices->tracking())));

  topLayout->addWidget(this->createHorizontalLine());

  // add calibration widgets
  QLabel* calLabel = new QLabel("<b>Calibration</b>");
  topLayout->addWidget(calLabel);
  calLabel->setToolTip(this->toolTip());

  mFileSelectWidget = new FileSelectWidget(this);
  connect(mFileSelectWidget, SIGNAL(fileSelected(QString)), this, SLOT(selectData(QString)));
  mFileSelectWidget->setNameFilter(QStringList() << "*.fts");
  topLayout->addWidget(mFileSelectWidget);

  mVerbose = new QCheckBox("Save data to temporal_calib.txt");
  topLayout->addWidget(mVerbose);

	QPushButton* calibrateButton = new QPushButton("Calibrate");
	calibrateButton->setToolTip("Calculate the temporal shift for the selected acqusition."
															"The shift is not applied in any way."
															"<p><b>NB:</b>"
															" The calculation takes a few seconds, and in this time the program will be unresponsive</p>");

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

void TemporalCalibrationWidget::showEvent(QShowEvent* event)
{
  mFileSelectWidget->refresh();
}

void TemporalCalibrationWidget::patientChangedSlot()
{
  QString filename = mServices->patient()->getActivePatientFolder() + "/US_Acq/";
  mFileSelectWidget->setPath(filename);
}

void TemporalCalibrationWidget::selectData(QString filename)
{
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
	mAlgorithm->setDebugFolder(mServices->patient()->getActivePatientFolder()+"/Logs/");
  else
    mAlgorithm->setDebugFolder("");

  bool success = true;
  double shift = mAlgorithm->calibrate(&success);
  if (success)
  {
	  reportSuccess(QString("Completed temporal calibration, found shift %1 ms").arg(shift,0,'f',0));
	  mResult->setText(QString("Shift = %1 ms").arg(shift, 0, 'f', 0));
  }
  else
  {
	  reportError(QString("Temporal calibration failed"));
	  mResult->setText(QString("failed"));
  }
}


}//namespace cx


