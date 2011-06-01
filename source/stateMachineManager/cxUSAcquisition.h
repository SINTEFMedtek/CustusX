/*
 * cxUSAcquisition.h
 *
 *  Created on: May 12, 2011
 *      Author: christiana
 */

#ifndef CXUSACQUISITION_H_
#define CXUSACQUISITION_H_

#include <QFuture>
#include <QFutureWatcher>
#include "cxUsReconstructionFileMaker.h"
#include "cxRecordSession.h"

namespace cx
{

/**Handles the us acquisition process
 *
 * Usage:
 *  ready() is emitted when change in readiness occurs. Use getWhatsMissingText() to display status
 *
 *  start/stop record handles the _streaming_ acquisition, NOT the tracking. This is done externally (yet).
 *  saveSession() gives the id of the tracking recorded data as input, and saves all data to disk.
 *                saveDataCompleted() is emitted when saving is finished.
 *
 *  TODO: merge the tracking recording into this class
 *
 */
class USAcquisition : public QObject
{
	Q_OBJECT
public:
	USAcquisition(QObject* parent = 0);
	QString getWhatsMissingText() const { return mWhatsMissing; }

signals:
	void ready(bool, QString);
	void saveDataCompleted(QString mhdFilename);
	void toolChanged();

public slots:
	void checkIfReadySlot();
	void startRecord();
	void stopRecord();
	void saveSession(QString sessionId);

private slots:
  void probeChangedSlot();
  void fileMakerWriteFinished();
  void dominantToolChangedSlot();
  void setTool(ssc::ToolPtr tool);
  ssc::ToolPtr getTool();

private:
  ssc::VideoSourcePtr mRTSource;
  ssc::RealTimeStreamSourceRecorderPtr mRTRecorder;
  ssc::ToolPtr mTool;

  QFuture<QString> mFileMakerFuture;
  QFutureWatcher<QString> mFileMakerFutureWatcher;
  UsReconstructionFileMakerPtr mFileMaker;

  QString mWhatsMissing;

  virtual ssc::TimedTransformMap getRecording(RecordSessionPtr session);

};
typedef boost::shared_ptr<USAcquisition> USAcquisitionPtr;


}

#endif /* CXUSACQUISITION_H_ */
