/*
 * cxTemporalCalibrationWidget.cpp
 *
 *  \date May 4, 2011
 *      \author christiana
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
#include "cxRecordSessionWidget.h"
#include "cxSettings.h"
#include "cxToolDataAdapters.h"
#include "cxDoubleDataAdapterTemporalCalibration.h"
#include "sscUtilHelpers.h"
#include "sscVolumeHelpers.h"
#include "vtkImageCorrelation.h"
#include "sscMessageManager.h"
#include "cxPatientService.h"

typedef vtkSmartPointer<vtkImageCorrelation> vtkImageCorrelationPtr;
typedef vtkSmartPointer<vtkDoubleArray> vtkDoubleArrayPtr;


namespace cx
{

typedef unsigned char uchar;


TemporalCalibrationWidget::TemporalCalibrationWidget(AcquisitionDataPtr acquisitionData, QWidget* parent) :
    BaseWidget(parent, "TemporalCalibrationWidget", "Temporal Calibration"),
    mRecordSessionWidget(new RecordSessionWidget(acquisitionData, this, "temporal_calib")),
		mInfoLabel(new QLabel(""))
{
  mAlgorithm.reset(new TemporalCalibration);
  connect(patientService()->getPatientData().get(), SIGNAL(patientChanged()), this, SLOT(patientChangedSlot()));

  mAcquisition.reset(new USAcquisition(acquisitionData));
  connect(mAcquisition.get(), SIGNAL(ready(bool,QString)), mRecordSessionWidget, SLOT(setReady(bool,QString)));
  connect(mAcquisition.get(), SIGNAL(saveDataCompleted(QString)), this, SLOT(selectData(QString)));
  mAcquisition->checkIfReadySlot();

  connect(mRecordSessionWidget, SIGNAL(newSession(QString)), mAcquisition.get(), SLOT(saveSession(QString)));
  connect(mRecordSessionWidget, SIGNAL(started(QString)), mAcquisition.get(), SLOT(startRecord(QString)));
  connect(mRecordSessionWidget, SIGNAL(stopped()), mAcquisition.get(), SLOT(stopRecord()));
  mRecordSessionWidget->setDescriptionVisibility(false);

  QVBoxLayout* topLayout = new QVBoxLayout(this);

  // add recording widgets
  QLabel* acqLabel = new QLabel("<b>Acquisition</b>");
  topLayout->addWidget(acqLabel);
  acqLabel->setToolTip(this->defaultWhatsThis());
  topLayout->addWidget(mInfoLabel);
  topLayout->addWidget(mRecordSessionWidget);
  topLayout->addWidget(new ssc::LabeledComboBoxWidget(this, ActiveProbeConfigurationStringDataAdapter::New()));
  topLayout->addWidget(new ssc::SpinBoxGroupWidget(this, DoubleDataAdapterTimeCalibration::New()));

  topLayout->addWidget(this->createHorizontalLine());

  // add calibration widgets
  QLabel* calLabel = new QLabel("<b>Calibration</b>");
  topLayout->addWidget(calLabel);
  calLabel->setToolTip(this->defaultWhatsThis());

  mFileSelectWidget = new ssc::FileSelectWidget(this);
  connect(mFileSelectWidget, SIGNAL(fileSelected(QString)), this, SLOT(selectData(QString)));
  topLayout->addWidget(mFileSelectWidget);

  mVerbose = new QCheckBox("Save data to temporal_calib.txt");
  topLayout->addWidget(mVerbose);

  QPushButton* calibrateButton = new QPushButton("Calibrate");
  calibrateButton->setToolTip(this->defaultWhatsThis());

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
	  "<p>Part 1, Acqusition: Move the probe in a sinusoidal pattern up and down in a water tank or similar."
	  "The <i>first</i> image should be a typical image, as it is used to correlate against all the others."
	  "<p>Part 2, Calibration: Press calibrate to calculate the temporal shift for the selected acquisition."
	  "The shift is not applied in any way. Refer to the log folder for the calibration curves."
      "</html>";
}

void TemporalCalibrationWidget::showEvent(QShowEvent* event)
{
  mFileSelectWidget->refresh();
}

void TemporalCalibrationWidget::patientChangedSlot()
{
//  std::cout << "TemporalCalibrationWidget::patientChangedSlot() "  << std::endl;
  QString filename = patientService()->getPatientData()->getActivePatientFolder() + "/US_Acq/";
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
    mAlgorithm->setDebugFolder(patientService()->getPatientData()->getActivePatientFolder()+"/Logs/");
  else
    mAlgorithm->setDebugFolder("");

  bool success = true;
  double shift = mAlgorithm->calibrate(&success);
  if (success)
  {
	  ssc::messageManager()->sendSuccess(QString("Completed temporal calibration, found shift %1 ms").arg(shift,0,'f',0));
	  mResult->setText(QString("Shift = %1 ms").arg(shift, 0, 'f', 0));
  }
  else
  {
	  ssc::messageManager()->sendError(QString("Temporal calibration failed"));
	  mResult->setText(QString("failed"));
  }
}


}//namespace cx


