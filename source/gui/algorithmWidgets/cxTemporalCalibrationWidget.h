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

namespace cx
{

/** GUI for performing temporal calibration
 */
class TemporalCalibrationWidget : public TrackedRecordWidget
{
  Q_OBJECT
public:
  TemporalCalibrationWidget(QWidget* parent);
  virtual ~TemporalCalibrationWidget();

  virtual QString defaultWhatsThis() const;

protected slots:
  void checkIfReadySlot();
  void postProcessingSlot(QString sessionId);
  void startedSlot();
  void stoppedSlot();

private slots:
  void probeChangedSlot();
  void reconstructFinishedSlot();
  void fileMakerWriteFinished();
  void dominantToolChangedSlot();

private:
  void computeTemporalCalibration(ssc::RTSourceRecorder::DataType volumes, ssc::TimedTransformMap tracking, ssc::ToolPtr probe);

  virtual ssc::TimedTransformMap getRecording(RecordSessionPtr session);

  SelectRTSourceStringDataAdapterPtr mRTSourceDataAdapter;
  ssc::RTSourcePtr mRTSource;
  ssc::RealTimeStreamSourceRecorderPtr mRTRecorder;
//  ssc::ThreadedReconstructerPtr mThreadedReconstructer;

  QFuture<QString> mFileMakerFuture;
  QFutureWatcher<QString> mFileMakerFutureWatcher;
  UsReconstructionFileMakerPtr mFileMaker;

  QString mLastSession;
};


}

#endif /* CXTEMPORALCALIBRATIONWIDGET_H_ */
