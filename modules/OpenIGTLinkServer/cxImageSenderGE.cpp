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

namespace cx
{

QString ImageSenderGE::getType()
{
	return "ISB_GE";
}

QStringList ImageSenderGE::getArgumentDescription()
{
	QStringList retval;
	retval << "For now configurations are written in the config file located in the directory to which the DATA_STREAM_ROOT environment variable points to";
	return retval;
}

ImageSenderGE::ImageSenderGE(QObject* parent) :
	ImageSender(parent),
//	mSocket(NULL),
	mSendTimer(0),
	mGrabTimer(0)
{
	//data_streaming::DataStreamApp test;

	mGrabTimer = new QTimer(this);
	connect(mGrabTimer, SIGNAL(timeout()), this, SLOT(grab())); // this signal will be executed in the thread of THIS, i.e. the main thread.
	mSendTimer = new QTimer(this);
	connect(mSendTimer, SIGNAL(timeout()), this, SLOT(send())); // this signal will be executed in the thread of THIS, i.e. the main thread.
}

void ImageSenderGE::initialize(StringMap arguments)
{
	mArguments = arguments;

	//the stream config filename - has to be located in the directory to which the DATA_STREAM_ROOT environment variable points to
	std::string configFilename = "config.txt";

	//where to dump the hdf files
	std::string fileRoot = "c:\\test";
	//is dumping enabled
	bool dumpHdfToDisk = false;

	//size of the scan converted texture
	int volumeDimensions[3] = {600, 600, 1}; //[voxels/pixels]
	double voxelSize[3] = {0.3, 0.3, 1.0}; //[mm]

	//interpolation type
	data_streaming::InterpolationType interpType = data_streaming::Bilinear;

	mGEStreamer.InitializeClientData(configFilename, fileRoot, dumpHdfToDisk, volumeDimensions, voxelSize, interpType);

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
}

bool ImageSenderGE::initialize_local()
{
	mImgStream = mGEStreamer.ConnectToScanner();
	if(!mImgStream)
		return false;
	else
		return true;
}

void ImageSenderGE::startStreaming(GrabberSenderPtr sender)
{
	bool initialized = this->initialize_local();

	if (!initialized || !mGrabTimer || !mSendTimer)
	{
		std::cout << "ImageSenderGE: Failed to start streaming: Not initialized." << std::endl;
		return;
	}

//	mSocket = socket;
	mSender = sender;
	mGrabTimer->start(10);
	mSendTimer->start(40);
	std::cout << "Started streaming from GS device" << std::endl;
}

void ImageSenderGE::stopStreaming()
{
	if (!mGrabTimer || !mSendTimer)
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

	vtkSmartPointer<vtkImageData> imgStream = mGEStreamer.GetNewFrame();
	if(!imgStream)
	{
		std::cout << "ImageSenderGE::grab(): No image from GEStreamer" << std::endl;
		return;
	}
	else
	{
//		std::cout << "ImageSenderGE::grab(): Got image from GEStreamer" << std::endl;
	}
	//Only set image and time if we got a new image
	mImgStream = imgStream;
	mLastGrabTime = mGEStreamer.GetTimeStamp();

	//Will only work with scanner, not simple test data
	/*if (mGEStreamer.frame)
		mLastGrabTime = mGEStreamer.frame->GetTimeStamp();
	else
	{
		std::cout << "ImageSenderGE::grab(): No mGEStreamer.frame. Could not get timestamp" << std::endl;
	}*/
}

void ImageSenderGE::send()
{
	if (!mSender || !mSender->isReady())
		return;

	IGTLinkImageMessage::Pointer imgMsg = this->getImageMessage();
	if (!imgMsg)
		return;

	mSender->send(this->getImageMessage());

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

	int* size = mGEStreamer.VolumeDimensions; // May be 3 dimensions
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
			IGTLinkImageMessage::TYPE_UINT8;// scalar type
		}
	}
	if (scalarType == -1)
	{
		std::cerr << "unknown image type" << std::endl;
		exit(0);
	}

	retval->SetDimensions(size); // May be 3 dimensions
	retval->SetSpacing(mGEStreamer.VoxelSize[0], mGEStreamer.VoxelSize[1], mGEStreamer.VoxelSize[2]); // May be 3 dimensions
	retval->SetScalarType(scalarType);
	retval->SetDeviceName("ImageSenderGE");
	retval->SetSubVolume(size, offset);
	retval->AllocateScalars();

	//TODO: get timestamp from GEStreamer
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

	retval->SetOrigin(mImgStream->GetOrigin()[0], mImgStream->GetOrigin()[1], mImgStream->GetOrigin()[2]);//Is origin set in mImgStream? No...

	//Set image data
	int fsize = retval->GetImageSize();
	memcpy(retval->GetScalarPointer(), mImgStream->GetScalarPointer(), fsize);

	return retval;
}

}// namespace cx

#endif //CX_USE_ISB_GE
