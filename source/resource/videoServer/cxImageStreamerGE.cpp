/*
 * cxImageStreamerGE.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: olevs
 */

#include "cxImageStreamerGE.h"

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
#include "cxReporter.h"
#include "cxTypeConversions.h"
#include "cxDataLocations.h"
#include "geConfig.h"
#include "vtkImageChangeInformation.h"
#include "vtkForwardDeclarations.h"

typedef vtkSmartPointer<vtkImageFlip> vtkImageFlipPtr;

namespace cx
{

ImageStreamerGE::ImageStreamerGE() :
	mGrabTimer(0),
	mExportScanconverted(true),
	mExportTissue(false),
	mExportBandwidth(false),
	mExportFrequency(false),
	mExportVelocity(false)
{
	//data_streaming::DataStreamApp test;
	mRenderTimer.reset(new CyclicActionLogger("GE Grabber Timer"));

	this->setSendInterval(40);

	mImgExportedStream = vtkSmartPointer<data_streaming::vtkExportedStreamData>();

	mGrabTimer = new QTimer(this);
	connect(mGrabTimer, SIGNAL(timeout()), this, SLOT(grab())); // this signal will be executed in the thread of THIS, i.e. the main thread.
	mSendTimer = new QTimer(this);
}

QString ImageStreamerGE::getType()
{
	return "ISB_GE";
}

QStringList ImageStreamerGE::getArgumentDescription()
{
	QStringList retval;
	//Tabs are set so that tool tip looks nice
	retval << "--ip:		GE scanner IP address";//default = 127.0.0.1, find a typical direct link address
	retval << "--streamport:		GE scanner streaming port, default = 6543";
	retval << "--commandport:	GE scanner command port, default = -1";//Unnecessary for us?
	retval << "--buffersize:		Size of GEStreamer buffer, default = 10";
	retval << "--imagesize:		Returned image/volume size in pixels (eg. 500x500x1), default = auto";
	retval << "--isotropic:		Use cubic voxels for the scan conversion, default = no";
	retval << "--openclpath:		Path to ScanConvert.cl";
	retval << "--test:		GEStreamer test mode (no, 2D or 3D), default = no";
	retval << "--useOpenCL:		Use OpenCL for scan conversion, default = 1";
	retval << "--streams:		Used video streams (separated by , with no spaces), default = scanconverted,bandwidth  Available streams (only 2D for now): scanconverted,tissue,bandwidth,frequency,velocity (all)";
	return retval;
}

void ImageStreamerGE::initialize(StringMap arguments)
{
	CommandLineStreamer::initialize(arguments);

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
    if (!mArguments.count("isotropic"))
        mArguments["isotropic"] = "no";
    if (!mArguments.count("useOpenCL"))
        mArguments["useOpenCL"] = "1";
    if (!mArguments.count("streams"))
        mArguments["streams"] = "scanconverted,bandwidth";

   	int bufferSize = convertStringWithDefault(mArguments["buffersize"], -1);

	data_streaming::OutputSizeComputationType imageCompType = data_streaming::AUTO;
   	long imageSize = -1;// -1 = auto
	if (!(mArguments["imagesize"].compare("auto", Qt::CaseInsensitive) == 0))
	{
		if (mArguments["isotropic"].compare("yes", Qt::CaseInsensitive) == 0)
			imageCompType = data_streaming::ISOTROPIC;
		else
			imageCompType = data_streaming::ANISOTROPIC;
		imageSize = 1;
	   	QStringList sizeList = QString(mArguments["imagesize"]).split(QRegExp("[x,X,*]"), QString::SkipEmptyParts);
		for (int i = 0; i < sizeList.length(); i++)
		{
			int dimSize = convertStringWithDefault(sizeList.at(i), 1);
			imageSize *= dimSize;
		}
		if (imageSize <= 1)
		{
			reportError("Error with calculated image size. imagesize: " + mArguments["imagesize"] + " = " + qstring_cast(imageSize));
		}
	}
	else
		imageCompType = data_streaming::AUTO;

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
			reportWarning("ImageStreamerGE: Unknown stream: " + streamList.at(i));
   	}

	bool useOpenCL = convertStringWithDefault(mArguments["useOpenCL"], 1);

	std::string openclpath = findOpenCLPath(mArguments["openclpath"]).toStdString();

	mGEStreamer.InitializeClientData(fileRoot, dumpHdfToDisk, imageCompType, imageSize, interpType, bufferSize, openclpath, useOpenCL);

