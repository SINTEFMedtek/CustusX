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
    mVideoSourceUid(type.append("_playback"))
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
		UsReconstructionFileReader reader;
		std::vector<TimedPosition> timestamps = reader.readFrameTimestamps(allFiles[i]);

		if (timestamps.empty())
			continue;

		TimelineEvent current(
						QString("Acquisition %1").arg(QFileInfo(allFiles[i]).fileName()),
						timestamps.front().mTime,
						timestamps.back().mTime);
        report("------------------------ here");
        report(allFiles[i]);
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
    /*QStringList nameFilters;
    nameFilters << "*TissueAngio.fts" << "*TissueFlow.fts" << "*ScanConverted.fts";
    QStringList res = getAbsolutePathToFiles(folder,nameFilters, true);
    if(res.isEmpty())
    {
        res=getAbsolutePathToFiles(folder,QStringList("*.fts"), true);
    }
    */
    QStringList res = getAbsolutePathToFiles(folder,QStringList("*.fts"), true);
    return res;
}

void USAcquisitionVideoPlayback::timerChangedSlot()
{
    TimelineEvent event;
    for (unsigned i=0; i<mEvents.size(); ++i)
    {
        if (mEvents[i].isInside(mTimer->getTime().toMSecsSinceEpoch()) && mEvents[i].mUid.endsWith(mType))
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
		mUSImageDataReader.reset(new UsReconstructionFileReader());
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
	ToolPtr tool = mBackend->getToolManager()->getFirstProbe();
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

