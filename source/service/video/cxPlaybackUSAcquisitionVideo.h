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
#ifndef CXPLAYBACKUSACQUISITIONVIDEO_H_
#define CXPLAYBACKUSACQUISITIONVIDEO_H_

#include <QObject>
#include <QFuture>
#include <QFutureWatcher>
#include <vector>
#include "sscVideoSource.h"
#include "cxUSReconstructInputData.h"
#include "cxPlaybackTime.h"
#include "cxForwardDeclarations.h"

namespace cx
{
typedef boost::shared_ptr<class BasicVideoSource> BasicVideoSourcePtr;

/**
 * \file
 * \addtogroup cxServiceVideo
 * @{
 */

/**\brief Handler for playback of US image data
 * from a US recording session.
 *
 * \ingroup cxServiceVideo
 * \date Apr 11, 2012
 * \author Christian Askeland, SINTEF
 *
 */
class USAcquisitionVideoPlayback : public QObject
{
	Q_OBJECT
public:
	USAcquisitionVideoPlayback();
	virtual ~USAcquisitionVideoPlayback();
	ssc::VideoSourcePtr getVideoSource();
	void setRoot(const QString path);
	void setTime(PlaybackTimePtr controller);
	bool isActive() const;
	std::vector<TimelineEvent> getEvents();

private slots:
	void timerChangedSlot();
	void usDataLoadFinishedSlot();

private:
	void updateFrame(QString filename);
	void loadFullData(QString filename);
	QStringList getAllFiles(QString folder);
	QString mRoot;
	PlaybackTimePtr mTimer;
//	ssc::ImageImportVideoSourcePtr mVideoSource;
	BasicVideoSourcePtr mVideoSource;
	std::vector<TimelineEvent> mEvents;

	ssc::USReconstructInputData mCurrentData;
	std::vector<double> mCurrentTimestamps; // copy of time frame timestamps from mCurrentData.

	UsReconstructionFileReaderPtr mUSImageDataReader;
	QFuture<ssc::USReconstructInputData> mUSImageDataFutureResult;
	QFutureWatcher<ssc::USReconstructInputData> mUSImageDataFutureWatcher;

};
typedef boost::shared_ptr<USAcquisitionVideoPlayback> USAcquisitionVideoPlaybackPtr;


/**
 * @}
 */
}

#endif /* CXPLAYBACKUSACQUISITIONVIDEO_H_ */