	//Setup the needed data stream types. The default is only scan converted data
	mGEStreamer.SetupExportParameters(mExportScanconverted, mExportTissue, mExportBandwidth, mExportFrequency, mExportVelocity);

	//Prevent copies of streamed data. Without this both tissue and flow frames trigger sending of data (resulting in all frames sent 2 times?)
	mGEStreamer.SetForceTissueFrameRate(true);
}

QString findOpenCLPath(QString additionalLocation)
{
	//Look in arg in, GEStreamer source dir, and installed dir
	QString retval;
	QStringList paths;
	paths << additionalLocation << GEStreamer_KERNEL_PATH << DataLocations::getShaderPath();
	QFileInfo path;
	path = QFileInfo(paths[0] + QString("/ScanConvertCL.cl"));
	if (!path.exists())
		path = QFileInfo(paths[1] + QString("/ScanConvertCL.cl"));
	if (!path.exists())
		path = QFileInfo(paths[2] + "/ScanConvertCL.cl");
	if (!path.exists())
	{
		reportWarning("Error: Can't find ScanConvertCL.cl in any of\n  " + paths.join("  \n"));
	}
	else
		retval = path.absolutePath();

	return retval;
}

void ImageStreamerGE::deinitialize_local()
{
	//Set mImgStream as an empty pointer
	mImgExportedStream = vtkSmartPointer<data_streaming::vtkExportedStreamData>();

	//Clear frame geometry
	data_streaming::frame_geometry emptyGeometry;
	mFrameGeometry = emptyGeometry;
	mFlowGeometry = emptyGeometry;
	mGEStreamer.DisconnectFromScanner();
}

bool ImageStreamerGE::initialize_local()
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
}

bool ImageStreamerGE::startStreaming(SenderPtr sender)
{
	this->setInitialized(this->initialize_local());

	if (!this->isInitialized() || !mGrabTimer || !mSendTimer)
	{
		std::cout << "ImageStreamerGE: Failed to start streaming: Not initialized." << std::endl;
		return false;
	}

	mSender = sender;
	mGrabTimer->start(5);
	std::cout << "Started streaming from GS device" << std::endl;
	return true;
}

void ImageStreamerGE::stopStreaming()
{
	if (!this->isInitialized() || !mGrabTimer || !mSendTimer)
		return;
	mGrabTimer->stop();
	mSender.reset();

	this->deinitialize_local();
}

void ImageStreamerGE::grab()
{
	if (!mGEStreamer.HasNewImageData())
		return;
	mRenderTimer->begin();

	vtkSmartPointer<data_streaming::vtkExportedStreamData> imgExportedStream = mGEStreamer.GetExportedStreamDataAndMoveToNextFrame();

	mRenderTimer->time("scanc");

	//Get new image
	if(!imgExportedStream)
	{
		std::cout << "ImageStreamerGE::grab(): No image from GEStreamer" << std::endl;
		return;
	}

	//Get frame geometry if we don't have it yet
	if (!equal(mFrameGeometry, imgExportedStream->GetTissueGeometry()))
	{
		// Frame geometry have changed.
		mFrameGeometry = imgExportedStream->GetTissueGeometry();
		mFrameGeometryChanged = true;
	}
	else
		mFrameGeometryChanged = false;

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


	this->send();

	mRenderTimer->time("sent");

//	this->printTimeIntervals();
}

