/*
 * cxUSAcquisition.h
 *
 *  \date May 12, 2011
 *      \author christiana
 */

#ifndef CXUSACQUISITION_H_
#define CXUSACQUISITION_H_

#include <QFuture>
#include <QFutureWatcher>
#include "cxUsReconstructionFileMaker.h"
#include "cxRecordSession.h"
#include <cxAcquisitionData.h>
#include "cxSavingVideoRecorder.h"

namespace cx
{

/**
 * \file
 * \addtogroup cxPluginAcquisition
 * @{
 */

/**
 * \brief Handles the us acquisition process.
 * \ingroup cxPluginAcquisition
 *
 * Usage:
 *  - ready() is emitted when change in readiness occurs. Use
 *    getWhatsMissingText() to display status
 *  - start/stop record handles the _streaming_ acquisition, NOT the tracking.
 *    This is done externally (yet).
 *  - saveSession() gives the id of the tracking recorded data as input, and
 *    saves all data to disk.
 *  - saveDataCompleted() is emitted when saving is finished.
 *
 *  TODO: merge the tracking recording into this class
 *
 */
class USAcquisition : public QObject
{
	Q_OBJECT
public:
	USAcquisition(AcquisitionDataPtr pluginData, QObject* parent = 0);
	QString getWhatsMissingText() const { return mWhatsMissing; }

signals:
	void ready(bool, QString);
	void toolChanged();
	void acquisitionDataReady(); ///< emitted when data is acquired and sent to the reconstruction module
	void saveDataCompleted(QString mhdFilename); ///< emitted when data has been saved to file

public slots:
	void checkIfReadySlot();
	void startRecord(QString sessionId);
	void stopRecord();
	void saveSession(QString sessionId, bool writeColor = false);///< \param writeColor If set to true, colors will be saved even if settings is set to 8 bit
	void clearSlot();

private slots:
	void probeChangedSlot();
	void fileMakerWriteFinished();
	void dominantToolChangedSlot();
	void setTool(ssc::ToolPtr tool);
	ssc::ToolPtr getTool();

private:
	AcquisitionDataPtr mPluginData;
	ssc::VideoSourcePtr mRTSource;
	SavingVideoRecorderPtr mVideoRecorder;
//	ssc::VideoRecorderPtr mRTRecorder;
	ssc::ToolPtr mTool;

	//  QFuture<QString> mFileMakerFuture;
	//  QFutureWatcher<QString> mFileMakerFutureWatcher;
	//  UsReconstructionFileMakerPtr mFileMaker;
	UsReconstructionFileMakerPtr mCurrentSessionFileMaker;

	std::list<QFutureWatcher<QString>*> mSaveThreads;

	QString mWhatsMissing;

	virtual ssc::TimedTransformMap getRecording(RecordSessionPtr session);
	void connectVideoSource(ssc::VideoSourcePtr source);
	void connectToPureVideo();

};
typedef boost::shared_ptr<USAcquisition> USAcquisitionPtr;

/**
* @}
*/
}

#endif /* CXUSACQUISITION_H_ */
