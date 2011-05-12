/*
 * cxTemporalCalibrationWidget.h
 *
 *  Created on: May 4, 2011
 *      Author: christiana
 */

#ifndef CXTEMPORALCALIBRATIONWIDGET_H_
#define CXTEMPORALCALIBRATIONWIDGET_H_

#include "cxBaseWidget.h"
#include "cxRecordBaseWidget.h"
#include <QFuture>
#include <QFutureWatcher>
#include "sscFileSelectWidget.h"
#include "cxUsReconstructionFileReader.h"
#include <cxTemporalCalibration.h>
#include "cxUSAcqusitionWidget.h"

namespace cx
{

/** GUI for performing temporal calibration
 */
class TemporalCalibrationWidget : public BaseWidget
{
  Q_OBJECT
public:
  TemporalCalibrationWidget(QWidget* parent);
  virtual ~TemporalCalibrationWidget();

  virtual QString defaultWhatsThis() const;

private slots:
  void patientChangedSlot();
  void selectData(QString filename);
  void calibrateSlot();

protected:
  void showEvent(QShowEvent* event);
private:
  USAcquisitionPtr mAcquisition;
  TemporalCalibrationPtr mAlgorithm;

  ssc::FileSelectWidget* mFileSelectWidget;
  QLineEdit* mResult;
  QCheckBox* mVerbose;
  RecordSessionWidget* mRecordSessionWidget;
  QLabel* mInfoLabel;
};


}

#endif /* CXTEMPORALCALIBRATIONWIDGET_H_ */
