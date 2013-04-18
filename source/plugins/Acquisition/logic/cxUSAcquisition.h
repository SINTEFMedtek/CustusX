// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#ifndef CXUSACQUISITION_H_
#define CXUSACQUISITION_H_

#include <QFuture>
#include <QFutureWatcher>

#include "cxRecordSession.h"
#include "cxAcquisitionData.h"
#include "sscTool.h"

namespace ssc
{
class USReconstructInputData;
}
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
 * \brief Independent algorithms for storing acquisition data.
 * \ingroup cxPluginAcquisition
 *
 * Use the start/stop pair to record video from the input streams
 * during that period. A cancel instead of stop will clear the recording.
 * After stopping, use getDataForStream() to get unsaved reconstruct data.
 * Use startSaveData() to launch save threads AND clear the stored data.
 *
 * Intended to be a unit-testable part of the USAcquisition class.
 *
 *  \date April 17, 2013
 *  \author christiana
 */
class USAcquisitionCore : public QObject
{
	Q_OBJECT
public:
	/**
	  * Start recording
	  */
	void startRecord(RecordSessionPtr session, ssc::ToolPtr tool, std::vector<ssc::VideoSourcePtr> video);
	void stopRecord();
	void cancelRecord();

	void set_rMpr(ssc::Transform3D rMpr);
	/**
	  * Retrieve an in-memory data set for the given stream uid.
	  */
	ssc::USReconstructInputData getDataForStream(QString streamUid);
	/**
	  * Start saving all data acquired after a start/stop record.
	  * A separate saveDataCompleted() signal is emitted
	  * for each completed saved stream.
	  * Internal record data is cleared after this call.
	  */
	void startSaveData(bool compressImages, bool writeColor);
	unsigned numberOfSavingThreads() const;

signals:
	void saveDataCompleted(QString mhdFilename); ///< emitted when data has been saved to file

private:
//	std::vector<SavingVideoRecorderPtr> mVideoRecorder;
//	ssc::ToolPtr mRecordingTool;
};
typedef boost::shared_ptr<USAcquisitionCore> USAcquisitionCorePtr;

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
 *  \date May 12, 2011
 *  \author christiana
 */
class USAcquisition : public QObject
{
	Q_OBJECT
public:
	USAcquisition(AcquisitionPtr base, QObject* parent = 0);
	virtual ~USAcquisition();
	int getNumberOfSavingThreads() const { return mSaveThreads.size(); }

signals:
	void acquisitionDataReady(); ///< emitted when data is acquired and sent to the reconstruction module
	void saveDataCompleted(QString mhdFilename); ///< emitted when data has been saved to file

private slots:
	void fileMakerWriteFinished();
	void checkIfReadySlot();
	void saveSession();
	void recordStarted();
	void recordStopped();
	void recordCancelled();

private:
	std::vector<ssc::VideoSourcePtr> getRecordingVideoSources();
	bool getWriteColor() const;
	void saveStreamSession(ssc::USReconstructInputData reconstructData, QString saveFolder, QString streamSessionName);

	AcquisitionPtr mBase;

	// video and tool used at start of recording:
	std::vector<SavingVideoRecorderPtr> mVideoRecorder;
	ssc::ToolPtr mRecordingTool;

	std::list<QFutureWatcher<QString>*> mSaveThreads;

	virtual ssc::TimedTransformMap getRecording(RecordSessionPtr session);

};
typedef boost::shared_ptr<USAcquisition> USAcquisitionPtr;

/**
* @}
*/
}

#endif /* CXUSACQUISITION_H_ */
