/*
 * cxImageSenderGE.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: olevs
 */

#include "cxImageSenderGE.h"

#ifdef CX_USE_ISB_GE

#include <QCoreApplication>
#include <QTimer>
#include <QTime>
#include <QHostAddress>
#include <QFileInfo>
#include <vtkImageFlip.h>
#include "igtlOSUtil.h"
#include "igtlImageMessage.h"
#include "igtlServerSocket.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vtkMetaImageReader.h"
#include "vtkImageImport.h"
#include "vtkLookupTable.h"
#include "vtkImageMapToColors.h"
#include "vtkMetaImageWriter.h"
#include "sscMessageManager.h"
#include "sscTypeConversions.h"
#include "cxDataLocations.h"
#include "geConfig.h"
#include "vtkImageChangeInformation.h"
#include "vtkForwardDeclarations.h"

namespace cx
{

QString ImageSenderGE::getType()
{
	return "ISB_GE";
}

QStringList ImageSenderGE::getArgumentDescription()
{
	QStringList retval;
	//Tabs are set so that tool tip looks nice
	retval << "--ip:		GE scanner IP address";//default = 127.0.0.1, find a typical direct link address
	retval << "--streamport:		GE scanner streaming port, default = 6543";
	retval << "--commandport:	GE scanner command port, default = -1";//Unnecessary for us?
	retval << "--buffersize:		Size of GEStreamer buffer, default = 10";
	retval << "--imagesize:		Returned image/volume size in pixels (eg. 500x500x1), default = auto";
	retval << "--openclpath:		Path to ScanConvert.cl";
	retval << "--test:		GEStreamer test mode (no, 2D or 3D), default = no";
	retval << "--useOpenCL:		Use OpenCL for scan conversion, default = 1";
	retval << "--streams:		Used video streams (separated by , with no spaces), default = scanconverted, Available streams (only 2D for now): scanconverted,tissue,bandwidth,frequency,velocity (all)";
	return retval;
}

ImageSenderGE::ImageSenderGE(QObject* parent) :
	ImageSender(parent),
	mInitialized(false),
	mSendTimer(0),
	mGrabTimer(0),
	mExportScanconverted(true),
	mExportTissue(false),
	mExportBandwidth(false),
	mExportFrequency(false),
	mExportVelocity(false)
{
	//data_streaming::DataStreamApp test;
	mRenderTimer.reset(new CyclicActionTimer("GE Grabber Timer"));

//	mImgStream = vtkSmartPointer<vtkImageData>();
	mImgExportedStream = vtkSmartPointer<data_streaming::vtkExportedStreamData>();

	mGrabTimer = new QTimer(this);
	connect(mGrabTimer, SIGNAL(timeout()), this, SLOT(grab())); // this signal will be executed in the thread of THIS, i.e. the main thread.
	mSendTimer = new QTimer(this);
//	connect(mSendTimer, SIGNAL(timeout()), this, SLOT(send())); // this signal will be executed in the thread of THIS, i.e. the main thread.
}

void ImageSenderGE::initialize(StringMap arguments)
{
	mArguments = arguments;

	//where to dump the hdf files
	std::string fileRoot = "c:\\test";
	//is dumping enabled
	bool dumpHdfToDisk = false;

	//interpolation type
	data_streaming::InterpolationType interpType = data_streaming::Bilinear;

	//Set defaults
	if (!mArguments.count("ip"))
		mArguments["ip"] = "127.0.0.1";
	if (!mArguments.count("streamport"))
		mArguments["streamport"] = "6543";
	if (!mArguments.count("commandport"))
		mArguments["commandport"] = "-1";
	if (!mArguments.count("buffersize"))
		mArguments["buffersize"] = "10";
    if (!mArguments.count("openclpath"))
        mArguments["openclpath"] = "";
    if (!mArguments.count("test"))
        mArguments["test"] = "no";
    if (!mArguments.count("imagesize"))
        mArguments["imagesize"] = "auto";
    if (!mArguments.count("useOpenCL"))
        mArguments["useOpenCL"] = "1";
    if (!mArguments.count("streams"))
        mArguments["streams"] = "scanconverted";

   	int bufferSize = convertStringWithDefault(mArguments["buffersize"], -1);

   	long imageSize = 1;
	if (!mArguments["imagesize"].compare("auto", Qt::CaseInsensitive) == 0)
	{
	   	QStringList sizeList = QString(mArguments["imagesize"]).split(QRegExp("[x,X,*]"), QString::SkipEmptyParts);
	   	bool ok = false;
		for (int i = 0; i < sizeList.length(); i++)
		{
			int dimSize = convertStringWithDefault(sizeList.at(i), 1);
			imageSize *= dimSize;
			ok = true;
		}
		if (imageSize <= 0 || !ok)
		{
			ssc::messageManager()->sendError("Error with calculated image size. imagesize: " + mArguments["imagesize"] + " = " + qstring_cast(imageSize));
		}
	}

   	//Select image streams to export
   	//Accept , ; . as separators
   	QStringList streamList = QString(mArguments["streams"]).split(",", QString::SkipEmptyParts);
   	mExportScanconverted = false;
   	mExportTissue = false;
   	mExportBandwidth = false;
   	mExportFrequency = false;
   	mExportVelocity = false;
   	for (int i = 0; i < streamList.length(); i++)
   	{
   		if (streamList.at(i).compare("scanconverted", Qt::CaseInsensitive) == 0)
   			mExportScanconverted = true;
   		else if (streamList.at(i).compare("tissue", Qt::CaseInsensitive) == 0)
   			mExportTissue = true;
   		else if (streamList.at(i).compare("bandwidth", Qt::CaseInsensitive) == 0)
   			mExportBandwidth = true;
   		else if (streamList.at(i).compare("frequency", Qt::CaseInsensitive) == 0)
   			mExportFrequency = true;
   		else if (streamList.at(i).compare("velocity", Qt::CaseInsensitive) == 0)
   			mExportVelocity = true;
   		else if (streamList.at(i).compare("all", Qt::CaseInsensitive) == 0)
   		{
   			mExportScanconverted = true;
   			mExportTissue = true;
   			mExportBandwidth = true;
   			mExportFrequency = true;
   			mExportVelocity = true;
   		}
   		else
   			ssc::messageManager()->sendWarning("ImageSenderGE: Unknown stream: " + streamList.at(i));
   	}

   	std::string openclpath = mArguments["openclpath"].toStdString();
	bool useOpenCL = convertStringWithDefault(mArguments["useOpenCL"], 1);

   	//Find GEStreamer OpenCL kernel code
   	//Look in arg in, GEStreamer source dir, and installed dir
   	QStringList paths;
   	paths << QString::fromStdString(openclpath) << GEStreamer_KERNEL_PATH << DataLocations::getShaderPath();
//   	std::cout << "OpenCL kernel paths: " << paths.join("  \n").toStdString();
   	QFileInfo path;
	path = QFileInfo(paths[0] + QString("/ScanConvertCL.cl"));
	if (!path.exists())
		path = QFileInfo(paths[1] + QString("/ScanConvertCL.cl"));
	if (!path.exists())
		path = QFileInfo(paths[2] + "/ScanConvertCL.cl");
	if (!path.exists())
	{
		ssc::messageManager()->sendWarning("Error: Can't find ScanConvertCL.cl in any of\n  " + paths.join("  \n"));
	} else
		openclpath = path.absolutePath().toStdString();

	mGEStreamer.InitializeClientData(fileRoot, dumpHdfToDisk, imageSize, interpType, bufferSize, openclpath, useOpenCL);

	//Setup the needed data stream types. The default is only scan converted data
//	mGEStreamer.SetupExportParameters(true, false, false, false);
	mGEStreamer.SetupExportParameters(mExportScanconverted, mExportTissue, mExportBandwidth, mExportFrequency);

	// Run an init/deinit to check that we have contact right away.
	// Do NOT keep the connection open: This is because we have no good way to
	// release resources if the server is a local app and is killed by CustusX.
	// This way, we can disconnect (thus releasing resources), and then safely
	// remove the usb cable without having dangling resources in openCV. (problem at least on Linux)
//	this->initialize_local();
//	this->deinitialize_local();

}

void ImageSenderGE::deinitialize_local()
{
	//Set mImgStream as an empty pointer
	mImgExportedStream = vtkSmartPointer<data_streaming::vtkExportedStreamData>();

	//Clear frame geometry
	data_streaming::frame_geometry emptyGeometry;
	mFrameGeometry = emptyGeometry;
	mFlowGeometry = emptyGeometry;
}

bool ImageSenderGE::initialize_local()
{
	std::string hostIp = mArguments["ip"].toStdString();
	int streamPort = convertStringWithDefault(mArguments["streamport"], -1);
	int commandPort = convertStringWithDefault(mArguments["commandport"], -1);

	data_streaming::TestMode test;
	if (mArguments["test"].compare("2D", Qt::CaseInsensitive) == 0)
		test = data_streaming::test2D;
	else if (mArguments["test"].compare("1", Qt::CaseInsensitive) == 0) //Also accept 1 as 2D test
		test = data_streaming::test2D;
	else if (mArguments["test"].compare("3D", Qt::CaseInsensitive) == 0)
		test = data_streaming::test3D;
	else //no
		test = data_streaming::noTest;

	return mGEStreamer.ConnectToScanner(hostIp, streamPort, commandPort, test);
	mGEStreamer.SetFlipTexture(false);

//	mImgStream = mGEStreamer.ConnectToScanner(hostIp, streamPort, commandPort, testMode);
//	if(!mImgStream)
//		return false;
//	else
//		return true;
}

bool ImageSenderGE::startStreaming(GrabberSenderPtr sender)
{
	mInitialized = this->initialize_local();

	if (!mInitialized || !mGrabTimer || !mSendTimer)
	{
		std::cout << "ImageSenderGE: Failed to start streaming: Not initialized." << std::endl;
		return false;
	}

//	mSocket = socket;
	mSender = sender;
	mGrabTimer->start(5);
	//mSendTimer->start(40);
	std::cout << "Started streaming from GS device" << std::endl;
	return true;
}

void ImageSenderGE::stopStreaming()
{
	if (!mInitialized || !mGrabTimer || !mSendTimer)
		return;
	mGrabTimer->stop();
	mSendTimer->stop();
//	mSocket = NULL;
	mSender.reset();

	this->deinitialize_local();
}

void ImageSenderGE::grab()
{

	//Wait for next frame
	//Will only work with scanner, not simple test data
	/*if (mGEStreamer.stream)
		mGEStreamer.stream->WaitForImageData();
	else
	{
		std::cout << "ImageSenderGE::grab(): No mGEStreamer.stream" << std::endl;
	}*/

//	mGEStreamer.WaitForImageData();
	if (!mGEStreamer.HasNewImageData())
		return;
	mRenderTimer->begin();
//	mRenderTimer->time("wait");

	vtkSmartPointer<data_streaming::vtkExportedStreamData> imgExportedStream = mGEStreamer.GetExportedStreamDataAndMoveToNextFrame();

	mRenderTimer->time("scanc");

	//Get new image
	if(!imgExportedStream)
	{
		std::cout << "ImageSenderGE::grab(): No image from GEStreamer" << std::endl;
		return;
	}

	//Get frame geometry if we don't have it yet
	//if(imgExportedStream->GetTissueGeometryChanged())
	if (!equal(mFrameGeometry, imgExportedStream->GetTissueGeometry()))
	{
		// Frame geometry have changed.
		mFrameGeometry = imgExportedStream->GetTissueGeometry();
		mFrameGeometryChanged = true;
	}
	else
		mFrameGeometryChanged = false;

	//if(imgExportedStream->GetFlowGeometryChanged() /*&& (mExportBandwidth || mExportFrequency)*/)
	if (!equal(mFlowGeometry, imgExportedStream->GetFlowGeometry()))
	{
		// Frame geometry have changed.
		mFlowGeometry = imgExportedStream->GetFlowGeometry();
		mFlowGeometryChanged = true;
	}
	else
		mFlowGeometryChanged = false;

	mRenderTimer->time("get");

	mImgExportedStream = imgExportedStream;
	mLastGrabTime = mImgExportedStream->GetTimeStamp();


	send();

	mRenderTimer->time("sent");

//	if (mRenderTimer->intervalPassed())
//	{
//        static int counter=0;
//        if (++counter%3==0)
//            ssc::messageManager()->sendDebug(mRenderTimer->dumpStatisticsSmall());
        mRenderTimer->reset();
//	}
}

void ImageSenderGE::send()
{
	if (!mSender || !mSender->isReady())
		return;

	QString uid;
	if (mExportScanconverted && mImgExportedStream->GetScanConvertedImage())
	{
		uid = "ScanConverted [RGBA]";
		send(uid, mImgExportedStream->GetScanConvertedImage(), mFrameGeometry, mFrameGeometryChanged);
	}
	if (mExportTissue && mImgExportedStream->GetTissueImage())
	{
		uid = "Tissue [R]";
		send(uid, mImgExportedStream->GetTissueImage(), mFrameGeometry, mFrameGeometryChanged);
	}
	if (mExportBandwidth && mImgExportedStream->GetBandwidthImage())
	{
		uid = "Bandwidth [R]";
		send(uid, mImgExportedStream->GetBandwidthImage(), mFlowGeometry, mFlowGeometryChanged);
	}
	if (mExportFrequency && mImgExportedStream->GetFrequencyImage())
	{
		uid = "Frequency [R]";
		send(uid, mImgExportedStream->GetFrequencyImage(), mFlowGeometry, mFlowGeometryChanged);
	}
	if (mExportVelocity && mImgExportedStream->GetVelocityImage())
	{
		uid = "Velocity [R]";
		send(uid, mImgExportedStream->GetVelocityImage(), mFlowGeometry, mFlowGeometryChanged);
	}
}


void ImageSenderGE::send(const QString& uid, const vtkImageDataPtr& img, data_streaming::frame_geometry geometry, bool geometryChanged)
{
	mRenderTimer->time("startsend");
//	vtkImageFlipPtr flipper = vtkImageFlipPtr::New();
//	flipper->SetInput(img);
//	flipper->SetFilteredAxis(0);
//	vtkImageDataPtr	flipped = flipper->GetOutput();
//	flipped->Update();
//	vtkImageDataPtr copy = vtkImageDataPtr::New();
//	copy->DeepCopy(img);
	if (geometryChanged)
	{
		ssc::ProbeData frameMessage = getFrameStatus(uid, geometry, img);
		mSender->send(frameMessage);
		std::cout << uid << " Nyquist " << geometry.vNyquist << std::endl;
		int*  dim = img->GetDimensions();
		std::cout << uid << " Volume size: " << dim[0] << " " << dim[1] << " " << dim[2] << std::endl;

	}
//	mRenderTimer->time("sendpr");

	// CustusX does not handle nonzero origin - set to zero, but AFTER getFrameStatus() is called.
	vtkImageChangeInformationPtr center = vtkImageChangeInformationPtr::New();
	center->SetInput(img);
	center->SetOutputOrigin(0,0,0);
	center->Update();
	mRenderTimer->time("orgnull");

	ssc::ImagePtr message(new ssc::Image(uid, center->GetOutput()));
	mRenderTimer->time("createimg");

	mSender->send(message);
	mRenderTimer->time("sendersend");
}

/*IGTLinkImageMessage::Pointer ImageSenderGE::getImageMessage()
{
	if(!mImgExportedStream)
	{
		std::cout << "ImageSenderGE::getImageMessage(): No GEStreamer image" << std::endl;
		return IGTLinkImageMessage::Pointer();
	}

	vtkSmartPointer<vtkImageData> img = mImgExportedStream->GetScanConvertedImage();
	if(!img)
	{
		std::cout << "ImageSenderGE::getImageMessage(): No scan converted image from GEStreamer" << std::endl;
		return IGTLinkImageMessage::Pointer();
	}

	IGTLinkImageMessage::Pointer retval = IGTLinkImageMessage::New();

//	int* size = mGEStreamer.VolumeDimensions; // May be 3 dimensions
	int* size = img->GetDimensions(); // May be 3 dimensions
	int offset[] = { 0, 0, 0 };

	int scalarType = -1;
	if(img->GetNumberOfScalarComponents() == 3 || img->GetNumberOfScalarComponents() == 4)
	{
		scalarType = IGTLinkImageMessage::TYPE_UINT32;// scalar type
	} else if(img->GetNumberOfScalarComponents() == 1)
	{
		if(img->GetScalarTypeMax() > 256 && img->GetScalarTypeMax() <= 65536)
		{
			scalarType = IGTLinkImageMessage::TYPE_UINT16;// scalar type
		}
		else if(img->GetScalarTypeMax() <= 256)
		{
			scalarType = IGTLinkImageMessage::TYPE_UINT8;// scalar type
		}
	}
	if (scalarType == -1)
	{
		std::cerr << "unknown image type" << std::endl;
		return IGTLinkImageMessage::Pointer();
	}

	retval->SetDimensions(size); // May be 3 dimensions
//	retval->SetSpacing(mGEStreamer.VoxelSize[0], mGEStreamer.VoxelSize[1], mGEStreamer.VoxelSize[2]); // May be 3 dimensions
	retval->SetScalarType(scalarType);
	retval->SetDeviceName("ImageSenderGE [BGRX]");
	retval->SetSubVolume(size, offset);
	retval->AllocateScalars();

	//Get timestamp from GEStreamer
	igtl::TimeStamp::Pointer ts;
	ts = igtl::TimeStamp::New();
//	double seconds = 1.0 / 1000 * (double) mLastGrabTime.toMSecsSinceEpoch();
	double seconds = 1.0 / 1000 * mLastGrabTime;
	ts->SetTime(seconds); //in seconds
	retval->SetTimeStamp(ts);

	igtl::Matrix4x4 matrix;
	matrix[0][0] = 1.0;  matrix[1][0] = 0.0;  matrix[2][0] = 0.0; matrix[3][0] = 0.0;
	matrix[0][1] = 0.0;  matrix[1][1] = 1.0;  matrix[2][1] = 0.0; matrix[3][1] = 0.0;
	matrix[0][2] = 0.0;  matrix[1][2] = 0.0;  matrix[2][2] = 1.0; matrix[3][2] = 0.0;
	matrix[0][3] = 0.0;  matrix[1][3] = 0.0;  matrix[2][3] = 0.0; matrix[3][3] = 1.0;
	retval->SetMatrix(matrix);

	retval->SetOrigin(img->GetOrigin()[0], img->GetOrigin()[1], img->GetOrigin()[2]);
	retval->SetSpacing(img->GetSpacing()[0], img->GetSpacing()[1], img->GetSpacing()[2]); // May be 3 dimensions

	//std::cout << "spacing: " << img->GetSpacing()[0] << " " << img->GetSpacing()[1] << " " << img->GetSpacing()[2] << std::endl;

	//Set image data
	int fsize = retval->GetImageSize();
	memcpy(retval->GetScalarPointer(), img->GetScalarPointer(), fsize);

	return retval;
}*/

ssc::ProbeData ImageSenderGE::getFrameStatus(QString uid, data_streaming::frame_geometry geometry, vtkSmartPointer<vtkImageData> img)
{
	ssc::ProbeData retval;
	if (!img || !mImgExportedStream)
		return retval;

	//Create ProbeImageData struct
	ssc::ProbeData::ProbeImageData imageData;
	imageData.mOrigin_p = ssc::Vector3D(geometry.origin[0] + img->GetOrigin()[0],
					geometry.origin[1]+ img->GetOrigin()[1],
					geometry.origin[2]+ img->GetOrigin()[2]);
	imageData.mSize = QSize(img->GetDimensions()[0], img->GetDimensions()[1]);
	imageData.mSpacing = ssc::Vector3D(img->GetSpacing());
	imageData.mClipRect_p = ssc::DoubleBoundingBox3D(img->GetExtent());

	// 1 = sector, 2 = linear
	if (geometry.imageType == data_streaming::Linear) //linear
		retval = ssc::ProbeData(ssc::ProbeData::tLINEAR);
	else //sector
		retval = ssc::ProbeData(ssc::ProbeData::tSECTOR);

	// Set start and end of sector in mm from origin
	// Set width of sector in mm for LINEAR, width of sector in radians for SECTOR.
	retval.setSector(mFrameGeometry.depthStart, mFrameGeometry.depthEnd, mFrameGeometry.width);
	retval.setImage(imageData);
//	retval.setTemporalCalibration();//Can set everything except temporal calibration

	retval.setUid(uid);
	return retval;
}

/*IGTLinkUSStatusMessage::Pointer ImageSenderGE::getFrameStatus()
{
	IGTLinkUSStatusMessage::Pointer retval = IGTLinkUSStatusMessage::New();

	vtkSmartPointer<vtkImageData> img = vtkSmartPointer<vtkImageData>();
	if(mImgExportedStream)
		vtkSmartPointer<vtkImageData> img = mImgExportedStream->GetScanConvertedImage();

  //This is origin from the scanner (= 0,0,0)
  //Origin according to image is set in the image message
  if (mImgExportedStream && img)
	  retval->SetOrigin(mFrameGeometry.origin[0] + img->GetOrigin()[0],
			  mFrameGeometry.origin[1]+ img->GetOrigin()[1],
			  mFrameGeometry.origin[2]+ img->GetOrigin()[2]);
  else
	  retval->SetOrigin(mFrameGeometry.origin);

  // 1 = sector, 2 = linear
  if (mFrameGeometry.imageType == data_streaming::Linear) //linear
	  retval->SetProbeType(2);
  else //sector
	  retval->SetProbeType(1);

  retval->SetDepthStart(mFrameGeometry.depthStart);// Start of sector in mm from origin
  retval->SetDepthEnd(mFrameGeometry.depthEnd);	// End of sector in mm from origin
  retval->SetWidth(mFrameGeometry.width);// Width of sector in mm for LINEAR, Width of sector in radians for SECTOR.

//  std::cout << "origin: " << mFrameGeometry.origin[0] << " " << mFrameGeometry.origin[1] << " " << mFrameGeometry.origin[2] << std::endl;
//  std::cout << "imageType: " << mFrameGeometry.imageType << std::endl;
//  std::cout << "depthStart: " << mFrameGeometry.depthStart << " end: " << mFrameGeometry.depthEnd << std::endl;
//  std::cout << "width: " << mFrameGeometry.width << std::endl;
//  std::cout << "tilt: " << mFrameGeometry.tilt << std::endl;

  return retval;
}*/

bool ImageSenderGE::equal(data_streaming::frame_geometry a, data_streaming::frame_geometry b)
{
	return !((a.origin[0] != b.origin[0]) || (a.origin[1] != b.origin[1]) || (a.origin[2] != b.origin[2])
			|| (a.imageType != b.imageType)
			|| !ssc::similar(a.depthStart, b.depthStart, 0.01)
			|| !ssc::similar(a.depthEnd, b.depthEnd, 0.01)
			|| !ssc::similar(a.width, b.width, 0.0001)
			|| !ssc::similar(a.tilt, b.tilt, 0.0001)
			|| !ssc::similar(a.elevationWidth, b.elevationWidth, 0.0001)
			|| !ssc::similar(a.elevationTilt, b.elevationTilt, 0.0001)
			|| !ssc::similar(a.vNyquist, b.vNyquist, 0.0001)
			|| !ssc::similar(a.PRF, b.PRF, 0.0001));
}

}// namespace cx

#endif //CX_USE_ISB_GE
