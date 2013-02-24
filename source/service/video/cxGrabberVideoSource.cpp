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
#include "cxGrabberVideoSource.h"

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

typedef vtkSmartPointer<vtkDataSetMapper> vtkDataSetMapperPtr;
typedef vtkSmartPointer<vtkImageFlip> vtkImageFlipPtr;

namespace cx
{

GrabberVideoSource::GrabberVideoSource()
{
	mConnected = false;
	mRedirecter = vtkSmartPointer<vtkImageChangeInformation>::New(); // used for forwarding only.

	vtkImageDataPtr emptyImage = ssc::generateVtkImageData(Eigen::Array3i(1,1,1),
	                                                       ssc::Vector3D(1,1,1),
	                                                       0);
	mEmptyImage.reset(new ssc::Image("<none>", emptyImage));
	mReceivedImage = mEmptyImage;
	mRedirecter->SetInput(mEmptyImage->getBaseVtkImageData());

	mTimeout = true; // must start invalid
	mTimeoutTimer = new QTimer(this);
	mTimeoutTimer->setInterval(1000);
	connect(mTimeoutTimer, SIGNAL(timeout()), this, SLOT(timeout()));
	connect(this, SIGNAL(connected(bool)), this, SIGNAL(streaming(bool))); // define connected as streaming.
}

GrabberVideoSource::~GrabberVideoSource()
{
	this->stopClient();
}

void GrabberVideoSource::timeout()
{
	if (mTimeout)
		return;

	ssc::messageManager()->sendWarning("Timeout!");
	mTimeout = true;
	emit newFrame();
}

QString GrabberVideoSource::getName()
{
	return mReceivedImage->getUid();
}

void GrabberVideoSource::fpsSlot(double fpsNumber)
{
	mFPS = fpsNumber;
	emit fps(fpsNumber);
}

QString GrabberVideoSource::getInfoString() const
{
	if (!mClient)
		return "";
	return mClient->hostDescription() + " - " + QString::number(mFPS, 'f', 1) + " fps";
}

QString GrabberVideoSource::getStatusString() const
{
	if (!mClient)
		return "Not connected";
	if (mTimeout)
		return "Timeout";
	return "Running";
}

void GrabberVideoSource::start()
{

}

void GrabberVideoSource::stop()
{

}

bool GrabberVideoSource::validData() const
{
	return mClient && !mTimeout;
}

double GrabberVideoSource::getTimestamp()
{
	return mReceivedImage->getAcquisitionTime().toMSecsSinceEpoch();
}

bool GrabberVideoSource::isConnected() const
{
	return mClient && mConnected;
}

bool GrabberVideoSource::isStreaming() const
{
	return this->isConnected();
}

void GrabberVideoSource::connectedSlot(bool on)
{
	mConnected = on;

	if (!on)
		this->disconnectServer();

	emit connected(on);
}

void GrabberVideoSource::directLink(std::map<QString, QString> args)
{
	this->runClient(GrabberReceiveThreadPtr(new GrabberDirectLinkThread(args, this)));
}


void GrabberVideoSource::connectServer(QString address, int port)
{
	this->runClient(GrabberReceiveThreadPtr(new GrabberReceiveThreadIGTLink(address, port, this)));
}


void GrabberVideoSource::runClient(GrabberReceiveThreadPtr client)
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
	mTimeoutTimer->start();
}

void GrabberVideoSource::imageReceivedSlot()
{
	if (!mClient)
		return;
	this->updateImage(mClient->getLastImageMessage());
}

void GrabberVideoSource::sonixStatusReceivedSlot()
{
	if (!mClient)
		return;
	this->updateSonixStatus(mClient->getLastSonixStatusMessage());
}

/**Get rid of the mClient thread.
 *
 */
void GrabberVideoSource::stopClient()
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

void GrabberVideoSource::disconnectServer()
{
	this->stopClient();

	mTimeoutTimer->stop();

	// clear the redirecter
	mReceivedImage = mEmptyImage;
	mRedirecter->SetInput(mEmptyImage->getBaseVtkImageData());
	emit newFrame(); // changed
}

void GrabberVideoSource::clientFinishedSlot()
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
void GrabberVideoSource::updateSonixStatus(ssc::ProbeData msg)
{
	ssc::ToolPtr tool = ToolManager::getInstance()->findFirstProbe();
	if (!tool)
		return;
	ssc::ProbePtr probe = tool->getProbe();
	if (!probe)
		return;

	// start with getting a valid data object from the probe, in order to keep
	// existing values (such as temporal calibration).
	ssc::ProbeData data = probe->getData();

	data.setType(msg.getType());
	data.setSector(msg.getDepthStart(), msg.getDepthEnd(), msg.getWidth());
	ssc::ProbeData::ProbeImageData image = data.getImage();
	image.mOrigin_p = msg.getImage().mOrigin_p;
	image.mSize = msg.getImage().mSize;
	image.mSpacing = msg.getImage().mSpacing;
	image.mClipRect_p = msg.getImage().mClipRect_p;
	data.setImage(image);

	probe->setData(data);
}

void GrabberVideoSource::updateImage(ssc::ImagePtr message)
{
	static CyclicActionTimer timer("Update Video Image");
	timer.begin();

	if (!message)
	{
		std::cout << "got empty image !!!" << std::endl;
		mReceivedImage = mEmptyImage;
		mRedirecter->SetInput(mEmptyImage->getBaseVtkImageData());
		return;
	}

	mReceivedImage = message;
	mRedirecter->SetInput(mReceivedImage->getBaseVtkImageData());

	mTimeout = false;
	mTimeoutTimer->start();

	//	std::cout << "emit newframe:\t" << QDateTime::currentDateTime().toString("hh:mm:ss.zzz").toStdString() << std::endl;
	emit newFrame();
	timer.time("emit");

	if (timer.intervalPassed())
	{
		static int counter=0;
//		if (++counter%10==0)
//			ssc::messageManager()->sendDebug(timer.dumpStatisticsSmall());
		timer.reset();
	}

}

vtkImageDataPtr GrabberVideoSource::getVtkImageData()
{
	return mRedirecter->GetOutput();
}

}
