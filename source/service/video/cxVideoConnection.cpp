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

/*
 * sscOpenIGTLinkRTSource.cpp
 *
 *  \date Oct 31, 2010
 *      \author christiana
 */
#include "cxVideoConnection.h"

#include <math.h>
#include <vtkImageData.h>
#include <vtkImageImport.h>
#include <vtkDataSetMapper.h>
#include <vtkTimerLog.h>
#include <vtkImageFlip.h>
#include <QTimer>
#include "vtkForwardDeclarations.h"
#include <vtkDataSetMapper.h>
#include <vtkLookupTable.h>
#include <vtkAlgorithmOutput.h>
#include <vtkImageExtractComponents.h>
#include <vtkImageMapToColors.h>
#include <vtkImageAppendComponents.h>
#include <vtkImageChangeInformation.h>
#include <vtkExtractVOI.h>
#include "sscTypeConversions.h"
#include "cxGrabberReceiveThreadIGTLink.h"
#include "sscMessageManager.h"
#include "sscTime.h"
#include "sscVector3D.h"
#include "sscProbeData.h"
#include "sscToolManager.h"
#include "sscDataManager.h"
#include "cxProbe.h"
#include "cxVideoService.h"
#include "cxToolManager.h"
#include "cxImageSenderFactory.h"
#include "cxGrabberReceiveThreadDirectLink.h"
#include "sscTypeConversions.h"
#include "sscImage.h"
#include "sscData.h"
#include "sscLogger.h"
#include "sscVolumeHelpers.h"
#include "cxRenderTimer.h"
#include "cxBasicVideoSource.h"

typedef vtkSmartPointer<vtkDataSetMapper> vtkDataSetMapperPtr;
typedef vtkSmartPointer<vtkImageFlip> vtkImageFlipPtr;

