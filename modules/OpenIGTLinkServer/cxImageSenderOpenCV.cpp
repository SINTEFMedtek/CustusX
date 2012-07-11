/*
 * cxImageSenderOpenCV.cpp
 *
 *  \date Jun 21, 2011
 *      \author christiana
 */

#include "cxImageSenderOpenCV.h"

#ifdef USE_OpenCV

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

#include <opencv2/imgproc/imgproc.hpp>

namespace
{
//------------------------------------------------------------
// Function to generate random matrix.
void GetRandomTestMatrix(igtl::Matrix4x4& matrix)
{
	//float position[3];
	//float orientation[4];

	matrix[0][0] = 1.0;
	matrix[1][0] = 0.0;
	matrix[2][0] = 0.0;
	matrix[3][0] = 0.0;
	matrix[0][1] = 0.0;
	matrix[1][1] = -1.0;
	matrix[2][1] = 0.0;
	matrix[3][1] = 0.0;
	matrix[0][2] = 0.0;
	matrix[1][2] = 0.0;
	matrix[2][2] = 1.0;
	matrix[3][2] = 0.0;
	matrix[0][3] = 0.0;
	matrix[1][3] = 0.0;
	matrix[2][3] = 0.0;
	matrix[3][3] = 1.0;
}

}

namespace cx
{

QString ImageSenderOpenCV::getType()
{
	return "OpenCV";
}

QStringList ImageSenderOpenCV::getArgumentDescription()
{
	QStringList retval;
	retval << "--videoport:  video id,     default=0";
	retval << "--height:     image height, default=1024";
	retval << "--width:      image width,  default=768";
	retval << "--properties: dump image properties";
	return retval;
}

ImageSenderOpenCV::ImageSenderOpenCV(QObject* parent) :
	ImageSender(parent),
	mSocket(NULL),
	mSendTimer(0),
	mGrabTimer(0)
{
	mGrabTimer = new QTimer(this);
	connect(mGrabTimer, SIGNAL(timeout()), this, SLOT(grab())); // this signal will be executed in the thread of THIS, i.e. the main thread.
	//  mTimer->start(40);
//	mGrabTimer->start(0);
	//  mTimer->start(1200); // for test of the timeout feature
	//  mTimer->start(100);


	mSendTimer = new QTimer(this);
	connect(mSendTimer, SIGNAL(timeout()), this, SLOT(send())); // this signal will be executed in the thread of THIS, i.e. the main thread.
//	mSendTimer->start(40);
}

void ImageSenderOpenCV::initialize(StringMap arguments)
{
	mArguments = arguments;

	// Run an init/deinit to check that we have contact right away.
	// Do NOT keep the connection open: This is because we have no good way to
	// release resources if the server is a local app and is killed by CustusX.
	// This way, we can disconnect (thus releasing resources), and then safely
	// remove the usb cable without having dangling resources in openCV. (problem at least on Linux)
	this->initialize_local();
	this->deinitialize_local();
}

void ImageSenderOpenCV::deinitialize_local()
{
	mVideoCapture.release();
	mVideoCapture = cv::VideoCapture();
}

void ImageSenderOpenCV::initialize_local()
{
//	// if in main thread only (debug)
//	if (this->thread() == QCoreApplication::instance()->thread() && !mSocket)
//		cv::namedWindow("ImageSenderOpenCV", CV_WINDOW_KEEPRATIO); //resizable window;

	if (!mArguments.count("videoport"))
		mArguments["videoport"] = "0";
	if (!mArguments.count("width"))
		mArguments["width"] = "1024";
	if (!mArguments.count("height"))
		mArguments["height"] = "768";

	QString videoSource = mArguments["videoport"];
	int videoport = convertStringWithDefault(mArguments["videoport"], 0);
	int height = convertStringWithDefault(mArguments["height"], 768);
	int width = convertStringWithDefault(mArguments["width"], 1024);

	mVideoCapture.open(videoSource.toStdString().c_str());
	if (!mVideoCapture.isOpened()) //if this fails, try to open as a video camera, through the use of an integer param
		mVideoCapture.open(videoport);
	if (!mVideoCapture.isOpened())
	{
		cerr << "ImageSenderOpenCV: Failed to open a video device or video file!\n" << endl;
		return;
	}
	else
	{
		mVideoCapture.set(CV_CAP_PROP_FRAME_WIDTH, width);
		mVideoCapture.set(CV_CAP_PROP_FRAME_HEIGHT, height);

		if (mArguments.count("properties"))
			this->dumpProperties();

		std::cout << "ImageSenderOpenCV: Started streaming from openCV device "
			<< videoSource.toStdString()
			<< ", size=(" << width << "," << height << ")" << std::endl;
	}
}

void ImageSenderOpenCV::startStreaming(QTcpSocket* socket)
{
	this->initialize_local();

	if (!mGrabTimer || !mSendTimer)
	{
		std::cout << "ImageSenderOpenCV: Failed to start streaming: Not initialized." << std::endl;
		return;
	}

	mSocket = socket;
	mGrabTimer->start(0);
	mSendTimer->start(40);
}

void ImageSenderOpenCV::stopStreaming()
{
	if (!mGrabTimer || !mSendTimer)
		return;
	mGrabTimer->stop();
	mSendTimer->stop();
	mSocket = NULL;

	this->deinitialize_local();
}

void ImageSenderOpenCV::dumpProperties()
{
	this->dumpProperty(CV_CAP_PROP_POS_MSEC, "CV_CAP_PROP_POS_MSEC");
	this->dumpProperty(CV_CAP_PROP_POS_FRAMES, "CV_CAP_PROP_POS_FRAMES");
	this->dumpProperty(CV_CAP_PROP_POS_AVI_RATIO, "CV_CAP_PROP_POS_AVI_RATIO");
	this->dumpProperty(CV_CAP_PROP_FRAME_WIDTH, "CV_CAP_PROP_FRAME_WIDTH");
	this->dumpProperty(CV_CAP_PROP_FRAME_HEIGHT, "CV_CAP_PROP_FRAME_HEIGHT");
	this->dumpProperty(CV_CAP_PROP_FPS, "CV_CAP_PROP_FPS");
	this->dumpProperty(CV_CAP_PROP_FOURCC, "CV_CAP_PROP_FOURCC");
	this->dumpProperty(CV_CAP_PROP_FRAME_COUNT, "CV_CAP_PROP_FRAME_COUNT");
	this->dumpProperty(CV_CAP_PROP_FORMAT, "CV_CAP_PROP_FORMAT");
	this->dumpProperty(CV_CAP_PROP_MODE, "CV_CAP_PROP_MODE");
	this->dumpProperty(CV_CAP_PROP_BRIGHTNESS, "CV_CAP_PROP_BRIGHTNESS");
	this->dumpProperty(CV_CAP_PROP_CONTRAST, "CV_CAP_PROP_CONTRAST");
	this->dumpProperty(CV_CAP_PROP_SATURATION, "CV_CAP_PROP_SATURATION");
	this->dumpProperty(CV_CAP_PROP_HUE, "CV_CAP_PROP_HUE");
	this->dumpProperty(CV_CAP_PROP_GAIN, "CV_CAP_PROP_GAIN");
	this->dumpProperty(CV_CAP_PROP_EXPOSURE, "CV_CAP_PROP_EXPOSURE");
	this->dumpProperty(CV_CAP_PROP_CONVERT_RGB, "CV_CAP_PROP_CONVERT_RGB");
	//  this->dumpProperty(CV_CAP_PROP_WHITE_BALANCE, "CV_CAP_PROP_WHITE_BALANCE");
	this->dumpProperty(CV_CAP_PROP_RECTIFICATION, "CV_CAP_PROP_RECTIFICATION");
}

void ImageSenderOpenCV::dumpProperty(int val, QString name)
{
	double value = mVideoCapture.get(val);
	if (value != -1)
		std::cout << "Property " << name.toStdString() << " : " << mVideoCapture.get(val) << std::endl;
}

void ImageSenderOpenCV::grab()
{
	//  QTime start = QTime::currentTime();
	// grab images from camera to opencv internal buffer, do not process
	mVideoCapture.grab();
	mLastGrabTime = QDateTime::currentDateTime();
	//  std::cout << "   grab: " << start.msecsTo(QTime::currentTime()) << " ms" << std::endl;
}

void ImageSenderOpenCV::send()
{
	//std::cout << "tick" << std::endl;
	//  QTime start = QTime::currentTime();
	IGTLinkImageMessage::Pointer imgMsg = this->getImageMessage();

	if (mSocket)
	{
		//------------------------------------------------------------
		// Pack (serialize) and send
		imgMsg->Pack();
		mSocket->write(reinterpret_cast<const char*> (imgMsg->GetPackPointer()), imgMsg->GetPackSize());
		//  std::cout << "tick " << start.msecsTo(QTime::currentTime()) << " ms" << std::endl;
	}
	//  std::cout << "   send: " << start.msecsTo(QTime::currentTime()) << " ms" << std::endl;
}

IGTLinkImageMessage::Pointer ImageSenderOpenCV::getImageMessage()
{
	if (!mVideoCapture.isOpened())
		return IGTLinkImageMessage::Pointer();

	QTime start = QTime::currentTime();

	cv::Mat frame_source;
	//  mVideoCapture >> frame_source;
	if (!mVideoCapture.retrieve(frame_source, 0))
		return IGTLinkImageMessage::Pointer();

	if (this->thread() == QCoreApplication::instance()->thread() && !mSocket)
	{
		cv::imshow("ImageSenderOpenCV", frame_source);
	}

	//  std::cout << "grab" << start.msecsTo(QTime::currentTime()) << " ms" << std::endl;
	//  return igtl::ImageMessage::Pointer();

	igtl::TimeStamp::Pointer timestamp;
	timestamp = igtl::TimeStamp::New();
	//  double now = 1.0/1000*(double)QDateTime::currentDateTime().toMSecsSinceEpoch();
	double grabTime = 1.0 / 1000 * (double) mLastGrabTime.toMSecsSinceEpoch();
	timestamp->SetTime(grabTime);
	static QDateTime lastlastGrabTime = mLastGrabTime;
//	std::cout << "OpenCV stamp:\t" << mLastGrabTime.toString("hh:mm:ss.zzz") << std::endl;
	std::cout << "OpenCV diff:\t" <<lastlastGrabTime.msecsTo(mLastGrabTime) << "\tdelay:\t" << mLastGrabTime.msecsTo(QDateTime::currentDateTime()) << std::endl;
	lastlastGrabTime = mLastGrabTime;

	cv::Mat frame;
	// temporary HACK: all the old probe defs are for 800x600, continue this line for now:
	cv::resize(frame_source, frame, cv::Size(800, 600), 0, 0, CV_INTER_LINEAR);

	//  std::cout << "grab " << start.msecsTo(QTime::currentTime()) << " ms" << std::endl;
	//  std::cout << "WH=("<< frame.cols << "," << frame.rows << ")" << ", Channels,Depth=("<< frame.channels() << "," << frame.depth() << ")" << std::endl;

	if (!frame.isContinuous())
	{
		std::cout << "Error: Non-continous frame data." << std::endl;
		return IGTLinkImageMessage::Pointer();
	}

	int size[] =
	{ 1.0, 1.0, 1.0 }; // spacing (mm/pixel)
	size[0] = frame.cols;
	size[1] = frame.rows;

	float spacingF[] =
	{ 1.0, 1.0, 1.0 }; // spacing (mm/pixel)
	int* svsize = size;
	int svoffset[] =
	{ 0, 0, 0 }; // sub-volume offset
	int scalarType = -1;

	if (frame.channels() == 3 || frame.channels() == 4)
	{
		scalarType = IGTLinkImageMessage::TYPE_UINT32;// scalar type
	}
	else if (frame.channels() == 1)
	{
		if (frame.depth() == 16)
		{
			scalarType = IGTLinkImageMessage::TYPE_UINT16;// scalar type
		}
		else if (frame.depth() == 8)
		{
			scalarType = IGTLinkImageMessage::TYPE_UINT8;// scalar type
		}
	}

	if (scalarType == -1)
	{
		std::cerr << "unknown image type" << std::endl;
		exit(0);
	}
	//------------------------------------------------------------
	// Create a new IMAGE type message
	IGTLinkImageMessage::Pointer imgMsg = IGTLinkImageMessage::New();
	imgMsg->SetDimensions(size);
	imgMsg->SetSpacing(spacingF);
	imgMsg->SetScalarType(scalarType);
	imgMsg->SetDeviceName("cxOpenCVGrabber");
	imgMsg->SetSubVolume(svsize, svoffset);
	imgMsg->AllocateScalars();
	imgMsg->SetTimeStamp(timestamp);

	unsigned char* destPtr = reinterpret_cast<unsigned char*> (imgMsg->GetScalarPointer());
	uchar* src = frame.data;
	//  std::cout << "pre copy " << start.msecsTo(QTime::currentTime()) << " ms" << std::endl;
	int N = size[0] * size[1];

	if (frame.channels() >= 3)
	{
		for (int i = 0; i < N; ++i)
		{
			*destPtr++ = 255;
			*destPtr++ = src[2];
			*destPtr++ = src[1];
			*destPtr++ = src[0];
			src += 3;
		}
	}
	if (frame.channels() == 1)
	{ // BW camera
		for (int i = 0; i < N; ++i)
		{
			*destPtr++ = *src++;
		}
	}

	//------------------------------------------------------------
	// Get randome orientation matrix and set it.
	igtl::Matrix4x4 matrix;
	GetRandomTestMatrix(matrix);
	imgMsg->SetMatrix(matrix);

	//  std::cout << "   grab+process " << start.msecsTo(QTime::currentTime()) << " ms" << std::endl;

	return imgMsg;
}

//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------

}

#endif // USE_OpenCV
