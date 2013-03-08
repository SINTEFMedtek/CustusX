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

#include "cxRecordSession.h"
#include "cxAcquisitionData.h"
#include "sscTool.h"

namespace cx
{
typedef boost::shared_ptr<class UsReconstructionFileMaker> UsReconstructionFileMakerPtr;
typedef boost::shared_ptr<class SavingVideoRecorder> SavingVideoRecorderPtr;

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
	USAcquisition(AcquisitionPtr base, QObject* parent = 0);
	virtual ~USAcquisition();
//	QString getWhatsMissingText() const { return mWhatsMissing; }
	int getNumberOfSavingThreads() const { return mSaveThreads.size(); }

signals:
//	void toolChanged();
	void acquisitionDataReady(); ///< emitted when data is acquired and sent to the reconstruction module
	void saveDataCompleted(QString mhdFilename); ///< emitted when data has been saved to file

private slots:
//	void probeChangedSlot();
	void fileMakerWriteFinished();
//	void dominantToolChangedSlot();
//	void setTool(ssc::ToolPtr tool);
//	ssc::ToolPtr getTool();

//	void clearSlot();
	void checkIfReadySlot();
	void saveSession();
	void recordStarted();
	void recordStopped();
	void recordCancelled();

private:
	std::vector<ssc::VideoSourcePtr> getRecordingVideoSources();
	bool getWriteColor() const;

	AcquisitionPtr mBase;
//	ssc::VideoSourcePtr mRTSource;
//	SavingVideoRecorderPtr mVideoRecorder;

	// video and tool used at start of recording:
	std::vector<SavingVideoRecorderPtr> mVideoRecorder;
	ssc::ToolPtr mRecordingTool;

//	UsReconstructionFileMakerPtr mCurrentSessionFileMaker;

	std::list<QFutureWatcher<QString>*> mSaveThreads;

//	QString mWhatsMissing;

	virtual ssc::TimedTransformMap getRecording(RecordSessionPtr session);
//	void connectVideoSource(ssc::VideoSourcePtr source);
//	void connectToPureVideo();

};
typedef boost::shared_ptr<USAcquisition> USAcquisitionPtr;

/**
* @}
*/
}

#endif /* CXUSACQUISITION_H_ */