namespace cx
{

VideoConnection::VideoConnection()
{
	mConnected = false;

	connect(ssc::toolManager(), SIGNAL(configured()),                 this, SLOT(connectVideoToProbe()));
	connect(ssc::toolManager(), SIGNAL(initialized()),                this, SLOT(connectVideoToProbe()));
	connect(ssc::toolManager(), SIGNAL(dominantToolChanged(QString)), this, SLOT(connectVideoToProbe()));
//	connect(this,               SIGNAL(videoSourcesChanged()),        this, SLOT(connectVideoToProbe()));
}

VideoConnection::~VideoConnection()
{
	this->stopClient();
}

void VideoConnection::fpsSlot(double fpsNumber)
{
	mFPS = fpsNumber;
	emit fps(fpsNumber);
}

//QString VideoConnection::getInfoString() const
//{
//	if (!mClient)
//		return "";
//	return mClient->hostDescription() + " - " + QString::number(mFPS, 'f', 1) + " fps";
//}

//QString VideoConnection::getStatusString() const
//{
//	if (!mClient)
//		return "Not connected";
//	if (mTimeout)
//		return "Timeout";
//	return "Running";
//}

bool VideoConnection::isConnected() const
{
	return mClient && mConnected;
}

void VideoConnection::connectedSlot(bool on)
{
	mConnected = on;

	if (on)
	{
		for (unsigned i=0; i<mSources.size(); ++i)
			mSources[i]->start();
	}
	else
	{
		this->disconnectServer();
	}

	emit connected(on);
}

void VideoConnection::directLink(std::map<QString, QString> args)
{
	this->runClient(GrabberReceiveThreadPtr(new GrabberDirectLinkThread(args, this)));
}

void VideoConnection::connectServer(QString address, int port)
{
	this->runClient(GrabberReceiveThreadPtr(new GrabberReceiveThreadIGTLink(address, port, this)));
}

void VideoConnection::runClient(GrabberReceiveThreadPtr client)
{
	if (mClient)
	{
		std::cout << "client already exist - returning" << std::endl;
		return;
	}
	mClient = client;
	connect(mClient.get(), SIGNAL(finished()), this, SLOT(clientFinishedSlot()));
	connect(mClient.get(), SIGNAL(imageReceived()), this, SLOT(imageReceivedSlot())); // thread-bridging connection
	connect(mClient.get(), SIGNAL(sonixStatusReceived()), this, SLOT(sonixStatusReceivedSlot())); // thread-bridging connection
	connect(mClient.get(), SIGNAL(fps(double)), this, SLOT(fpsSlot(double))); // thread-bridging connection
	//connect(mClient.get(), SIGNAL(connected(bool)), this, SIGNAL(connected(bool))); // thread-bridging connection
	connect(mClient.get(), SIGNAL(connected(bool)), this, SLOT(connectedSlot(bool)));

	mClient->start();
}

void VideoConnection::imageReceivedSlot()
{
	if (!mClient)
		return;
	this->updateImage(mClient->getLastImageMessage());
}

void VideoConnection::sonixStatusReceivedSlot()
{
	if (!mClient)
		return;
	this->updateSonixStatus(mClient->getLastSonixStatusMessage());
}

/**Get rid of the mClient thread.
 *
 */
void VideoConnection::stopClient()
{
	if (mClient)
	{
		mClient->quit();
		mClient->wait(2000); // forever or until dead thread

		if (mClient->isRunning())
		{
			mClient->terminate();
			mClient->wait(); // forever or until dead thread
			ssc::messageManager()->sendWarning(QString("Video Client [%1] did not quit normally - terminated.").arg(mClient->hostDescription()));
		}

		disconnect(mClient.get(), SIGNAL(finished()), this, SLOT(clientFinishedSlot()));
		disconnect(mClient.get(), SIGNAL(imageReceived()), this, SLOT(imageReceivedSlot())); // thread-bridging connection
		disconnect(mClient.get(), SIGNAL(sonixStatusReceived()), this, SLOT(sonixStatusReceivedSlot())); // thread-bridging connection
		disconnect(mClient.get(), SIGNAL(fps(double)), this, SLOT(fpsSlot(double))); // thread-bridging connection
		disconnect(mClient.get(), SIGNAL(connected(bool)), this, SLOT(connectedSlot(bool)));

		mClient.reset();
	}
}

void VideoConnection::disconnectServer()
{
	this->stopClient();

	for (unsigned i=0; i<mSources.size(); ++i)
	{
		mSources[i]->setInput(ssc::ImagePtr());
	}
}

void VideoConnection::clientFinishedSlot()
{
	if (!mClient)
		return;
//	if (mClient->isRunning()) // buggy: client might return running even if shutting down
//		return;
	this->disconnectServer();
}

/** extract information from the IGTLinkUSStatusMessage
 *  and store locally. Also reset the old local info with
 *  information from the probe in toolmanager.
 */
void VideoConnection::updateSonixStatus(ssc::ProbeData msg)
{
	ssc::ToolPtr tool = ToolManager::getInstance()->findFirstProbe();
	if (!tool)
		return;
	ssc::ProbePtr probe = tool->getProbe();
	if (!probe)
		return;

	// start with getting a valid data object from the probe, in order to keep
	// existing values (such as temporal calibration).
	// Note that the 'active' data is get while the 'uid' data is set.
	ssc::ProbeData data = probe->getData();
	std::cout << "VideoConnection::updateSonixStatus pre \n" << streamXml2String(data) << std::endl;

	data.setUid(msg.getUid());
	data.setType(msg.getType());
	data.setSector(msg.getDepthStart(), msg.getDepthEnd(), msg.getWidth());
	ssc::ProbeData::ProbeImageData image = data.getImage();
	image.mOrigin_p = msg.getImage().mOrigin_p;
	image.mSize = msg.getImage().mSize;
	image.mSpacing = msg.getImage().mSpacing;
	image.mClipRect_p = msg.getImage().mClipRect_p;
	data.setImage(image);

	std::cout << "VideoConnection::updateSonixStatus post\n" << streamXml2String(data) << std::endl;

	probe->setData(data);
}

void VideoConnection::updateImage(ssc::ImagePtr message)
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
//		std::cout << "*************** creating new VideoSourcev"<< source.get() <<" for " << message->getUid() << std::endl;
		mSources.push_back(source);
		source->start();
		newSource = true;
	}
	// set input.
	source->setInput(message);
//	std::cout << "  updating stream " << message->getUid() << std::endl;

	QString info = mClient->hostDescription() + " - " + QString::number(mFPS, 'f', 1) + " fps";
	source->setInfoString(info);

	if (newSource)
	{
		this->connectVideoToProbe();
		emit videoSourcesChanged();
	}
}

std::vector<ssc::VideoSourcePtr> VideoConnection::getVideoSources()
{
	std::vector<ssc::VideoSourcePtr> retval;
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
	SSC_LOG("");
	ssc::ToolPtr tool = ToolManager::getInstance()->findFirstProbe();
	if (!tool)
		return;
	ssc::ProbePtr probe = tool->getProbe();
	if (!probe)
		return;

	for (unsigned i=0; i<mSources.size(); ++i)
	{
//		std::cout << "***********============= set source in probe " << tool->getUid() << std::endl;
		probe->setRTSource(mSources[i]);
	}
}



}
