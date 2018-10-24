/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxPlaybackUSAcquisitionVideo.h"
#include <QStringList>
#include <QDir>
#include <QtCore>
#include <boost/bind.hpp>
#include "vtkImageImport.h"
#include "vtkImageData.h"
#include "cxTypeConversions.h"
#include "cxUsReconstructionFileReader.h"
#include "cxTestVideoSource.h"
#include "cxTrackingService.h"
#include "cxReporter.h"
//#include "cxProbeImpl.h"
#include "cxUSFrameData.h"
#include "cxPlaybackTime.h"

#include "cxBasicVideoSource.h"
#include "cxImage.h"
#include "cxImageDataContainer.h"
#include "cxVideoServiceBackend.h"
#include "cxFileHelpers.h"
#include <QtConcurrent>
#include "cxTool.h"

namespace cx
{

USAcquisitionVideoPlayback::USAcquisitionVideoPlayback(VideoServiceBackendPtr backend, QString type) :
	QObject(NULL),
    mVideoSourceUid("playback " + type)
{
    mType=type;
    mBackend = backend;
	mVideoSource.reset(new BasicVideoSource(mVideoSourceUid));
	mVideoSource->setStatusString(QString("No US Acquisition"));

	connect(&mUSImageDataFutureWatcher, SIGNAL(finished()), this, SLOT(usDataLoadFinishedSlot()));
}

USAcquisitionVideoPlayback::~USAcquisitionVideoPlayback()
{
}


VideoSourcePtr USAcquisitionVideoPlayback::getVideoSource()
{
       return mVideoSource;
}

bool USAcquisitionVideoPlayback::isActive() const
{
	return mTimer ? true : false;
}

void USAcquisitionVideoPlayback::setTime(PlaybackTimePtr controller)
{
	if (mTimer)
		disconnect(mTimer.get(), SIGNAL(changed()), this, SLOT(timerChangedSlot()));
	mTimer = controller;
	if (mTimer)
		connect(mTimer.get(), SIGNAL(changed()), this, SLOT(timerChangedSlot()));

	if (controller)
	{
		mVideoSource->start();
	}
	else
	{
		mVideoSource->deconfigure();
	}
}

void USAcquisitionVideoPlayback::setRoot(const QString path)
{
	mRoot = path;
	mEvents = this->getEvents();
}

std::vector<TimelineEvent> USAcquisitionVideoPlayback::getEvents()
{
	std::vector<TimelineEvent> events;

	QStringList allFiles = this->getAbsolutePathToFtsFiles(mRoot);
	for (int i=0; i<allFiles.size(); ++i)
	{
		UsReconstructionFileReader reader(mBackend->file());
		std::vector<TimedPosition> timestamps = reader.readFrameTimestamps(allFiles[i]);

		if (timestamps.empty())
			continue;

		TimelineEvent current(
						QString("Acquisition %1").arg(QFileInfo(allFiles[i]).fileName()),
						timestamps.front().mTime,
						timestamps.back().mTime);
		current.mUid = allFiles[i];
		current.mGroup = "acquisition";
		current.mColor = QColor::fromHsv(36, 255, 222);

		events.push_back(current);

	}

	return events;
}

/**Get all mhd files in folder, recursively.
 */
QStringList USAcquisitionVideoPlayback::getAbsolutePathToFtsFiles(QString folder)
{
    return getAbsolutePathToFiles(folder,QStringList(QString("*_").append(mType)), true);
}

QString USAcquisitionVideoPlayback::getType() const
{
    return mType;
}

void USAcquisitionVideoPlayback::timerChangedSlot()
{
    TimelineEvent event;
    for (unsigned i=0; i<mEvents.size(); ++i)
    {
        if (mEvents[i].isInside(mTimer->getTime().toMSecsSinceEpoch()))
		{
			event = mEvents[i];
			break;
		}
	}

	this->loadFullData(event.mUid);
	this->updateFrame(event.mUid);
}

void USAcquisitionVideoPlayback::loadFullData(QString filename)
{
	//	 if same filename, ok and return
	if (filename == mCurrentData.mFilename)
		return;

	mVideoSource->setInfoString(QString(""));
	mVideoSource->setStatusString(QString("No US Acquisition"));
	mVideoSource->deconfigure();

	// if no data: ignore but keep the already loaded data
	if (filename.isEmpty())
		return;

	// clear data
	mCurrentData = USReconstructInputData();

	// if no new data, return
	if (filename.isEmpty())
		return;

	// load new data
	// start an asynchronous read of data
	if (!mUSImageDataReader)
	{
		mUSImageDataReader.reset(new UsReconstructionFileReader(mBackend->file()));
		mUSImageDataFutureResult = QtConcurrent::run(boost::bind(&UsReconstructionFileReader::readAllFiles, mUSImageDataReader, filename, ""));
		mUSImageDataFutureWatcher.setFuture(mUSImageDataFutureResult);
	}
}

void USAcquisitionVideoPlayback::usDataLoadFinishedSlot()
{
	// file read operation has completed: read and clear
	mCurrentData = mUSImageDataFutureResult.result();
	mCurrentData.mProbeDefinition.mData.setUid(mVideoSourceUid);
	// clear result so we can check for it next run
	mUSImageDataReader.reset();

	mVideoSource->start();

	// set the probe sector from file data:
	ToolPtr tool = mBackend->tracking()->getFirstProbe();
	if (tool)
	{
		ProbePtr probe = tool->getProbe();
		if (probe)
			probe->setProbeDefinition(mCurrentData.mProbeDefinition.mData);
	}

	// create a vector to allow for quick search
	mCurrentTimestamps.clear();
	for (unsigned i=0; i<mCurrentData.mFrames.size(); ++i)
		mCurrentTimestamps.push_back(mCurrentData.mFrames[i].mTime);

	this->updateFrame(mCurrentData.mFilename);
}

void  USAcquisitionVideoPlayback::updateFrame(QString filename)
{
	if (mUSImageDataReader)
	{
		mVideoSource->setInfoString(QString("Loading US Data..."));
		mVideoSource->setStatusString(QString("Loading US Data..."));
		mVideoSource->setInput(ImagePtr());
		return;
	}

	if (mCurrentData.mFilename.isEmpty() || !mCurrentData.mUsRaw || filename!=mCurrentData.mFilename)
	{
		mVideoSource->setInfoString(QString(""));
		mVideoSource->setStatusString(QString("No US Acquisition"));
		mVideoSource->setInput(ImagePtr());
		return;
	}

	// if not already started:
	mVideoSource->start();

	double timestamp = mTimer->getTime().toMSecsSinceEpoch();

	// find index of current frame: Use the last frame _before_ the current timestamp.
	std::vector<double>::iterator iter = std::lower_bound(mCurrentTimestamps.begin(), mCurrentTimestamps.end(), timestamp);
	if (iter==mCurrentTimestamps.begin())
		return;
	--iter; // use the frame before, not after.
	int index = std::distance(mCurrentTimestamps.begin(), iter);

	int timeout = 1000; // invalidate data if timestamp differ from time too much
	mVideoSource->overrideTimeout(fabs(timestamp-*iter)>timeout);

	ImagePtr image(new Image(mVideoSourceUid, mCurrentData.mUsRaw->getImageContainer()->get(index)));
	image->setAcquisitionTime(QDateTime::fromMSecsSinceEpoch(timestamp));

	mVideoSource->setInfoString(QString("%1 - Frame %2").arg(mCurrentData.mUsRaw->getName()).arg(index));
	if (mVideoSource->validData())
		mVideoSource->setStatusString(QString(""));
	else
		mVideoSource->setStatusString(QString("Timeout"));

	mVideoSource->setInput(image);
}



} // cx

