/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
