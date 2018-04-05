/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxImageStreamerOpenCV.h"
#include "cxConfig.h"

#include <QCoreApplication>
#include <QTimer>
#include <QTime>
#include <QHostAddress>
#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vtkMetaImageReader.h"
#include "vtkImageImport.h"
#include "vtkLookupTable.h"
#include "vtkImageMapToColors.h"
#include "vtkMetaImageWriter.h"

#include "cxTypeConversions.h"
#include "cxCommandlineImageStreamerFactory.h"
#include "cxStringHelpers.h"
#include "cxDoubleProperty.h"
#include "cxBoolProperty.h"
#include "cxLogger.h"
#include "cxVideoServerConfig.h"

#ifdef CX_USE_OpenCV
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#endif

namespace cx
{

std::vector<PropertyPtr> ImageStreamerOpenCVArguments::getSettings(QDomElement root)
{
	std::vector<PropertyPtr> retval;
	retval.push_back(this->getVideoPortOption(root));
	retval.push_back(this->getPrintPropertiesOption(root));
	return retval;
}

DoublePropertyBasePtr ImageStreamerOpenCVArguments::getVideoPortOption(QDomElement root)
{
	DoublePropertyPtr retval;
	retval = DoubleProperty::initialize("videoport", "Video Port", "Select video source as an integer from 0 and up.", 0, DoubleRange(0, 10, 1), 0, root);
	retval->setGuiRepresentation(DoublePropertyBase::grSPINBOX);
	retval->setGroup("OpenCV");
	return retval;
}

BoolPropertyBasePtr ImageStreamerOpenCVArguments::getPrintPropertiesOption(QDomElement root)
{
	BoolPropertyPtr retval;
	bool defaultValue = false;
	retval = BoolProperty::initialize("properties", "Print Properties",
											"When starting OpenCV, print properties to console",
											defaultValue, root);
	retval->setAdvanced(true);
	retval->setGroup("OpenCV");
	return retval;
}

StringMap ImageStreamerOpenCVArguments::convertToCommandLineArguments(QDomElement root)
{
	StringMap retval;
	retval["--type"] = "OpenCV";
	retval["--videoport"] = this->getVideoPortOption(root)->getValueAsVariant().toString();
	if (this->getPrintPropertiesOption(root)->getValue())
		retval["--properties"] = "1";
	return retval;
}

QStringList ImageStreamerOpenCVArguments::getArgumentDescription()
{
	QStringList retval;
#ifdef CX_USE_OpenCV
	retval << "--videoport:		video id,     default=0";
	retval << "--in_width:		width of incoming image, default=camera";
	retval << "--in_height:		height of incoming image, default=camera";
	retval << "--out_width:		width of outgoing image, default=camera";
	retval << "--out_height:		width of outgoing image, default=camera";
	retval << "--properties:		dump image properties";
#endif
	return retval;
}

} // namespace cx

//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------

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
ImageStreamerOpenCV::ImageStreamerOpenCV()
{
	mGrabbing = false;
	mAvailableImage = false;
	setSendInterval(40);

#ifdef CX_USE_OpenCV
	mVideoCapture.reset(new cv::VideoCapture());
#endif
	mSendTimer = new QTimer(this);
	connect(mSendTimer, SIGNAL(timeout()), this, SLOT(send())); // this signal will be executed in the thread of THIS, i.e. the main thread.
}

ImageStreamerOpenCV::~ImageStreamerOpenCV()
{
	this->deinitialize_local();
}

QString ImageStreamerOpenCV::getType()
{
	return "OpenCV";
}

QStringList ImageStreamerOpenCV::getArgumentDescription()
{
	return ImageStreamerOpenCVArguments().getArgumentDescription();
}


void ImageStreamerOpenCV::initialize(StringMap arguments)
{
	CommandLineStreamer::initialize(arguments);
}

void ImageStreamerOpenCV::deinitialize_local()
{
#ifdef CX_USE_OpenCV
	while (mGrabbing) // grab() method seems to call processmessages itself...
		qApp->processEvents();
	mVideoCapture->release();
	mVideoCapture.reset(new cv::VideoCapture());
#endif
}

