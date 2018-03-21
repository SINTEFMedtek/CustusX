/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXPLAYBACKUSACQUISITIONVIDEO_H_
#define CXPLAYBACKUSACQUISITIONVIDEO_H_

#include "org_custusx_core_video_Export.h"

#include <QObject>
#include <QFuture>
#include <QFutureWatcher>
#include <vector>
#include "cxVideoSource.h"
#include "cxUSReconstructInputData.h"
#include "cxPlaybackTime.h"
#include "cxForwardDeclarations.h"

namespace cx
{
typedef boost::shared_ptr<class BasicVideoSource> BasicVideoSourcePtr;
typedef boost::shared_ptr<class VideoServiceBackend> VideoServiceBackendPtr;

/**
 * \file
 * \addtogroup org_custusx_core_video
 * @{
 */

/**\brief Handler for playback of US image data
 * from a US recording session.
 *
 * \ingroup org_custusx_core_video
 * \date Apr 11, 2012
 * \author Christian Askeland, SINTEF
 *
 */
class org_custusx_core_video_EXPORT USAcquisitionVideoPlayback : public QObject
{
	Q_OBJECT
public:
    explicit USAcquisitionVideoPlayback(VideoServiceBackendPtr backend, QString type);
	virtual ~USAcquisitionVideoPlayback();
	VideoSourcePtr getVideoSource();
	void setRoot(const QString path);
	void setTime(PlaybackTimePtr controller);
	bool isActive() const;
	std::vector<TimelineEvent> getEvents();

    QString getType() const;

private slots:
    void timerChangedSlot();
    void usDataLoadFinishedSlot();

private:
    void updateFrame(QString filename);
	void loadFullData(QString filename);
	QStringList getAbsolutePathToFtsFiles(QString folder);
	QString mRoot;
    QString mType;
    PlaybackTimePtr mTimer;
	BasicVideoSourcePtr mVideoSource;
	std::vector<TimelineEvent> mEvents;
    const QString mVideoSourceUid;

	USReconstructInputData mCurrentData;
	std::vector<double> mCurrentTimestamps; // copy of time frame timestamps from mCurrentData.

	UsReconstructionFileReaderPtr mUSImageDataReader;
	QFuture<USReconstructInputData> mUSImageDataFutureResult;
	QFutureWatcher<USReconstructInputData> mUSImageDataFutureWatcher;

	VideoServiceBackendPtr mBackend;
};
typedef boost::shared_ptr<USAcquisitionVideoPlayback> USAcquisitionVideoPlaybackPtr;


/**
 * @}
 */
}

#endif /* CXPLAYBACKUSACQUISITIONVIDEO_H_ */