void ImageStreamerGE::send()
{
	if (!this->isReadyToSend())
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


void ImageStreamerGE::send(const QString& uid, const vtkImageDataPtr& img, data_streaming::frame_geometry geometry, bool geometryChanged)
{
	mRenderTimer->time("startsend");

	if (geometryChanged)
	{
		ProbeDefinitionPtr frameMessage( getFrameStatus(uid, geometry, img));
		PackagePtr package(new Package());
		package->mProbe = frameMessage;
		mSender->send(package);
		std::cout << uid << " Nyquist " << geometry.vNyquist << std::endl;
		int*  dim = img->GetDimensions();
		std::cout << uid << " Volume size: " << dim[0] << " " << dim[1] << " " << dim[2] << std::endl;

	}

	// CustusX does not handle nonzero origin - set to zero, but AFTER getFrameStatus() is called.
	vtkImageChangeInformationPtr center = vtkImageChangeInformationPtr::New();
	center->SetInputData(img);
	center->SetOutputOrigin(0,0,0);
	center->Update();
	mRenderTimer->time("orgnull");

	ImagePtr message(new Image(uid, center->GetOutput()));
	mRenderTimer->time("createimg");

	PackagePtr package(new Package());
	package->mImage = message;
	mSender->send(package);
	mRenderTimer->time("sendersend");
}

ProbeDefinitionPtr ImageStreamerGE::getFrameStatus(QString uid, data_streaming::frame_geometry geometry, vtkSmartPointer<vtkImageData> img)
{
	ProbeDefinitionPtr retval;
	if (!img || !mImgExportedStream)
		return retval;

	// 1 = sector, 2 = linear
	if (geometry.imageType == data_streaming::Linear) //linear
		retval = ProbeDefinitionPtr( new ProbeDefinition(ProbeDefinition::tLINEAR));
	else //sector
		retval = ProbeDefinitionPtr( new ProbeDefinition(ProbeDefinition::tSECTOR));

//		std::cout << "Geometry origin: " << geometry.origin[0] << " " << geometry.origin[1] << " " << geometry.origin[2] << std::endl;
//		std::cout << "Image origin: " << img->GetOrigin()[0] << " " << img->GetOrigin()[1] << " " << img->GetOrigin()[2] << std::endl;

        double inputDepthStart = geometry.origin[1] * 1000;//m -> mm
        double yImageOrigin = inputDepthStart * cos(geometry.width/2.0);
        double correctDepthStart = geometry.depthStart - inputDepthStart;
        double correctDepthEnd = geometry.depthEnd - inputDepthStart;


//        std::cout << "width: " << geometry.width << std::endl;
//        std::cout << "inputDepthStart: " << inputDepthStart << " mm" << std::endl;
//        std::cout << "yImageOrigin: " << yImageOrigin << " mm" << std::endl;
//		std::cout << "correctDepthStart: " << correctDepthStart << std::endl;
//		std::cout << "correctDepthEnd: " << correctDepthEnd << std::endl;

	// Set start and end of sector in mm from origin
		// Set width of sector in mm for LINEAR, width of sector in radians for SECTOR.
//    retval->setSector(geometry.depthStart, geometry.depthEnd, geometry.width);
		retval->setSector(correctDepthStart, correctDepthEnd, geometry.width);

//	retval->setOrigin_p(Vector3D(geometry.origin[0] + img->GetOrigin()[0],
//					geometry.origin[1]+ img->GetOrigin()[1],
//					geometry.origin[2]+ img->GetOrigin()[2]));
        retval->setOrigin_p(Vector3D(geometry.origin[0]*1000 / img->GetSpacing()[0] + img->GetOrigin()[0],
                                     //geometry.origin[1]*1000 / img->GetSpacing()[1] + img->GetOrigin()[1],
                                     yImageOrigin / img->GetSpacing()[1] + img->GetOrigin()[1],
                /*geometry.origin[2]*1000 +*/ img->GetOrigin()[2]));
	retval->setSize(QSize(img->GetDimensions()[0], img->GetDimensions()[1]));
	retval->setSpacing(Vector3D(img->GetSpacing()));
	retval->setClipRect_p(DoubleBoundingBox3D(img->GetExtent()));

	retval->setUid(uid);

//	std::cout << "depthStart: " << geometry.depthStart << " depthEnd: " << geometry.depthEnd << std::endl;
//	std::cout << "Origin: " << retval->getOrigin_p() << std::endl;

	return retval;
}

bool ImageStreamerGE::equal(data_streaming::frame_geometry a, data_streaming::frame_geometry b)
{
	return !((a.origin[0] != b.origin[0]) || (a.origin[1] != b.origin[1]) || (a.origin[2] != b.origin[2])
			|| (a.imageType != b.imageType)
			|| !similar(a.depthStart, b.depthStart, 0.01)
			|| !similar(a.depthEnd, b.depthEnd, 0.01)
			|| !similar(a.width, b.width, 0.0001)
			|| !similar(a.tilt, b.tilt, 0.0001)
			|| !similar(a.elevationWidth, b.elevationWidth, 0.0001)
			|| !similar(a.elevationTilt, b.elevationTilt, 0.0001)
			|| !similar(a.vNyquist, b.vNyquist, 0.0001)
			|| !similar(a.PRF, b.PRF, 0.0001));
}
void ImageStreamerGE::printTimeIntervals()
{
	if (mRenderTimer->intervalPassed())
	{
        static int counter=0;
        if (++counter%3==0)
			reporter()->sendDebug(mRenderTimer->dumpStatisticsSmall());
	          std::cout << mRenderTimer->dumpStatisticsSmall() << std::endl;
        mRenderTimer->reset();
	}
}

}// namespace cx

#endif //CX_USE_ISB_GE
