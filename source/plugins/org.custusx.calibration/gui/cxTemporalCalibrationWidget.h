/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXTEMPORALCALIBRATIONWIDGET_H_
#define CXTEMPORALCALIBRATIONWIDGET_H_

#include "org_custusx_calibration_Export.h"

#include "cxBaseWidget.h"
#include "cxRecordSessionWidget.h"
#include <QFuture>
#include <QFutureWatcher>
#include "cxFileSelectWidget.h"
#include <cxTemporalCalibration.h>
#include "cxUSAcqusitionWidget.h"

namespace cx
{
typedef boost::shared_ptr<class VisServices> VisServicesPtr;

/**
 * \file
 * \addtogroup org_custusx_calibration
 * @{
 */

/** GUI for performing temporal calibration
 *
 */
class org_custusx_calibration_EXPORT TemporalCalibrationWidget : public BaseWidget
{
  Q_OBJECT
public:
  TemporalCalibrationWidget(VisServicesPtr services, AcquisitionServicePtr acquisitionService, QWidget* parent);
  virtual ~TemporalCalibrationWidget();

private slots:
  void patientChangedSlot();
  void selectData(QString filename);
  void calibrateSlot();

protected:
  void showEvent(QShowEvent* event);
private:
  TemporalCalibrationPtr mAlgorithm;

  VisServicesPtr mServices;
  FileSelectWidget* mFileSelectWidget;
  QLineEdit* mResult;
  QCheckBox* mVerbose;
  RecordSessionWidget* mRecordSessionWidget;
  QLabel* mInfoLabel;
};


/**
 * @}
 */
}

#endif /* CXTEMPORALCALIBRATIONWIDGET_H_ */
