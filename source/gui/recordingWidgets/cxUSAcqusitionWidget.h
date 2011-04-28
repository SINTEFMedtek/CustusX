#ifndef CXUSACQUSITIONWIDGET_H_
#define CXUSACQUSITIONWIDGET_H_

#include "cxRecordBaseWidget.h"

#include <QFuture>
#include <QFutureWatcher>

namespace cx
{
/**
 * USAcqusitionWidget
 *
 * \brief
 *
 * \date Dec 9, 2010
 * \author Janne Beate Bakeng, SINTEF
 */
class USAcqusitionWidget : public TrackedRecordWidget
{
  Q_OBJECT
public:
  USAcqusitionWidget(QWidget* parent);
  virtual ~USAcqusitionWidget();

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
  virtual ssc::TimedTransformMap getRecording(RecordSessionPtr session);

  SelectRTSourceStringDataAdapterPtr mRTSourceDataAdapter;
  ssc::RTSourcePtr mRTSource;
  ssc::RealTimeStreamSourceRecorderPtr mRTRecorder;
  ssc::ThreadedReconstructerPtr mThreadedReconstructer;

  QFuture<QString> mFileMakerFuture;
  QFutureWatcher<QString> mFileMakerFutureWatcher;
  UsReconstructionFileMakerPtr mFileMaker;
};
}//namespace cx

#endif /* CXUSACQUSITIONWIDGET_H_ */
