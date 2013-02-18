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
#include "cxOpenIGTLinkRTSource.h"

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
#include "cxOpenIGTLinkClient.h"
#include "sscMessageManager.h"
#include "sscTime.h"
#include "sscVector3D.h"
#include "sscProbeData.h"
#include "sscToolManager.h"
#include "sscDataManager.h"
#include "cxTool.h"
#include "cxProbe.h"
#include "cxVideoService.h"
#include "cxToolManager.h"
#include "cxImageSenderFactory.h"
#include "cxGrabberDirectLinkThread.h"
#include "sscTypeConversions.h"
#include "sscImage.h"
#include "sscData.h"
#include "sscLogger.h"

typedef vtkSmartPointer<vtkDataSetMapper> vtkDataSetMapperPtr;
typedef vtkSmartPointer<vtkImageFlip> vtkImageFlipPtr;

namespace cx
{

OpenIGTLinkRTSource::OpenIGTLinkRTSource() :
				mImageImport(vtkImageImportPtr::New()),
				updateSonixParameters(false)
{
	mLastTimestamp = 0;
	mConnected = false;
	mRedirecter = vtkSmartPointer<vtkImageChangeInformation>::New(); // used for forwarding only.

	mRedirecter->SetInput(mImageImport->GetOutput());

	mImageImport->SetNumberOfScalarComponents(1);
	this->setEmptyImage();
//  this->setTestImage();

	mTimeout = true; // must start invalid
	mTimeoutTimer = new QTimer(this);
	mTimeoutTimer->setInterval(1000);
	connect(mTimeoutTimer, SIGNAL(timeout()), this, SLOT(timeout()));
	connect(this, SIGNAL(connected(bool)), this, SIGNAL(streaming(bool))); // define connected as streaming.
}

OpenIGTLinkRTSource::~OpenIGTLinkRTSource()
{
	this->stopClient();
}

void OpenIGTLinkRTSource::timeout()
{
	if (mTimeout)
		return;

	ssc::messageManager()->sendWarning("Timeout!");
	mTimeout = true;
	emit newFrame();
}

QString OpenIGTLinkRTSource::getName()
{
	if (mDeviceName.isEmpty())
		return "IGTLink";
	return mDeviceName;
}

void OpenIGTLinkRTSource::fpsSlot(double fpsNumber)
{
	mFPS = fpsNumber;
	emit fps(fpsNumber);
}

QString OpenIGTLinkRTSource::getInfoString() const
{
	if (!mClient)
		return "";
	return mClient->hostDescription() + " - " + QString::number(mFPS, 'f', 1) + " fps";
}

QString OpenIGTLinkRTSource::getStatusString() const
{
	if (!mClient)
		return "Not connected";
	if (mTimeout)
		return "Timeout";
	return "Running";
}

void OpenIGTLinkRTSource::start()
{

}

//void OpenIGTLinkRTSource::pause()
//{
//
//}

void OpenIGTLinkRTSource::stop()
{

}

bool OpenIGTLinkRTSource::validData() const
{
	return mClient && !mTimeout;
}

double OpenIGTLinkRTSource::getTimestamp()
{
	//oldHACK we need time sync before we can use the real timetags delivered with the image
	//return ssc::getMilliSecondsSinceEpoch();

	return mLastTimestamp;
}

bool OpenIGTLinkRTSource::isConnected() const
{
	return mClient && mConnected;
}

bool OpenIGTLinkRTSource::isStreaming() const
{
	return this->isConnected();
}

void OpenIGTLinkRTSource::connectedSlot(bool on)
{
	mConnected = on;

	if (!on)
		this->disconnectServer();

	emit connected(on);
}

void OpenIGTLinkRTSource::directLink(std::map<QString, QString> args)
{
	this->runClient(IGTLinkClientBasePtr(new GrabberDirectLinkThread(args, this)));
}


void OpenIGTLinkRTSource::connectServer(QString address, int port)
{
	this->runClient(IGTLinkClientBasePtr(new IGTLinkClient(address, port, this)));
}


void OpenIGTLinkRTSource::runClient(IGTLinkClientBasePtr client)
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

void OpenIGTLinkRTSource::imageReceivedSlot()
{
	if (!mClient)
		return;
	this->updateImage(mClient->getLastImageMessage());
}

void OpenIGTLinkRTSource::sonixStatusReceivedSlot()
{
	if (!mClient)
		return;
	this->updateSonixStatus(mClient->getLastSonixStatusMessage());
}

/**Get rid of the mClient thread.
 *
 */
void OpenIGTLinkRTSource::stopClient()
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


void OpenIGTLinkRTSource::disconnectServer()
{
	this->stopClient();

	mTimeoutTimer->stop();

	// clear the redirecter
	mRedirecter->SetInput(mImageImport->GetOutput());
	mFilter_IGTLink_to_RGB = vtkImageDataPtr();

	emit newFrame(); // changed
}

void OpenIGTLinkRTSource::clientFinishedSlot()
{
//	SSC_LOG("*************************");
	if (!mClient)
		return;
//	if (mClient->isRunning()) // buggy: client might return running even if shutting down
//		return;
	this->disconnectServer();
}

/** chrash-avoiding measure -  for startup
 */
void OpenIGTLinkRTSource::setEmptyImage()
{
	mImageMessage = IGTLinkImageMessage::Pointer();
	mImageImport->SetWholeExtent(0, 1, 0, 1, 0, 0);
	mImageImport->SetDataExtent(0, 1, 0, 1, 0, 0);
	mImageImport->SetDataScalarTypeToUnsignedChar();
	std::fill(mZero.begin(), mZero.end(), 0);
	mImageImport->SetImportVoidPointer(mZero.begin());
	mImageImport->Modified();
}

void OpenIGTLinkRTSource::setTestImage()
{
	int W = 512;
	int H = 512;

	int numberOfComponents = 4;
	mImageMessage = IGTLinkImageMessage::Pointer();
	mImageImport->SetWholeExtent(0, W - 1, 0, H - 1, 0, 0);
	mImageImport->SetDataExtent(0, W - 1, 0, H - 1, 0, 0);
	mImageImport->SetDataScalarTypeToUnsignedChar();
	mImageImport->SetNumberOfScalarComponents(numberOfComponents);
	mTestData.resize(W * H * numberOfComponents);
	std::fill(mTestData.begin(), mTestData.end(), 50);
	std::vector<unsigned char>::iterator current;

	for (int y = 0; y < H; ++y)
		for (int x = 0; x < W; ++x)
		{
			current = mTestData.begin() + int((x + W * y) * numberOfComponents);
			current[0] = 255;
			current[1] = 0;
			current[2] = x / 2;
			current[3] = 0;
//      mTestData[x+W*y] = x/2;
		}

	mImageImport->SetImportVoidPointer(&(*mTestData.begin()));
	mImageImport->Modified();
}

void OpenIGTLinkRTSource::updateImageImportFromIGTMessage(IGTLinkImageMessage::Pointer message)
{
	mImageMessage = message;
	// Retrive the image data
	float spacing[3]; // spacing (mm/pixel)
	int svsize[3]; // sub-volume size
	int svoffset[3]; // sub-volume offset
	int scalarType; // scalar type
	int size[3]; // image dimension

	// Note: subvolumes is not supported. Implement when needed.

	scalarType = message->GetScalarType();
	message->GetDimensions(size);
	message->GetSpacing(spacing);
	message->GetSubVolume(svsize, svoffset);
//	message->GetOrigin(origin);
	mDeviceName = message->GetDeviceName();
//  std::cout << "size : " << ssc::Vector3D(size[0], size[1], size[2]) << std::endl;

	//for linear probes used in other substance than the scanner is calibrated for we want to compensate
	//for the change in sound of speed in that substance, do this by changing spacing in the images y-direction,
	//this is only valid for linear probes
//	spacing[1] *= mLinearSoundSpeedCompesation;


	mImageImport->SetNumberOfScalarComponents(1);

	switch (scalarType)
	{
	case IGTLinkImageMessage::TYPE_INT8:
		std::cout << "signed char is not supported. Falling back to unsigned char." << std::endl;
		mImageImport->SetDataScalarTypeToUnsignedChar();
		break;
	case IGTLinkImageMessage::TYPE_UINT8:
		mImageImport->SetDataScalarTypeToUnsignedChar();
		break;
	case IGTLinkImageMessage::TYPE_INT16:
		mImageImport->SetDataScalarTypeToShort();
		break;
	case IGTLinkImageMessage::TYPE_UINT16:
//    std::cout << "SetDataScalarTypeToUnsignedShort." << std::endl;
//		mImageImport->SetDataScalarTypeToUnsignedShort();
    mImageImport->SetNumberOfScalarComponents(2);
    mImageImport->SetDataScalarTypeToUnsignedChar();
		break;
	case IGTLinkImageMessage::TYPE_INT32:
	case IGTLinkImageMessage::TYPE_UINT32:
//    std::cout << "SetDataScalarTypeTo4channel." << std::endl;
		// assume RGBA unsigned colors
		mImageImport->SetNumberOfScalarComponents(4);
//    mImageImport->SetDataScalarTypeToInt();
		mImageImport->SetDataScalarTypeToUnsignedChar();
//    std::cout << "32bit received" << std::endl;
		break;
	case IGTLinkImageMessage::TYPE_FLOAT32:
		mImageImport->SetDataScalarTypeToFloat();
		break;
	case IGTLinkImageMessage::TYPE_FLOAT64:
		mImageImport->SetDataScalarTypeToDouble();
		break;
	default:
		std::cout << "unknown type. Falling back to unsigned char." << std::endl;
		mImageImport->SetDataScalarTypeToUnsignedChar();
	}

	// get timestamp from igtl second-format:
	igtl::TimeStamp::Pointer timestamp = igtl::TimeStamp::New();
	mImageMessage->GetTimeStamp(timestamp);
//  static double last = 0;
//  if (last==0)
//    last = timestamp->GetTimeStamp();
//  std::cout << "raw time" << timestamp->GetTimeStamp() << ", " << timestamp->GetTimeStamp() - last << std::endl;

	mLastTimestamp = timestamp->GetTimeStamp() * 1000;
//	mLastTimestamp += mTimestampCalibration;

//	mDebug_orgTime = timestamp->GetTimeStamp() * 1000; // ms
	mImageImport->SetDataOrigin(0, 0, 0);
	mImageImport->SetDataSpacing(spacing[0], spacing[1], spacing[2]);
	mImageImport->SetWholeExtent(0, size[0] - 1, 0, size[1] - 1, 0, size[2] - 1);
	mImageImport->SetDataExtentToWholeExtent();
	mImageImport->SetImportVoidPointer(mImageMessage->GetScalarPointer());

	mImageImport->Modified();

	// Update the parts of the probe data that must be read from the image.
	ssc::ProbeData::ProbeImageData imageData = mSonixProbeData.getImage();
	imageData.mSpacing = ssc::Vector3D(spacing[0], spacing[1], spacing[2]);
	imageData.mSize = QSize(size[0], size[1]);
	imageData.mClipRect_p = ssc::DoubleBoundingBox3D(0, imageData.mSize.width(), 0, imageData.mSize.height(), 0, 0);
	mSonixProbeData.setImage(imageData);
//	std::cout << "Received Sonix message:\n" << streamXml2String(mSonixProbeData) << std::cout;

	//Only do the following for the digital Ultrasonix interface
	if (updateSonixParameters)
	{
		//TODO: Send all necessary parameters (origin + size) in IGTLinkUSStatusMessage and only update from there.
		this->updateSonix();
	}
}

/**extract information from the IGTLinkUSStatusMessage
 * and store locally. Also reset the old local info with
 * information from the probe in toolmanager.
 *
 */
void OpenIGTLinkRTSource::updateSonixStatus(IGTLinkUSStatusMessage::Pointer message)
{
	// start with getting a valid data object from the probe, in order to keep
	// existing values (such as temporal calibration).
	ProbePtr probe = this->getValidProbe();
	if (probe)
		mSonixProbeData = probe->getData();

	mSonixProbeData.setType(ssc::ProbeData::TYPE(message->GetProbeType()));
	mSonixProbeData.setSector(
		message->GetDepthStart(),
		message->GetDepthEnd(),
		message->GetWidth(),
		0);
	ssc::ProbeData::ProbeImageData imageData = mSonixProbeData.getImage();
	imageData.mOrigin_p = ssc::Vector3D(message->GetOrigin());
	mSonixProbeData.setImage(imageData);

	//std::cout << "Received Sonix message:\n" << streamXml2String(mSonixProbeData) << std::cout;

//	sonixVideo = true; // Temporary hack to turn off ARGB_RGBA for sonix

//	std::cout << "depthStart: " << message->GetDepthStart() << std::endl;
//	std::cout << "depthEnd: " << message->GetDepthEnd() << std::endl;
//	std::cout << "width: " << message->GetWidth() << std::endl;

	updateSonixParameters = true;
}

ProbePtr OpenIGTLinkRTSource::getValidProbe()
{
	ssc::ToolPtr tool = ToolManager::getInstance()->findFirstProbe();
	if (!tool)
		return ProbePtr();
	ProbePtr probe = boost::shared_dynamic_cast<Probe>(tool->getProbe());
	if (!probe)
	{
		ssc::messageManager()->sendWarning("OpenIGTLinkRTSource::updateSonixStatus: Found no Probe");
		return ProbePtr();
	}

	return probe;
}

/**Update the probe sector parameters, based on the accumulated information
 * from incoming IGTLink messages.
 */
void OpenIGTLinkRTSource::updateSonix()
{
	ProbePtr probe = this->getValidProbe();
	if (!probe)
		return;

	//std::cout << "Ready to emit Sonix message:\n" << streamXml2String(mSonixProbeData) << std::cout;

	probe->setData(mSonixProbeData);
	updateSonixParameters = false;
}

void OpenIGTLinkRTSource::updateImage(IGTLinkImageMessage::Pointer message)
{
	static CyclicActionTimer timer("Update Video Image");
	timer.begin();
#if 1 // remove to use test image
	if (!message)
	{
		std::cout << "got empty image !!!" << std::endl;
		this->setEmptyImage();
		return;
	}

	this->updateImageImportFromIGTMessage(message);
	mImageImport->GetOutput()->Update();
#endif

	mTimeout = false;
	mTimeoutTimer->start();

	// this seems to add 3ms per update()
	// insert a ARGB->RBGA filter. TODO: need to check the input more thoroughly here, this applies only to the internal CustusX US pipeline.
	if (mImageImport->GetOutput()->GetNumberOfScalarComponents() == 4 && !mFilter_IGTLink_to_RGB)
	{
		// the cx sonix server sends BGRX
		if (QString(message->GetDeviceName()) == "ImageSenderSonix")
		{
			mFilter_IGTLink_to_RGB = this->createFilterBGR2RGB(mImageImport->GetOutput());
		}
		// the cx mac QT grabber server sends ARGB,
		// the cx opencv server also sends ARGB, in order to mimic the mac server.
		else if (QString(message->GetDeviceName()) == "cxOpenCVGrabber" || QString(message->GetDeviceName()) == "GrabberServer")
		{
			mFilter_IGTLink_to_RGB = this->createFilterARGB2RGB(mImageImport->GetOutput());
		}
		else // default: strip alpha channel (should not happen, but cx expects RGB or Gray, not alpha)
		{
			mFilter_IGTLink_to_RGB = this->createFilterRGBA2RGB(mImageImport->GetOutput());
		}

		if (mFilter_IGTLink_to_RGB)
			mRedirecter->SetInput(mFilter_IGTLink_to_RGB);
	}
	timer.time("convert");

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


/**Create a pipeline that convert the input 4-component ARGB image (from QuickTime-Mac)
 * into a vtk-style RGBA image.
 *
 */
vtkImageDataPtr OpenIGTLinkRTSource::createFilterARGB2RGB(vtkImageDataPtr input)
{
	vtkImageAppendComponentsPtr merger = vtkImageAppendComponentsPtr::New();

	/// extract the RGB part of input (1,2,3) and insert as (0,1,2) in output
	vtkImageExtractComponentsPtr splitterRGB = vtkImageExtractComponentsPtr::New();
	splitterRGB->SetInput(input);
	splitterRGB->SetComponents(1, 2, 3);
	merger->SetInput(0, splitterRGB->GetOutput());

// Removed adding of Alpha channel: this is always 1 anyway (cross fingers)
//  /// extract the A part of input (0) and insert as (3) in output
//  vtkImageExtractComponentsPtr splitterA = vtkImageExtractComponentsPtr::New();
//  splitterA->SetInput(input);
//  splitterA->SetComponents(0);
//  merger->SetInput(1, splitterA->GetOutput());

	return merger->GetOutput();
}

/**Filter that converts from BGR to RGB encoding.
 *
 */
vtkImageDataPtr OpenIGTLinkRTSource::createFilterBGR2RGB(vtkImageDataPtr input)
{
  vtkImageAppendComponentsPtr merger = vtkImageAppendComponentsPtr::New();

  vtkImageExtractComponentsPtr splitterRGB = vtkImageExtractComponentsPtr::New();
  splitterRGB->SetInput(input);
  splitterRGB->SetComponents(2, 1, 0);//hack convert from BGRA to RGB
  merger->SetInput(0, splitterRGB->GetOutput());

  return merger->GetOutput();
}

/**Filter that converts from RGBA to RGB encoding.
 *
 */
vtkImageDataPtr OpenIGTLinkRTSource::createFilterRGBA2RGB(vtkImageDataPtr input)
{
  vtkImageAppendComponentsPtr merger = vtkImageAppendComponentsPtr::New();

  vtkImageExtractComponentsPtr splitterRGB = vtkImageExtractComponentsPtr::New();
  splitterRGB->SetInput(input);
  splitterRGB->SetComponents(0, 1, 2);
  merger->SetInput(0, splitterRGB->GetOutput());

  return merger->GetOutput();
}

vtkImageDataPtr OpenIGTLinkRTSource::getVtkImageData()
{
	return mRedirecter->GetOutput();
}

}
