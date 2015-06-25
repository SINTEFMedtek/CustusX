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
#include "cxVideoConnection.h"

#include <vtkDataSetMapper.h>
#include <vtkImageFlip.h>

#include "cxTrackingService.h"
#include "cxBasicVideoSource.h"
#include "cxVideoServiceBackend.h"
#include "cxNullDeleter.h"
#include "cxImageReceiverThread.h"
#include "cxImage.h"
#include "cxLogger.h"
#include <QApplication>
#include "boost/function.hpp"
#include "boost/bind.hpp"

typedef vtkSmartPointer<vtkDataSetMapper> vtkDataSetMapperPtr;
typedef vtkSmartPointer<vtkImageFlip> vtkImageFlipPtr;

namespace cx
{

VideoConnection::VideoConnection(VideoServiceBackendPtr backend)
{
	mBackend = backend;
	mUnusedProbeDataVector.clear();

	connect(mBackend->getToolManager().get(), &TrackingService::stateChanged, this, &VideoConnection::connectVideoToProbe);
	connect(mBackend->getToolManager().get(), SIGNAL(activeToolChanged(QString)), this, SLOT(connectVideoToProbe()));
}

VideoConnection::~VideoConnection()
{
	this->stopClient();
	this->waitForClientFinished();
}

void VideoConnection::waitForClientFinished()
{
	if (mThread)
	{
		mThread->wait(2000);
		// NOTE: OpenCV requires a running event loop in the main thread to quit normally.
		// During system shutdown, this is not the case and we get this warning.
		// Attempts to solve using qApp->processEvents() has failed due to (lots of) side effects.
		// Seems to happen with other streamers as well.
//		CX_LOG_WARNING() << "Video thread finished: " << mThread->isFinished();
//		CX_LOG_WARNING() << "Video thread running: " << mThread->isRunning();
//		if (mThread->isRunning())
//			CX_LOG_WARNING() << "Video thread did not quit normally - ignoring.";
	}
}

void VideoConnection::fpsSlot(QString source, double fpsNumber)
{
	mFPS = fpsNumber;
	emit fps(source, fpsNumber);
}

bool VideoConnection::isConnected() const
{
	return mThread;
}

StreamerServicePtr VideoConnection::getStreamerInterface()
{
	return mStreamerInterface;
}


namespace
{
class EventProcessingThread : public QThread
{
	virtual void run()
	{
		this->exec();
		qApp->processEvents(); // exec() docs doesn't guarantee that the posted events are processed. - do that here.
	}
};
}

void VideoConnection::runDirectLinkClient(StreamerServicePtr service)
{
	if (mClient)
	{
		// in this case we already have a working system: ignore
		CX_LOG_INFO() << "Video client already exists - cannot start";
		return;
	}
	if (mThread)
	{
		// in this case we have a thread but no client: probably shutting down: ignore
		CX_LOG_INFO() << "Video thread already exists (in shutdown?) - cannot start";
		return;
	}

    mStreamerInterface = service;
	mClient = new ImageReceiverThread(mStreamerInterface);

	connect(mClient.data(), &ImageReceiverThread::imageReceived, this, &VideoConnection::imageReceivedSlot); // thread-bridging connection
	connect(mClient.data(), &ImageReceiverThread::sonixStatusReceived, this, &VideoConnection::statusReceivedSlot); // thread-bridging connection
	connect(mClient.data(), &ImageReceiverThread::fps, this, &VideoConnection::fpsSlot); // thread-bridging connection

	mThread = new EventProcessingThread;
	mThread->setObjectName("org.custusx.core.video.imagereceiver");
	mClient->moveToThread(mThread);

    connect(mThread.data(), &QThread::started, this, &VideoConnection::onConnected);
    connect(mThread.data(), &QThread::started, mClient.data(), &ImageReceiverThread::initialize);

    connect(mClient.data(), &ImageReceiverThread::finished, mThread.data(), &QThread::quit);
    connect(mClient.data(), &ImageReceiverThread::finished, mClient.data(), &ImageReceiverThread::deleteLater);
    connect(mThread.data(), &QThread::finished, this, &VideoConnection::onDisconnected);
    connect(mThread.data(), &QThread::finished, mThread.data(), &QThread::deleteLater);

	mThread->start();
}

void VideoConnection::imageReceivedSlot()
{
	if (!mClient)
		return;
	this->updateImage(mClient->getLastImageMessage());
}

void VideoConnection::statusReceivedSlot()
{
	if (!mClient)
		return;
	this->updateStatus(mClient->getLastSonixStatusMessage());
}

void VideoConnection::stopClient()
{
	if (!mThread)
		return;

	if (mClient)
	{
		disconnect(mClient.data(), &ImageReceiverThread::imageReceived, this, &VideoConnection::imageReceivedSlot); // thread-bridging connection
		disconnect(mClient.data(), &ImageReceiverThread::sonixStatusReceived, this, &VideoConnection::statusReceivedSlot); // thread-bridging connection
		disconnect(mClient.data(), &ImageReceiverThread::fps, this, &VideoConnection::fpsSlot); // thread-bridging connection

		QMetaObject::invokeMethod(mClient, "shutdown", Qt::QueuedConnection);

		mClient = NULL;
	}
}

void VideoConnection::disconnectServer()
{
	this->stopClient();
}

void VideoConnection::onConnected()
{
	this->startAllSources();
	emit connected(true);
}

void VideoConnection::onDisconnected()
{
	mClient = NULL;
	mThread = NULL; // because this method listens to thread::finished

	this->resetProbe();

	this->stopAllSources();

	for (unsigned i=0; i<mSources.size(); ++i)
		mSources[i]->setInput(ImagePtr());

	ToolPtr tool = mBackend->getToolManager()->getFirstProbe();
	if (tool && tool->getProbe())
		this->removeSourceFromProbe(tool);

	mSources.clear();
	mStreamerInterface.reset();

	emit connected(false);
	emit videoSourcesChanged();
}

void VideoConnection::useUnusedProbeDataSlot()
{
	disconnect(mBackend->getToolManager().get(), &TrackingService::stateChanged, this, &VideoConnection::useUnusedProbeDataSlot);

	std::vector<ProbeDefinitionPtr> unusedProbeDataVector = mUnusedProbeDataVector;
	mUnusedProbeDataVector.clear();

	for (unsigned i = 0;  i < unusedProbeDataVector.size(); ++i)
		this->updateStatus(unusedProbeDataVector[i]);
}

void VideoConnection::resetProbe()
{
	ToolPtr tool = mBackend->getToolManager()->getFirstProbe();
	if (!tool || !tool->getProbe())
		return;
	ProbePtr probe = tool->getProbe();
	if (probe)
	{
		ProbeDefinition data = probe->getProbeData();
		data.setUseDigitalVideo(false);
		probe->setProbeSector(data);
	}
}

/** extract information from the IGTLinkUSStatusMessage
 *  and store locally. Also reset the old local info with
 *  information from the probe in toolmanager.
 */
void VideoConnection::updateStatus(ProbeDefinitionPtr msg)
{
	ToolPtr tool = mBackend->getToolManager()->getFirstProbe();
	if (!tool || !tool->getProbe())
	{
		//Don't throw away the ProbeData. Save it until it can be used
		if (mUnusedProbeDataVector.empty())
			connect(mBackend->getToolManager().get(), &TrackingService::stateChanged, this, &VideoConnection::useUnusedProbeDataSlot);
		mUnusedProbeDataVector.push_back(msg);
		return;
	}
	ProbePtr probe = tool->getProbe();

	// start with getting a valid data object from the probe, in order to keep
	// existing values (such as temporal calibration).
	// Note that the 'active' data is get while the 'uid' data is set.
	ProbeDefinition data = probe->getProbeData();

	data.setUid(msg->getUid());
	data.setType(msg->getType());
	data.setSector(msg->getDepthStart(), msg->getDepthEnd(), msg->getWidth());
	data.setOrigin_p(msg->getOrigin_p());
	data.setSize(msg->getSize());
	data.setSpacing(msg->getSpacing());
	data.setClipRect_p(msg->getClipRect_p());
	data.setUseDigitalVideo(true);

	probe->setProbeSector(data);
	probe->setActiveStream(msg->getUid());
}

void VideoConnection::startAllSources()
{
	for (unsigned i=0; i<mSources.size(); ++i)
		mSources[i]->start();
}

void VideoConnection::stopAllSources()
{
	for (unsigned i=0; i<mSources.size(); ++i)
		mSources[i]->stop();
}

void VideoConnection::removeSourceFromProbe(ToolPtr tool)
{
	ProbePtr probe = tool->getProbe();
	for (unsigned i=0; i<mSources.size(); ++i)
		probe->removeRTSource(mSources[i]);
}

void VideoConnection::updateImage(ImagePtr message)
{
	BasicVideoSourcePtr source;

	// look for existing VideoSource
	for (unsigned i=0; i<mSources.size(); ++i)
	{
		if (message && message->getUid() == mSources[i]->getUid())
			source = mSources[i];
	}

	bool newSource = false;
	// no existing found: create new
	if (!source)
	{
		source.reset(new BasicVideoSource());
		mSources.push_back(source);
		source->start();
		newSource = true;
	}
	// set input.
	source->setInput(message);

	QString info = mClient->hostDescription() + " - " + QString::number(mFPS, 'f', 1) + " fps";
	source->setInfoString(info);

	if (newSource)
	{
		this->connectVideoToProbe();
		emit videoSourcesChanged();
	}
}

std::vector<VideoSourcePtr> VideoConnection::getVideoSources()
{
	std::vector<VideoSourcePtr> retval;
	std::copy(mSources.begin(), mSources.end(), std::back_inserter(retval));
	return retval;
}

/** Imbue probe with all stream and probe info from grabber.
 *
 * Call when active probe is changed or when streaming config is changed (new streams, new probedata)
 *
 * Find the active probe, then insert all current streams into that probe.
 *
 */
void VideoConnection::connectVideoToProbe()
{
	ToolPtr tool = mBackend->getToolManager()->getFirstProbe();
	if (!tool)
		return;

	ProbePtr probe = tool->getProbe();
	if (!probe)
		return;

	for (unsigned i=0; i<mSources.size(); ++i)
		probe->setRTSource(mSources[i]);
}

}