void ImageStreamerOpenCV::initialize_local()
{
#ifdef CX_USE_OpenCV

	if (!mArguments.count("videoport"))
		mArguments["videoport"] = "0";
	if (!mArguments.count("out_width"))
		mArguments["out_width"] = "";
	if (!mArguments.count("out_height"))
		mArguments["out_height"] = "";
    if (!mArguments.count("in_width"))
        mArguments["in_width"] = "";
    if (!mArguments.count("in_height"))
        mArguments["in_height"] = "";

	QString videoSource = mArguments["videoport"];
	int videoport = convertStringWithDefault(mArguments["videoport"], 0);

	bool sourceIsInt = false;
	videoSource.toInt(&sourceIsInt);

	if (sourceIsInt){
	    // open device (camera)
		mVideoCapture->open(videoport);
	}
	else{
        // open file
		mVideoCapture->open(videoSource.toStdString().c_str());
	}

	if (!mVideoCapture->isOpened())
	{
		cerr << "ImageStreamerOpenCV: Failed to open a video device or video file!\n" << endl;
		return;
	}

	// try one grab before accepting the streamer
	// - this fails if no camera is attached
	try
	{
		mVideoCapture->grab();
	}
	catch(cv::Exception& e)
	{
		CX_LOG_ERROR() << "OpenCV failed with message: " << e.what();
		mVideoCapture->release();
		return;
	}


	{
		//determine default values
		int default_width = mVideoCapture->get(CV_CAP_PROP_FRAME_WIDTH);
		int default_height = mVideoCapture->get(CV_CAP_PROP_FRAME_HEIGHT);

		//set input size
		int in_width = convertStringWithDefault(mArguments["in_width"], default_width);
		int in_height = convertStringWithDefault(mArguments["in_height"], default_height);
		mVideoCapture->set(CV_CAP_PROP_FRAME_WIDTH, in_width);
		mVideoCapture->set(CV_CAP_PROP_FRAME_HEIGHT, in_height);

		//set output size (resize)
		int out_width = convertStringWithDefault(mArguments["out_width"], in_width);
		int out_height = convertStringWithDefault(mArguments["out_height"], in_height);
		mRescaleSize.setWidth(out_width);
		mRescaleSize.setHeight(out_height);

		if (mArguments.count("properties"))
			this->dumpProperties();

		std::cout << "ImageStreamerOpenCV: Started streaming from openCV device "
			<< videoSource.toStdString()
			<< ", size=(" << in_width << "," << in_height << ")";
		if (( in_width!=mRescaleSize.width() )|| (in_height!=mRescaleSize.height()))
			std::cout << ". Scaled to (" << mRescaleSize.width() << "," << mRescaleSize.height() << ")";

		std::cout << std::endl;
	}
#endif
}

void ImageStreamerOpenCV::startStreaming(SenderPtr sender)
{
#ifdef CX_USE_OpenCV
	this->initialize_local();

	if (!mSendTimer || !mVideoCapture->isOpened())
	{
		reportError("ImageStreamerOpenCV: Failed to start streaming: Not initialized.");
		return;
	}

	mSender = sender;
	mSendTimer->start(getSendInterval());
	this->continousGrabEvent(); // instead of grabtimer

#else
	reportWarning("ImageStreamerOpenCV: Failed to start streaming: CX_USE_OpenCV not defined.");
#endif //CX_USE_OpenCV
}

void ImageStreamerOpenCV::stopStreaming()
{
	if (!mSendTimer)
		return;
	mSendTimer->stop();
	mSender.reset();

	this->deinitialize_local();
}

bool ImageStreamerOpenCV::isStreaming()
{
	return (mSendTimer && mVideoCapture->isOpened());
}

void ImageStreamerOpenCV::dumpProperties()
{
#ifdef CX_USE_OpenCV
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
#endif
}

void ImageStreamerOpenCV::dumpProperty(int val, QString name)
{
#ifdef CX_USE_OpenCV
	double value = mVideoCapture->get(val);
	if (value != -1)
		std::cout << "Property " << name.toStdString() << " : " << mVideoCapture->get(val) << std::endl;
#endif
}

/**
 * Grab image, then post an event asking for a new grab.
 *
 * This is an alternative to using a QTimer with timeout 0.
 */
void ImageStreamerOpenCV::continousGrabEvent()
{
	if (!mSendTimer->isActive())
		return;
	this->grab();
	QMetaObject::invokeMethod(this, "continousGrabEvent", Qt::QueuedConnection);
}

void ImageStreamerOpenCV::grab()
{
#ifdef CX_USE_OpenCV
	if (!mVideoCapture->isOpened())
	{
		return;
	}

	mGrabbing = true;
	// grab images from camera to opencv internal buffer, do not process
	bool success = mVideoCapture->grab();
	if (success)
		mLastGrabTime = QDateTime::currentDateTime();
	mAvailableImage = success;
	mGrabbing = false;
#endif
}

