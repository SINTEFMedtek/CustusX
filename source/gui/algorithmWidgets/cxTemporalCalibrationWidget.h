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

private:
  ssc::FileSelectWidget* mFileSelectWidget;
  TemporalCalibrationPtr mAlgorithm;
  QLineEdit* mResult;
  QCheckBox* mVerbose;
};


}

#endif /* CXTEMPORALCALIBRATIONWIDGET_H_ */
