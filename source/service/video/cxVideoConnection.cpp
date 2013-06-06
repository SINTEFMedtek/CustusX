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
#include "cxIGTLinkedImageReceiverThread.h"
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
#include "cxDirectlyLinkedImageReceiverThread.h"
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
	mUnsusedProbeDataVector.clear();

	connect(ssc::toolManager(), SIGNAL(configured()),                 this, SLOT(connectVideoToProbe()));
	connect(ssc::toolManager(), SIGNAL(initialized()),                this, SLOT(connectVideoToProbe()));
	connect(ssc::toolManager(), SIGNAL(dominantToolChanged(QString)), this, SLOT(connectVideoToProbe()));
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

	emit connected(on);
}

void VideoConnection::runDirectLinkClient(std::map<QString, QString> args)
{
	this->runClient(ImageReceiverThreadPtr(new DirectlyLinkedImageReceiverThread(args, this)));
}

void VideoConnection::runIGTLinkedClient(QString address, int port)
{
	this->runClient(ImageReceiverThreadPtr(new IGTLinkedImageReceiverThread(address, port, this)));
}

void VideoConnection::runClient(ImageReceiverThreadPtr client)
{
	if (mClient)
	{
		std::cout << "client already exist - returning" << std::endl;
		return;
	}
	mClient = client;
	connect(mClient.get(), SIGNAL(finished()), this, SLOT(clientFinishedSlot()));
	connect(mClient.get(), SIGNAL(imageReceived()), this, SLOT(imageReceivedSlot())); // thread-bridging connection
	connect(mClient.get(), SIGNAL(sonixStatusReceived()), this, SLOT(statusReceivedSlot())); // thread-bridging connection
	connect(mClient.get(), SIGNAL(fps(double)), this, SLOT(fpsSlot(double))); // thread-bridging connection
	connect(mClient.get(), SIGNAL(connected(bool)), this, SLOT(connectedSlot(bool)));

	mClient->start();
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
		disconnect(mClient.get(), SIGNAL(sonixStatusReceived()), this, SLOT(statusReceivedSlot())); // thread-bridging connection
		disconnect(mClient.get(), SIGNAL(fps(double)), this, SLOT(fpsSlot(double))); // thread-bridging connection
		disconnect(mClient.get(), SIGNAL(connected(bool)), this, SLOT(connectedSlot(bool)));

		mClient.reset();
	}
}

void VideoConnection::disconnectServer()
{
	this->stopClient();

	for (unsigned i=0; i<mSources.size(); ++i)
		mSources[i]->setInput(ssc::ImagePtr());

	ssc::ToolPtr tool = ToolManager::getInstance()->findFirstProbe();
	if (tool && tool->getProbe())
		this->removeSourceFromProbe(tool);

	mSources.clear();
	emit videoSourcesChanged();
}

void VideoConnection::clientFinishedSlot()
{
	if (!mClient)
		return;
//	if (mClient->isRunning()) // buggy: client might return running even if shutting down
//		return;
	this->disconnectServer();
}

void VideoConnection::useUnusedProbeDataSlot()
{
	disconnect(ToolManager::getInstance(), SIGNAL(probeAvailable()), this, SLOT(useUnusedProbeDataSlot()));
	for (std::vector<ssc::ProbeDataPtr>::const_iterator citer = mUnsusedProbeDataVector.begin(); citer != mUnsusedProbeDataVector.end(); ++citer)
		this->updateStatus(*citer);
	mUnsusedProbeDataVector.clear();
}

/** extract information from the IGTLinkUSStatusMessage
 *  and store locally. Also reset the old local info with
 *  information from the probe in toolmanager.
 */
void VideoConnection::updateStatus(ssc::ProbeDataPtr msg)
{
	ssc::ToolPtr tool = ToolManager::getInstance()->findFirstProbe();
	if (!tool || !tool->getProbe())
	{
		//Don't throw away the ProbeData. Save it until it can be used
		if (mUnsusedProbeDataVector.empty())
			connect(ToolManager::getInstance(), SIGNAL(probeAvailable()), this, SLOT(useUnusedProbeDataSlot()));
		mUnsusedProbeDataVector.push_back(msg);
		return;
	}
	ProbePtr probe = boost::dynamic_pointer_cast<Probe>(tool->getProbe());

	// start with getting a valid data object from the probe, in order to keep
	// existing values (such as temporal calibration).
	// Note that the 'active' data is get while the 'uid' data is set.
	ssc::ProbeData data = probe->getProbeData();

	data.setUid(msg->getUid());
	data.setType(msg->getType());
	data.setSector(msg->getDepthStart(), msg->getDepthEnd(), msg->getWidth());
	ssc::ProbeData::ProbeImageData image = data.getImage();
	image.mOrigin_p = msg->getImage().mOrigin_p;
	image.mSize = msg->getImage().mSize;
	image.mSpacing = msg->getImage().mSpacing;
	image.mClipRect_p = msg->getImage().mClipRect_p;
	data.setImage(image);

	probe->useDigitalVideo(true);
	probe->setProbeSector(data);
	probe->setActiveStream(msg->getUid());
}

void VideoConnection::startAllSources()
{
	for (unsigned i=0; i<mSources.size(); ++i)
		mSources[i]->start();
}

void VideoConnection::removeSourceFromProbe(ssc::ToolPtr tool)
{
	ssc::ProbePtr probe = tool->getProbe();
	for (unsigned i=0; i<mSources.size(); ++i)
		probe->removeRTSource(mSources[i]);
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
	ssc::ToolPtr tool = ToolManager::getInstance()->findFirstProbe();
	if (!tool)
		return;

	ssc::ProbePtr probe = tool->getProbe();
	if (!probe)
		return;

	for (unsigned i=0; i<mSources.size(); ++i)
		probe->setRTSource(mSources[i]);
}

}
