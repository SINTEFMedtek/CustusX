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