void ImageStreamerOpenCV::send()
{
	if (!mSender || !mSender->isReady())
		return;
	if (!mAvailableImage)
	{
//		reportDebug("dropped resend of frame");
		return;
	}
	PackagePtr package(new Package());
	package->mImage = this->getImageMessage();
	mSender->send(package);
	mAvailableImage = false;

//	static int counter=0;
//	if (++counter%50==0)
//		std::cout << "=== ImageStreamerOpenCV   send: " << start.msecsTo(QTime::currentTime()) << " ms" << std::endl;
}

ImagePtr ImageStreamerOpenCV::getImageMessage()
{
#ifdef CX_USE_OpenCV
	if (!mVideoCapture->isOpened())
		return ImagePtr();

	QTime start = QTime::currentTime();

	cv::Mat frame_source;
	//  mVideoCapture >> frame_source;
	if (!mVideoCapture->retrieve(frame_source, 0))
		return ImagePtr();

	if (this->thread() == QCoreApplication::instance()->thread() && !mSender)
	{
		cv::imshow("ImageStreamerOpenCV", frame_source);
	}

	cv::Mat frame = frame_source;
	if (( frame.cols!=mRescaleSize.width() )|| (frame.rows!=mRescaleSize.height()))
	{
		cv::resize(frame_source, frame, cv::Size(mRescaleSize.width(), mRescaleSize.height()), 0, 0, CV_INTER_LINEAR);
	}

	vtkImageDataPtr raw = this->convertTovtkImageData(frame);
	ImagePtr image(new Image("openCV", raw));
	image->setAcquisitionTime(mLastGrabTime);
	return image;
#else
	return ImagePtr();
#endif
}

vtkImageDataPtr ImageStreamerOpenCV::convertTovtkImageData(cv::Mat& frame)
{
	vtkImageDataPtr retval = vtkImageDataPtr::New();

	Eigen::Array3i dim(frame.cols, frame.rows, 1);
//	Eigen::Array3f spacing(1,1);
	retval->SetDimensions(dim.data());
	retval->SetSpacing(1,1,1);

	int dataType = -1;

//	if (frame.channels() == 3 || frame.channels() == 4) // dropped support for alpha - dont think openCV uses this.
	if (frame.channels() == 3)
	{
		dataType = VTK_UNSIGNED_CHAR;
	}
	else if (frame.channels() == 1)
	{
		// dropped support: must iterate over using shorts later on.
//		if (frame.depth() == 16)
//		{
//			dataType = VTK_UNSIGNED_SHORT;
//		}
		if (frame.depth() == 8)
		{
			dataType = VTK_UNSIGNED_CHAR;
		}
	}

	if (dataType == -1)
	{
		std::cerr << "unknown image type" << std::endl;
		return vtkImageDataPtr();
	}

	retval->AllocateScalars(dataType, frame.channels());

	//------------------------------------------------------------
	// Create a new IMAGE type message
//	IGTLinkImageMessage::Pointer imgMsg = IGTLinkImageMessage::New();
//	imgMsg->SetDimensions(size);
//	imgMsg->SetSpacing(spacingF);
//	imgMsg->SetScalarType(scalarType);
//	imgMsg->SetSubVolume(svsize, svoffset);
//	imgMsg->AllocateScalars();
//	imgMsg->SetTimeStamp(timestamp);

	unsigned char* dest = reinterpret_cast<unsigned char*> (retval->GetScalarPointer());
	uchar* src = frame.data;
	int N = frame.rows * frame.cols;
//	QString colorFormat;

	if (frame.channels() == 3)
	{
		if (frame.isContinuous())
		{
			// 3-channel continous colors
			for (int i = 0; i < N; ++i)
			{
//				*destPtr++ = 255;
				*dest++ = src[2]; // R
				*dest++ = src[1]; // G
				*dest++ = src[0]; // B
				src += 3;
			}
		}
		else
		{
//			std::cout << "noncontinous conversion, rows=" << size[1] << std::endl;
			for (int i=0; i<dim[1]; ++i)
			{
				 const uchar* src = frame.ptr<uchar>(i);
				 for (int j=0; j<dim[0]; ++j)
				 {
//						*destPtr++ = 255;
						*dest++ = src[2];
						*dest++ = src[1];
						*dest++ = src[0];
						src += 3;
				 }
			}
		}
//		colorFormat = "ARGB";
	}
	if (frame.channels() == 1)
	{
		if (!frame.isContinuous())
		{
			std::cout << "Error: Non-continous frame data." << std::endl;
			return vtkImageDataPtr();
		}

		// BW camera
		for (int i = 0; i < N; ++i)
		{
			*dest++ = *src++;
		}
//		colorFormat = "R";
	}

	return retval;

}

//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------

}
