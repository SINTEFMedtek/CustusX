#include "cxGEStreamer.h"

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
#include "cxCommandlineImageStreamerFactory.h"

typedef vtkSmartPointer<vtkImageFlip> vtkImageFlipPtr;

namespace cx
{

GEStreamer::GEStreamer() :
	mGrabTimer(0)
{
	mRenderTimer.reset(new CyclicActionLogger("GE Grabber Timer"));

	this->setSendInterval(40);

	mImgExportedStream = vtkSmartPointer<data_streaming::vtkExportedStreamData>();

	mGrabTimer = new QTimer(this);
	connect(mGrabTimer, SIGNAL(timeout()), this, SLOT(grab())); // this signal will be executed in the thread of THIS, i.e. the main thread.
	mSendTimer = new QTimer(this);
}

QString GEStreamer::getType()
{
	return "ISB_GE";
}

void GEStreamer::applyOptions()
{
	std::string fileRoot = "c:\\test";
	bool dumpHdfToDisk = false;

	data_streaming::InterpolationType interpType = data_streaming::Bilinear;

   	int bufferSize = mOptions.bufferSize;

	data_streaming::OutputSizeComputationType imageCompType = data_streaming::AUTO;

	long imageSize = -1;
	if(mOptions.computationType != data_streaming::AUTO)
	{
		imageSize = mOptions.imageSize;
	}

	bool useOpenCL = mOptions.useOpenCL;

	std::string openclpath = findGEOpenCLKernels("").toStdString();

	mGEStreamer.InitializeClientData(fileRoot, dumpHdfToDisk, imageCompType, imageSize, interpType, bufferSize, openclpath, useOpenCL);

	//Setup the needed data stream types. The default is only scan converted data
	mGEStreamer.SetupExportParameters(mOptions.scanconvertedStream, mOptions.tissueStream, mOptions.bandwidthStream, mOptions.frequencyStream, mOptions.velocityStream);

	//Prevent copies of streamed data. Without this both tissue and flow frames trigger sending of data (resulting in all frames sent 2 times?)
	mGEStreamer.SetForceTissueFrameRate(true);
}

QString findGEOpenCLKernels(QString additionalLocation)
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

bool GEStreamer::initialize_local()
{
	std::string hostIp = mOptions.IP;
	int streamPort = mOptions.streamPort;
	int commandPort = mOptions.commandPort;
	data_streaming::TestMode test;
	QString testMode(mOptions.testMode.c_str());
	if (testMode.compare("2D", Qt::CaseInsensitive) == 0)
		test = data_streaming::test2D;
	else if (testMode.compare("1", Qt::CaseInsensitive) == 0) //Also accept 1 as 2D test
		test = data_streaming::test2D;
	else if (testMode.compare("3D", Qt::CaseInsensitive) == 0)
		test = data_streaming::test3D;
	else //no
		test = data_streaming::noTest;

	this->applyOptions();

	return mGEStreamer.ConnectToScanner(hostIp, streamPort, commandPort, test);
}

void GEStreamer::deinitialize_local()
{
	//Set mImgStream as an empty pointer
	mImgExportedStream = vtkSmartPointer<data_streaming::vtkExportedStreamData>();

	//Clear frame geometry
	data_streaming::frame_geometry emptyGeometry;
	mFrameGeometry = emptyGeometry;
	mFlowGeometry = emptyGeometry;
	mGEStreamer.DisconnectFromScanner();
}

void GEStreamer::setOptions(const Options& options)
{
	mOptions = options;
	this->applyOptions();
}

bool GEStreamer::startStreaming(SenderPtr sender)
{
	this->setInitialized(this->initialize_local());

	if (!this->isInitialized() || !mGrabTimer || !mSendTimer)
	{
		std::cout << "GEStreamer: Failed to start streaming: Not initialized." << std::endl;
		return false;
	}

	mSender = sender;
	mGrabTimer->start(5);
	std::cout << "Started streaming from GS device" << std::endl;
	return true;
}

void GEStreamer::stopStreaming()
{
	if (!this->isInitialized() || !mGrabTimer || !mSendTimer)
		return;
	mGrabTimer->stop();
	mSender.reset();

	this->deinitialize_local();
}

void GEStreamer::streamSlot()
{
	std::cout << "GEStreamer::streamSlot()" << std::endl;
}

void GEStreamer::grab()
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
}

