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

#include "cxPlaybackUSAcquisitionVideo.h"
#include <QStringList>
#include <QDir>
#include <QtCore>
#include <boost/bind.hpp>
#include "vtkImageImport.h"
#include "vtkImageData.h"
#include "sscTypeConversions.h"
#include "cxUsReconstructionFileReader.h"
#include "sscTestVideoSource.h"
#include "sscImageImportVideoSource.h"
#include "cxToolManager.h"
#include "cxProbe.h"
#include "sscUSFrameData.h"
#include "cxPlaybackTime.h"
#include "sscLogger.h"
#include "cxBasicVideoSource.h"
#include "sscImage.h"
#include "cxImageDataContainer.h"

namespace cx
{

USAcquisitionVideoPlayback::USAcquisitionVideoPlayback() : QObject(NULL), mVideoSourceUid("playback")
{
	mVideoSource.reset(new BasicVideoSource(mVideoSourceUid));
	mVideoSource->setStatusString(QString("No US Acquisition"));

	connect(&mUSImageDataFutureWatcher, SIGNAL(finished()), this, SLOT(usDataLoadFinishedSlot()));
}

USAcquisitionVideoPlayback::~USAcquisitionVideoPlayback()
{
}

ssc::VideoSourcePtr USAcquisitionVideoPlayback::getVideoSource()
{
	return mVideoSource;
}

bool USAcquisitionVideoPlayback::isActive() const
{
	return mTimer;
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

	QStringList allFiles = this->getAllFiles(mRoot);
	for (int i=0; i<allFiles.size(); ++i)
	{
		UsReconstructionFileReader reader;
		std::vector<ssc::TimedPosition> timestamps = reader.readFrameTimestamps(allFiles[i]);

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
 *
 * Cut and paste from ssc::FileSelectWidget .
 *
 */
QStringList USAcquisitionVideoPlayback::getAllFiles(QString folder)
{
	QStringList mNameFilters;
	mNameFilters << "*.fts";

	QDir dir(folder);
	QStringList files = dir.entryList(mNameFilters, QDir::Files);

	QStringList retval;
	for (int i = 0; i < files.size(); ++i)
	{
		retval << (dir.absolutePath() + "/" + files[i]);
	}
	QStringList folders = dir.entryList(QStringList(), QDir::AllDirs | QDir::NoDotAndDotDot);

	for (int i = 0; i < folders.size(); ++i)
	{
		files = this->getAllFiles(folder + "/" + folders[i]);
		retval.append(files);
	}

	return retval;
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
	mCurrentData = ssc::USReconstructInputData();

	// if no new data, return
	if (filename.isEmpty())
		return;

	// load new data
	// start an asynchronous read of data
	if (!mUSImageDataReader)
	{
		mUSImageDataReader.reset(new UsReconstructionFileReader());
		mUSImageDataFutureResult = QtConcurrent::run(boost::bind(&UsReconstructionFileReader::readAllFiles, mUSImageDataReader, filename, ""));
		mUSImageDataFutureWatcher.setFuture(mUSImageDataFutureResult);
	}
}

void USAcquisitionVideoPlayback::usDataLoadFinishedSlot()
{
	// file read operation has completed: read and clear
	mCurrentData = mUSImageDataFutureResult.result();
	mCurrentData.mProbeData.mData.setUid(mVideoSourceUid);
	// clear result so we can check for it next run
	mUSImageDataReader.reset();

	mVideoSource->start();

	// set the probe sector from file data:
	ssc::ToolPtr tool = ToolManager::getInstance()->findFirstProbe();
	if (tool)
	{
		ProbePtr probe = boost::dynamic_pointer_cast<Probe>(tool->getProbe());
		if (probe)
			probe->setProbeSector(mCurrentData.mProbeData.mData);
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
		mVideoSource->setInput(ssc::ImagePtr());
		return;
	}

	if (mCurrentData.mFilename.isEmpty() || !mCurrentData.mUsRaw || filename!=mCurrentData.mFilename)
	{
		mVideoSource->setInfoString(QString(""));
		mVideoSource->setStatusString(QString("No US Acquisition"));
		mVideoSource->setInput(ssc::ImagePtr());
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

	ssc::ImagePtr image(new ssc::Image(mVideoSourceUid, mCurrentData.mUsRaw->getImageContainer()->get(index)));
	image->setAcquisitionTime(QDateTime::fromMSecsSinceEpoch(timestamp));

	mVideoSource->setInfoString(QString("%1 - Frame %2").arg(mCurrentData.mUsRaw->getName()).arg(index));
	if (mVideoSource->validData())
		mVideoSource->setStatusString(QString(""));
	else
		mVideoSource->setStatusString(QString("Timeout"));

	mVideoSource->setInput(image);
}



} // cx

