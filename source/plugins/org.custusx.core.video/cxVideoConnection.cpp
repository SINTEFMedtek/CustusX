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

typedef vtkSmartPointer<vtkDataSetMapper> vtkDataSetMapperPtr;
typedef vtkSmartPointer<vtkImageFlip> vtkImageFlipPtr;

namespace cx
{

VideoConnection::VideoConnection(VideoServiceBackendPtr backend)
{
	mBackend = backend;
	mConnected = false;
	mUnsusedProbeDataVector.clear();

	connect(mBackend->getToolManager().get(), &TrackingService::stateChanged, this, &VideoConnection::connectVideoToProbe);
	connect(mBackend->getToolManager().get(), SIGNAL(activeToolChanged(QString)), this, SLOT(connectVideoToProbe()));
}

VideoConnection::~VideoConnection()
{
	this->stopClient();
}

void VideoConnection::fpsSlot(QString source, double fpsNumber)
{
	mFPS = fpsNumber;
	emit fps(source, fpsNumber);
}

bool VideoConnection::isConnected() const
{
	return mClient && mConnected;
}

void VideoConnection::connectedSlot(bool on)
{
	mConnected = on;

	if (on)
		this->startAllSources();
	else
		this->disconnectServer();

	std::cout << "EMIT CONNECTED " << on << std::endl;
	emit connected(on);
}

StreamerServicePtr VideoConnection::getStreamerInterface()
{
	return mStreamerInterface;
}

void VideoConnection::runDirectLinkClient(StreamerService* service)
{
	mStreamerInterface.reset(service, null_deleter());//Can't allow boost to delete service
	ImageReceiverThreadPtr imageReceiverThread(new ImageReceiverThread(mStreamerInterface));

	this->runClient(imageReceiverThread);
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

void VideoConnection::runClient(ImageReceiverThreadPtr client)
{
	if (mClient)
	{
		std::cout << "client already exist - returning" << std::endl;
		return;
	}
	mClient = client;
	connect(mClient.get(), SIGNAL(imageReceived()), this, SLOT(imageReceivedSlot())); // thread-bridging connection
	connect(mClient.get(), SIGNAL(sonixStatusReceived()), this, SLOT(statusReceivedSlot())); // thread-bridging connection
	connect(mClient.get(), SIGNAL(fps(QString, double)), this, SLOT(fpsSlot(QString, double))); // thread-bridging connection
	connect(mClient.get(), SIGNAL(connected(bool)), this, SLOT(connectedSlot(bool)));

	mThread.reset(new EventProcessingThread);
	mThread->setObjectName("org.custusx.core.video.imagereceiver");
	mClient->moveToThread(mThread.get());
	connect(mClient.get(), &ImageReceiverThread::failedToStart, mThread.get(), &QThread::quit);
	connect(mThread.get(), &QThread::finished, this, &VideoConnection::clientFinishedSlot);
	QMetaObject::invokeMethod(mClient.get(), "initialize", Qt::QueuedConnection);

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
	QThreadPtr thread = mThread;
	mThread.reset(); // avoid multiple entries to this method during event processing.

	QMetaObject::invokeMethod(mClient.get(), "shutdown", Qt::QueuedConnection);
	QString hostdescription = mClient->hostDescription();

	thread->quit();
	int timeout = 2000;
	int interval = 25;
	for (unsigned i=0; i<timeout/interval; ++i)
	{
		if (!thread->isRunning())
			break;
		thread->wait(interval); // forever or until dead thread
		qApp->processEvents();
	}
	if (thread->isRunning())
	{
		reportWarning(QString("Video Client [%1] did not quit normally - attempting to terminate.").arg(hostdescription));
		thread->terminate();
		thread->wait(); // forever or until dead thread
		reportWarning(QString("Video Client [%1] did not quit normally - terminated.").arg(hostdescription));
	}
	thread.reset();

	mClient.reset();
}

void VideoConnection::disconnectServer()
{
	this->stopClient();
	this->cleanupAfterDisconnectServer();
}

void VideoConnection::cleanupAfterDisconnectServer()
{
//	this->stopClient();
	mClient.reset();

	this->resetProbe();

	this->stopAllSources();

	for (unsigned i=0; i<mSources.size(); ++i)
		mSources[i]->setInput(ImagePtr());

	ToolPtr tool = mBackend->getToolManager()->getFirstProbe();
	if (tool && tool->getProbe())
		this->removeSourceFromProbe(tool);

	mSources.clear();
	mStreamerInterface.reset();
	emit videoSourcesChanged();
}

void VideoConnection::clientFinishedSlot()
{
	if (!mClient)
		return;
//	this->disconnectServer();
	this->cleanupAfterDisconnectServer();
}

void VideoConnection::useUnusedProbeDataSlot()
{
	disconnect(mBackend->getToolManager().get(), &TrackingService::stateChanged, this, &VideoConnection::useUnusedProbeDataSlot);
	for (std::vector<ProbeDefinitionPtr>::const_iterator citer = mUnsusedProbeDataVector.begin(); citer != mUnsusedProbeDataVector.end(); ++citer)
		this->updateStatus(*citer);
	mUnsusedProbeDataVector.clear();
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
		if (mUnsusedProbeDataVector.empty())
			connect(mBackend->getToolManager().get(), &TrackingService::stateChanged, this, &VideoConnection::useUnusedProbeDataSlot);
		mUnsusedProbeDataVector.push_back(msg);
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
