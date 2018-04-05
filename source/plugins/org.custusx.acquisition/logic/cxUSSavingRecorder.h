/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXUSSAVINGRECORDER_H
#define CXUSSAVINGRECORDER_H

#include "org_custusx_acquisition_Export.h"

#include <vector>
#include <QFutureWatcher>
#include "cxForwardDeclarations.h"
#include "cxTransform3D.h"

namespace cx
{
struct USReconstructInputData;
}
namespace cx
{
typedef boost::shared_ptr<class UsReconstructionFileMaker> UsReconstructionFileMakerPtr;
typedef boost::shared_ptr<class SavingVideoRecorder> SavingVideoRecorderPtr;
typedef boost::shared_ptr<class RecordSession> RecordSessionPtr;

/**
 * \file
 * \addtogroup org_custusx_acquisition
 * @{
 */


/**
 * \brief Record and save ultrasound data.
 * \ingroup org_custusx_acquisition
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
class org_custusx_acquisition_EXPORT USSavingRecorder : public QObject
{
	Q_OBJECT
public:
	USSavingRecorder();
	virtual ~USSavingRecorder();
	/**
	  * Start recording
	  */
	void startRecord(RecordSessionPtr session, ToolPtr tool, ToolPtr reference, std::vector<VideoSourcePtr> video);
	void stopRecord();
	void cancelRecord();

	void setWriteColor(bool on);
	void set_rMpr(Transform3D rMpr);
	/**
	  * Retrieve an in-memory data set for the given stream uid.
	  */
	USReconstructInputData getDataForStream(QString streamUid);
	/**
	  * Start saving all data acquired after a start/stop record.
	  * A separate saveDataCompleted() signal is emitted
	  * for each completed saved stream.
	  */
	void startSaveData(QString baseFolder, bool compressImages);
	size_t getNumberOfSavingThreads() const;
	void clearRecording();

signals:
	void saveDataCompleted(QString mhdFilename); ///< emitted when data has been saved to file

private slots:
	void fileMakerWriteFinished();
private:
//	std::map<double, Transform3D> getToolHistory(ToolPtr tool, RecordSessionPtr session);
	void saveStreamSession(USReconstructInputData reconstructData, QString saveFolder, QString streamSessionName, bool compress);
	USReconstructInputData getDataForStream(unsigned videoRecorderIndex);

	RecordSessionPtr mSession;
	std::list<QFutureWatcher<QString>*> mSaveThreads;
	// video and tool used at start of recording:
	std::vector<SavingVideoRecorderPtr> mVideoRecorder;
	ToolPtr mRecordingTool;
	ToolPtr mReference;
	bool mDoWriteColor;
	Transform3D m_rMpr;
};
typedef boost::shared_ptr<USSavingRecorder> USSavingRecorderPtr;

/**
* @}
*/
}

#endif // CXUSSAVINGRECORDER_H