void GEStreamer::send()
{
	if (!this->isReadyToSend())
		return;

	QString uid;
	if (mOptions.scanconvertedStream && mImgExportedStream->GetScanConvertedImage())
	{
		uid = "ScanConverted [RGBA]";
		send(uid, mImgExportedStream->GetScanConvertedImage(), mFrameGeometry, mFrameGeometryChanged);
	}
	if (mOptions.tissueStream && mImgExportedStream->GetTissueImage())
	{
		uid = "Tissue [R]";
		send(uid, mImgExportedStream->GetTissueImage(), mFrameGeometry, mFrameGeometryChanged);
	}
	if (mOptions.bandwidthStream && mImgExportedStream->GetBandwidthImage())
	{
		uid = "Bandwidth [R]";
		send(uid, mImgExportedStream->GetBandwidthImage(), mFlowGeometry, mFlowGeometryChanged);
	}
	if (mOptions.frequencyStream && mImgExportedStream->GetFrequencyImage())
	{
		uid = "Frequency [R]";
		send(uid, mImgExportedStream->GetFrequencyImage(), mFlowGeometry, mFlowGeometryChanged);
	}
	if (mOptions.velocityStream && mImgExportedStream->GetVelocityImage())
	{
		uid = "Velocity [R]";
		send(uid, mImgExportedStream->GetVelocityImage(), mFlowGeometry, mFlowGeometryChanged);
	}
}


void GEStreamer::send(const QString& uid, const vtkImageDataPtr& img, data_streaming::frame_geometry geometry, bool geometryChanged)
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

ProbeDefinitionPtr GEStreamer::getFrameStatus(QString uid, data_streaming::frame_geometry geometry, vtkSmartPointer<vtkImageData> img)
{
	ProbeDefinitionPtr retval;
	if (!img || !mImgExportedStream)
		return retval;

	// 1 = sector, 2 = linear
	if (geometry.imageType == data_streaming::Linear) //linear
		retval = ProbeDefinitionPtr( new ProbeDefinition(ProbeDefinition::tLINEAR));
	else //sector
		retval = ProbeDefinitionPtr( new ProbeDefinition(ProbeDefinition::tSECTOR));

	double inputDepthStart = geometry.origin[1] * 1000;//m -> mm
	double yImageOrigin = inputDepthStart * cos(geometry.width/2.0);
	double correctDepthStart = geometry.depthStart - inputDepthStart;
	double correctDepthEnd = geometry.depthEnd - inputDepthStart;

	// Set start and end of sector in mm from origin
	// Set width of sector in mm for LINEAR, width of sector in radians for SECTOR.
	retval->setSector(correctDepthStart, correctDepthEnd, geometry.width);

	retval->setOrigin_p(Vector3D(geometry.origin[0]*1000 / img->GetSpacing()[0] + img->GetOrigin()[0],
								 //geometry.origin[1]*1000 / img->GetSpacing()[1] + img->GetOrigin()[1],
								 yImageOrigin / img->GetSpacing()[1] + img->GetOrigin()[1],
			/*geometry.origin[2]*1000 +*/ img->GetOrigin()[2]));
	retval->setSize(QSize(img->GetDimensions()[0], img->GetDimensions()[1]));
	retval->setSpacing(Vector3D(img->GetSpacing()));
	retval->setClipRect_p(DoubleBoundingBox3D(img->GetExtent()));

	retval->setUid(uid);

	return retval;
}

bool GEStreamer::equal(data_streaming::frame_geometry a, data_streaming::frame_geometry b)
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
void GEStreamer::printTimeIntervals()
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
