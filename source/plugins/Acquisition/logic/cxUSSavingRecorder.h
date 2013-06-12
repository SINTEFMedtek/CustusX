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
#ifndef CXUSSAVINGRECORDER_H
#define CXUSSAVINGRECORDER_H

#include <vector>
#include <QFutureWatcher>
#include "cxForwardDeclarations.h"
#include "sscTransform3D.h"

namespace ssc
{
class USReconstructInputData;
}
namespace cx
{
typedef boost::shared_ptr<class UsReconstructionFileMaker> UsReconstructionFileMakerPtr;
typedef boost::shared_ptr<class SavingVideoRecorder> SavingVideoRecorderPtr;
typedef boost::shared_ptr<class RecordSession> RecordSessionPtr;

/**
 * \file
 * \addtogroup cxPluginAcquisition
 * @{
 */


/**
 * \brief Record and save ultrasound data.
 * \ingroup cxPluginAcquisition
 *
 * Use the start/stop pair to record video from the input streams
 * during that period. A cancel instead of stop will clear the recording.
 *
 * After stopping, use
 *    - getDataForStream() to get unsaved reconstruct data.
 *    - startSaveData() to launch save threads, emitting signals for each completed save.
 *
 * Use clearRecording() to free memory and temporary files (this can be a lot of disk space).
 *
 * Intended to be a unit-testable part of the USAcquisition class.
 *
 *  \date April 17, 2013
 *  \author christiana
 */
class USSavingRecorder : public QObject
{
	Q_OBJECT
public:
	USSavingRecorder();
	/**
	  * Start recording
	  */
	void startRecord(RecordSessionPtr session, ssc::ToolPtr tool, std::vector<ssc::VideoSourcePtr> video);
	void stopRecord();
	void cancelRecord();

	void setWriteColor(bool on);
	void set_rMpr(ssc::Transform3D rMpr);
	/**
	  * Retrieve an in-memory data set for the given stream uid.
	  */
	ssc::USReconstructInputData getDataForStream(QString streamUid);
	/**
	  * Start saving all data acquired after a start/stop record.
	  * A separate saveDataCompleted() signal is emitted
	  * for each completed saved stream.
	  */
	void startSaveData(QString baseFolder, bool compressImages);
	unsigned getNumberOfSavingThreads() const;
	void clearRecording();

signals:
	void saveDataCompleted(QString mhdFilename); ///< emitted when data has been saved to file

private slots:
	void fileMakerWriteFinished();
private:
	std::map<double, ssc::Transform3D> getRecording();
	void saveStreamSession(ssc::USReconstructInputData reconstructData, QString saveFolder, QString streamSessionName, bool compress);
	ssc::USReconstructInputData getDataForStream(unsigned videoRecorderIndex);

	RecordSessionPtr mSession;
	std::list<QFutureWatcher<QString>*> mSaveThreads;
	// video and tool used at start of recording:
	std::vector<SavingVideoRecorderPtr> mVideoRecorder;
	ssc::ToolPtr mRecordingTool;
	bool mDoWriteColor;
	ssc::Transform3D m_rMpr;
};
typedef boost::shared_ptr<USSavingRecorder> USSavingRecorderPtr;

/**
* @}
*/
}

#endif // CXUSSAVINGRECORDER_H
