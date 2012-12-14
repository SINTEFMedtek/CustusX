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
	retval << "--imagesize:		Returned image/volume size in pixels, default = 500x500x1";
	retval << "--openclpath:		Path to ScanConvert.cl";
	retval << "--testmode:		GEStreamer test mode, default = 0";
	retval << "--useOpenCL:		Use OpenCL for scan conversion, default = 1";
	return retval;
}

ImageSenderGE::ImageSenderGE(QObject* parent) :
	ImageSender(parent),
	mInitialized(false),
	mSendTimer(0),
	mGrabTimer(0)
{
	//data_streaming::DataStreamApp test;

	mImgStream = vtkSmartPointer<vtkImageData>();

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

	//size of the scan converted 2D image in pixels
//	long imageSize2D = 500*500;

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
    if (!mArguments.count("testmode"))
        mArguments["testmode"] = "0";
    if (!mArguments.count("imagesize"))
        mArguments["imagesize"] = "500x500x1";
    if (!mArguments.count("useOpenCL"))
        mArguments["useOpenCL"] = "1";

   	int bufferSize = convertStringWithDefault(mArguments["buffersize"], -1);

   	QStringList sizeList = QString(mArguments["imagesize"]).split(QRegExp("[x,X,*]"), QString::SkipEmptyParts);
   	long imageSize = 1;
   	for (int i = 0; i < sizeList.length(); i++)
   	{
   		int dimSize = convertStringWithDefault(sizeList.at(i), 1);
   		imageSize *= dimSize;
   	}
   	if (imageSize <= 1)
   		ssc::messageManager()->sendError("Error with calculated image size. imagesize: " + mArguments["imagesize"] + " = " + qstring_cast(imageSize));

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
	mImgStream = vtkSmartPointer<vtkImageData>();

	//Clear frame geometry
	data_streaming::frame_geometry emptyGeometry;
	mFrameGeometry = emptyGeometry;
}

bool ImageSenderGE::initialize_local()
{
	std::string hostIp = mArguments["ip"].toStdString();
	int streamPort = convertStringWithDefault(mArguments["streamport"], -1);
	int commandPort = convertStringWithDefault(mArguments["commandport"], -1);
	bool testMode = convertStringWithDefault(mArguments["testmode"], 0);

	mImgStream = mGEStreamer.ConnectToScanner(hostIp, streamPort, commandPort, testMode);
	if(!mImgStream)
		return false;
	else
		return true;
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

	mGEStreamer.WaitForImageData();
//	if (!mGEStreamer.HasNewImageData())
//		return;

	bool testMode = convertStringWithDefault(mArguments["testmode"], 0);

	//Update mGEStreamer.frame
	//All function should now be called on this object
	vtkSmartPointer<vtkImageData> imgStream = mGEStreamer.GetNewFrame();
	if(!testMode && mGEStreamer.frame == NULL)
	{
		std::cout << "ImageSenderGE::grab() failed: Got no frame" << std::endl;
		return;
	}

	//Get frame geometry if we don't have it yet
	if(!testMode && (mGEStreamer.frame->GetGeometryChanged() || (mFrameGeometry.width < 0.0001)))
	{
		// Frame geometry have changed.
		mFrameGeometry = mGEStreamer.GetFrameGeometry();
		mFrameGeometryChanged = true;
		//std::cout << "Get new GE frame geometry" << std::endl;
	}
	else
		mFrameGeometryChanged = false;

	if(!imgStream)
	{
		std::cout << "ImageSenderGE::grab(): No image from GEStreamer" << std::endl;
		return;
	}
	else
	{
		//sstd::cout << "ImageSenderGE::grab(): Got image from GEStreamer" << std::endl;
	}
	//Only set image and time if we got a new image
	mImgStream = imgStream;
	mLastGrabTime = mGEStreamer.GetTimeStamp();

	send();
}

void ImageSenderGE::send()
{
	if (!mSender || !mSender->isReady())
		return;

	if(mFrameGeometryChanged)
	{
		IGTLinkUSStatusMessage::Pointer statMsg =  this->getFrameStatus();
		mSender->send(statMsg);
	}


	IGTLinkImageMessage::Pointer imgMsg = this->getImageMessage();
	if (!imgMsg)
		return;

	mSender->send(imgMsg);

//	if (mSocket)
//	{
//		//------------------------------------------------------------
//		// Pack (serialize) and send
//		imgMsg->Pack();
//		mSocket->write(reinterpret_cast<const char*> (imgMsg->GetPackPointer()), imgMsg->GetPackSize());
//		//  std::cout << "tick " << start.msecsTo(QTime::currentTime()) << " ms" << std::endl;
//	}
}


IGTLinkImageMessage::Pointer ImageSenderGE::getImageMessage()
{
	if(!mImgStream)
	{
		std::cout << "ImageSenderGE::getImageMessage(): No GEStreamer image" << std::endl;
		return IGTLinkImageMessage::Pointer();
	}

	IGTLinkImageMessage::Pointer retval = IGTLinkImageMessage::New();

//	int* size = mGEStreamer.VolumeDimensions; // May be 3 dimensions
	int* size = mImgStream->GetDimensions(); // May be 3 dimensions
	int offset[] = { 0, 0, 0 };

	int scalarType = -1;
	if(mImgStream->GetNumberOfScalarComponents() == 3 || mImgStream->GetNumberOfScalarComponents() == 4)
	{
		scalarType = IGTLinkImageMessage::TYPE_UINT32;// scalar type
	} else if(mImgStream->GetNumberOfScalarComponents() == 1)
	{
		if(mImgStream->GetScalarTypeMax() > 256 && mImgStream->GetScalarTypeMax() <= 65536)
		{
			scalarType = IGTLinkImageMessage::TYPE_UINT16;// scalar type
		}
		else if(mImgStream->GetScalarTypeMax() <= 256)
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
	retval->SetDeviceName("ImageSenderGE");
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

	retval->SetOrigin(mImgStream->GetOrigin()[0], mImgStream->GetOrigin()[1], mImgStream->GetOrigin()[2]);
	retval->SetSpacing(mImgStream->GetSpacing()[0], mImgStream->GetSpacing()[1], mImgStream->GetSpacing()[2]); // May be 3 dimensions

	//std::cout << "spacing: " << mImgStream->GetSpacing()[0] << " " << mImgStream->GetSpacing()[1] << " " << mImgStream->GetSpacing()[2] << std::endl;

	//Set image data
	int fsize = retval->GetImageSize();
	memcpy(retval->GetScalarPointer(), mImgStream->GetScalarPointer(), fsize);

	return retval;
}

IGTLinkUSStatusMessage::Pointer ImageSenderGE::getFrameStatus()
{
  IGTLinkUSStatusMessage::Pointer retval = IGTLinkUSStatusMessage::New();

  //This is origin from the scanner (= 0,0,0)
  //Origin according to image is set in the image message
  if (mImgStream)
	  retval->SetOrigin(mFrameGeometry.origin[0] + mImgStream->GetOrigin()[0],
			  mFrameGeometry.origin[1]+ mImgStream->GetOrigin()[1],
			  mFrameGeometry.origin[2]+ mImgStream->GetOrigin()[2]);
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
}

}// namespace cx

#endif //CX_USE_ISB_GE
