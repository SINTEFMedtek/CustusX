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

typedef vtkSmartPointer<vtkDataSetMapper> vtkDataSetMapperPtr;
typedef vtkSmartPointer<vtkImageFlip> vtkImageFlipPtr;

namespace cx
{

OpenIGTLinkRTSource::OpenIGTLinkRTSource() :
				mImageImport(vtkImageImportPtr::New()), mLinearSoundSpeedCompesation(1.0), updateSonixParameters(false), mDepthStart(
								0), mDepthEnd(0), mWidth(0)
{
	mLastTimestamp = 0;
	mTimestampCalibration = 0;
	mConnected = false;
	mRedirecter = vtkSmartPointer<vtkImageChangeInformation>::New(); // used for forwarding only.

	mSize[0] = 0;
	mSize[1] = 0;
	mSize[2] = 0;
	mOrigin[0] = 0.0;
	mOrigin[1] = 0.0;
	mOrigin[2] = 0.0;
	mSpacing[0] = 0.0;
	mSpacing[1] = 0.0;
	mSpacing[2] = 0.0;

	//image flip
//  vtkImageFlipPtr flipper = vtkImageFlipPtr::New();
//  flipper->SetFilteredAxes(0); //flipp around Y axis
//  flipper->SetInput(mImageImport->GetOutput());
//  mRedirecter->SetInput(flipper->GetOutput());
	mRedirecter->SetInput(mImageImport->GetOutput());

	mImageImport->SetNumberOfScalarComponents(1);
	this->setEmptyImage();
//  this->setTestImage();

	mTimeout = true; // must start invalid
	mTimeoutTimer = new QTimer(this);
	mTimeoutTimer->setInterval(1000);
	connect(mTimeoutTimer, SIGNAL(timeout()), this, SLOT(timeout()));
	connect(this, SIGNAL(connected(bool)), this, SIGNAL(streaming(bool))); // define connected as streaming.
	//connect(this, SIGNAL(connected(bool)), this, SLOT(connectedSlot(bool))); // define connected as streaming.
}

OpenIGTLinkRTSource::~OpenIGTLinkRTSource()
{
	//disconnect();
	if (mClient)
	{
		mClient->quit();
		mClient->wait(2000);
		if (mClient->isRunning())
		{
			mClient->terminate();
			mClient->wait(); // forever or until dead thread
		}
	}
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

/**Set a time shift that is added to every timestamp acquired from the source.
 * This can be used to calibrate time shifts between source and client.
 */
void OpenIGTLinkRTSource::setTimestampCalibration(double delta)
{
	if (ssc::similar(mTimestampCalibration, delta))
		return;
	if (!ssc::similar(delta, 0.0))
		ssc::messageManager()->sendInfo("set time calibration in rt source: " + qstring_cast(delta) + "ms");
	mTimestampCalibration = delta;
}

void OpenIGTLinkRTSource::setSoundSpeedCompensation(double gamma)
{
	mLinearSoundSpeedCompesation = gamma;
	ssc::messageManager()->sendInfo(
					"Linear sound speed compensation set to: "
					+ qstring_cast(mLinearSoundSpeedCompesation));
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

void OpenIGTLinkRTSource::connectServer(QString address, int port)
{
	if (mClient)
	{
		std::cout << "no client - returning" << std::endl;
		return;
	}
//  std::cout << "OpenIGTLinkRTSource::connect to server" << std::endl;
	mClient.reset(new IGTLinkClient(address, port, this));
	connect(mClient.get(), SIGNAL(finished()), this, SLOT(clientFinishedSlot()));
	connect(mClient.get(), SIGNAL(imageReceived()), this, SLOT(imageReceivedSlot())); // thread-bridging connection
	connect(mClient.get(), SIGNAL(sonixStatusReceived()), this, SLOT(sonixStatusReceivedSlot())); // thread-bridging connection
	connect(mClient.get(), SIGNAL(fps(double)), this, SLOT(fpsSlot(double))); // thread-bridging connection
	//connect(mClient.get(), SIGNAL(connected(bool)), this, SIGNAL(connected(bool))); // thread-bridging connection
	connect(mClient.get(), SIGNAL(connected(bool)), this, SLOT(connectedSlot(bool)));

	mClient->start();
	mTimeoutTimer->start();

//  emit changed();
//  emit serverStatusChanged();
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

void OpenIGTLinkRTSource::disconnectServer()
{
//  std::cout << "IGTLinkWidget::disconnect server" << std::endl;
	if (mClient)
	{
		mClient->quit();
		mClient->wait(2000); // forever or until dead thread

		disconnect(mClient.get(), SIGNAL(finished()), this, SLOT(clientFinishedSlot()));
		disconnect(mClient.get(), SIGNAL(imageReceived()), this, SLOT(imageReceivedSlot())); // thread-bridging connection
		disconnect(mClient.get(), SIGNAL(sonixStatusReceived()), this, SLOT(sonixStatusReceivedSlot())); // thread-bridging connection
		disconnect(mClient.get(), SIGNAL(fps(double)), this, SLOT(fpsSlot(double))); // thread-bridging connection
		//disconnect(mClient.get(), SIGNAL(connected(bool)), this, SIGNAL(connected(bool))); // thread-bridging connection
		disconnect(mClient.get(), SIGNAL(connected(bool)), this, SLOT(connectedSlot(bool)));
		mClient.reset();
	}

	mTimeoutTimer->stop();
	emit newFrame(); // changed

//  emit changed();
//  emit serverStatusChanged();
}

void OpenIGTLinkRTSource::clientFinishedSlot()
{
//  std::cout << "IGTLinkWidget::clientFinishedSlot" << std::endl;
	if (!mClient)
		return;
	if (mClient->isRunning())
		return;
	this->disconnectServer();
}

/** chrash-avoiding measure -  for startup
 */
void OpenIGTLinkRTSource::setEmptyImage()
{
	mImageMessage = igtl::ImageMessage::Pointer();
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
	mImageMessage = igtl::ImageMessage::Pointer();
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

void OpenIGTLinkRTSource::updateImageImportFromIGTMessage(igtl::ImageMessage::Pointer message)
{
	mImageMessage = message;
	// Retrive the image data
	float spacing[3]; // spacing (mm/pixel)
	int svsize[3]; // sub-volume size
	int svoffset[3]; // sub-volume offset
	int scalarType; // scalar type

	// Note: subvolumes is not supported. Implement when needed.

	scalarType = message->GetScalarType();
	message->GetDimensions(mSize);
	message->GetSpacing(spacing);
	message->GetSubVolume(svsize, svoffset);
	message->GetOrigin(mOrigin);
	mDeviceName = message->GetDeviceName();
//  std::cout << "size : " << ssc::Vector3D(size[0], size[1], size[2]) << std::endl;

	mImageImport->SetNumberOfScalarComponents(1);

	switch (scalarType)
	{
	case igtl::ImageMessage::TYPE_INT8:
		std::cout << "signed char is not supported. Falling back to unsigned char." << std::endl;
		mImageImport->SetDataScalarTypeToUnsignedChar();
		break;
	case igtl::ImageMessage::TYPE_UINT8:
		mImageImport->SetDataScalarTypeToUnsignedChar();
		break;
	case igtl::ImageMessage::TYPE_INT16:
		mImageImport->SetDataScalarTypeToShort();
		break;
	case igtl::ImageMessage::TYPE_UINT16:
//    std::cout << "SetDataScalarTypeToUnsignedShort." << std::endl;
		mImageImport->SetDataScalarTypeToUnsignedShort();
		break;
	case igtl::ImageMessage::TYPE_INT32:
	case igtl::ImageMessage::TYPE_UINT32:
//    std::cout << "SetDataScalarTypeTo4channel." << std::endl;
		// assume RGBA unsigned colors
		mImageImport->SetNumberOfScalarComponents(4);
//    mImageImport->SetDataScalarTypeToInt();
		mImageImport->SetDataScalarTypeToUnsignedChar();
//    std::cout << "32bit received" << std::endl;
		break;
	case igtl::ImageMessage::TYPE_FLOAT32:
		mImageImport->SetDataScalarTypeToFloat();
		break;
	case igtl::ImageMessage::TYPE_FLOAT64:
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
	mLastTimestamp += mTimestampCalibration;

	mDebug_orgTime = timestamp->GetTimeStamp() * 1000; // ms
//  double now = (double)QDateTime::currentDateTime().toMSecsSinceEpoch();
//  std::cout << QString("cv+cx delay: %1").arg((int)(now - mDebug_orgTime)) << " ms" << std::endl;

	mImageImport->SetDataOrigin(0, 0, 0);

	//for linear probes used in other substance than the scanner is calibrated for we want to compensate
	//for the change in sound of speed in that substance, do this by changing spacing in the images y-direction,
	//this is only valid for linear probes
	mImageImport->SetDataSpacing(spacing[0], spacing[1] * mLinearSoundSpeedCompesation, spacing[2]);

	mImageImport->SetWholeExtent(0, mSize[0] - 1, 0, mSize[1] - 1, 0, mSize[2] - 1);
	mImageImport->SetDataExtentToWholeExtent();
	mImageImport->SetImportVoidPointer(mImageMessage->GetScalarPointer());

	mImageImport->Modified();

	//Only do the following for the digital Ultrasonix interface
	if (updateSonixParameters)
	{
		//TODO: Send all necessary parameters (origin + size) in IGTLinkSonixStatusMessage and only update from there.
		this->updateSonix();
	}
}

void OpenIGTLinkRTSource::updateSonixStatus(IGTLinkSonixStatusMessage::Pointer message)
{
	//std::cout << "void OpenIGTLinkRTSource::updateSonixStatus(IGTLinkSonixStatusMessage::Pointer message)" << std::endl;
	//TODO: Use the status information

	int roi[8];
	message->GetROI(roi);
	std::cout << "Ultrasonix roi (x,y)*4: " << roi[0] << " " << roi[1] << " " << roi[2] << " " << roi[3] << " "
					<< roi[4] << " " << roi[5] << " " << roi[6] << " " << roi[7] << " " << std::endl;

	ssc::ToolPtr tool = boost::shared_dynamic_cast<Tool>(ssc::toolManager()->getDominantTool());
	if (!tool)
		return;
	ProbePtr probe = boost::shared_dynamic_cast<Probe>(tool->getProbe());
	if (!probe)
	{
		ssc::messageManager()->sendWarning("OpenIGTLinkRTSource::updateSonixStatus: Dominant tool is not a probe");
		return;
	}
	if (tool->getProbeSector().mType == ssc::ProbeData::tNONE)
	{
		ssc::messageManager()->sendInfo(
						"OpenIGTLinkRTSource::updateSonixStatus: Dominant tool have no sector information. This will be created.");
//  	return;
	}

//  mImageImport->GetDataSpacing(spacing);
	message->GetSpacing(mSpacing); //Use spacing from message

//  float origin[3];
//  message->GetOrigin(origin);
//  std::cout << "origin3: " << origin[0] << ", " << origin[1] << ", " << origin[2] << std::endl;

	//Test if x and y values are matching that of a linear probe
	//x					left									right
	if ((roi[0] != roi[6]) && (roi[2] != roi[4]) &&
	//y					top										bottom
					(roi[1] != roi[3]) && (roi[5] != roi[7]))
	{
		ssc::messageManager()->sendWarning("ROI x/y values not matching that of a linear probe");

		if (probe->getData().mType != ssc::ProbeData::tSECTOR)
			ssc::messageManager()->sendWarning(
							"OpenIGTLinkRTSource::updateSonixStatus: Probe not sector probe, but ROI is from sector probe");

		//TODO: Calculate depthStart, depthEnd and width from the ROI x/y points or send more parameters: uCurce (Ulterius curve definition)
		//  probeSector = ssc::ProbeData(ssc::ProbeData::tSECTOR, depthStart, depthEnd, width);
	}
	else // Linear probe
	{
		if (probe->getData().mType == ssc::ProbeData::tSECTOR)
			ssc::messageManager()->sendWarning(
							"OpenIGTLinkRTSource::updateSonixStatus: Probe is a sector probe, but ROI is from linear probe");
		mDepthStart = roi[1]; // in pixels
		mDepthEnd = roi[5]; // in pixels
		mWidth = roi[2] - roi[0]; // in pixels
	}

	updateSonixParameters = true;
}

//Update probe sector parameters
void OpenIGTLinkRTSource::updateSonix()
{
	ssc::ToolPtr tool = boost::shared_dynamic_cast<Tool>(ssc::toolManager()->getDominantTool());
	if (!tool)
		return;
	ProbePtr probe = boost::shared_dynamic_cast<Probe>(tool->getProbe());
	if (!probe)
	{
		ssc::messageManager()->sendWarning("OpenIGTLinkRTSource::updateSonixStatus: Dominant tool is not a probe");
		return;
	}
	if (tool->getProbeSector().mType == ssc::ProbeData::tNONE)
	{
		ssc::messageManager()->sendInfo(
						"OpenIGTLinkRTSource::updateSonixStatus: Dominant tool have no sector information. This will be created.");
//    return;
	}

	double dStart = (mDepthStart - mOrigin[1]) * mSpacing[1]; //mm
	double dEnd = (mDepthEnd - mOrigin[1]) * mSpacing[1]; //mm
	double dWidth = mWidth * mSpacing[0]; //mm
	if (tool->getProbeSector().mType != ssc::ProbeData::tNONE)
		probe->changeProbeSectorParameters(dStart, dEnd, dWidth); //mm
	else // No probe sector. Create one.
	{
		ssc::ProbeData probeSector = ssc::ProbeData(ssc::ProbeData::tLINEAR, dStart, dEnd, dWidth);
		probe->setProbeSector(probeSector);
		probe->setRTSource(videoService()->getVideoConnection()->getVideoSource());
	}
//  std::cout << "depth Start/end (pixels): " << depthStart << ", " << depthEnd << "     mm: " << dStart << ", " << dEnd << std::endl;

//  std::cout << "set spacing: " << mSpacing[0] << " " << mSpacing[1] << std::endl;
//  std::cout << "set size: " << mSize[0] << " " << mSize[1] << std::endl;
//  std::cout << "set origin: " << mOrigin[0] << " " << mOrigin[1] << std::endl;

	ssc::ProbeData::ProbeImageData imageData;
	imageData.mSpacing = ssc::Vector3D(mSpacing[0], mSpacing[1], 1);
	imageData.mSize = QSize(mSize[0], mSize[1]);
	imageData.mOrigin_p = ssc::Vector3D(mOrigin[0], mOrigin[1], 0);

	probe->setProbeImageData(imageData);
}

void OpenIGTLinkRTSource::updateImage(igtl::ImageMessage::Pointer message)
{

//  std::cout << "void OpenIGTLinkRTSource::updateImage(igtl::ImageMessage::Pointer message)" << std::endl;

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
	if (mImageImport->GetOutput()->GetNumberOfScalarComponents() == 4 && !mFilter_ARGB_RGBA)
	{
		mFilter_ARGB_RGBA = this->createFilterARGB2RGBA(mImageImport->GetOutput());
		mRedirecter->SetInput(mFilter_ARGB_RGBA);
	}

	emit newFrame();

//  double now = (double)QDateTime::currentDateTime().toMSecsSinceEpoch();
//  std::cout << QString("cv+cx delay: %1").arg((int)(now - mDebug_orgTime)) << " ms" << std::endl;
}

/**Create a pipeline that convert the input 4-component ARGB image (from QuickTime-Mac)
 * into a vtk-style RGBA image.
 *
 */
vtkImageDataPtr OpenIGTLinkRTSource::createFilterARGB2RGBA(vtkImageDataPtr input)
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

vtkImageDataPtr OpenIGTLinkRTSource::getVtkImageData()
{
	return mRedirecter->GetOutput();
}

}
