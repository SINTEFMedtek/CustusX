#include "cxTestGEInterface.h"

#ifdef CX_USE_ISB_GE
#include <QTimer>
#include "GEStreamer.h"
#include "cxDirectlyLinkedSender.h"
#include "cxTestSenderController.h"
#include "cxImageStreamerGE.h"
#include "sscMessageManager.h"

#include "catch.hpp"

namespace cxtest
{

void TestGEInterface::setUp()
{
	cx::MessageManager::initialize();
}

void TestGEInterface::tearDown()
{
	cx::MessageManager::shutdown();
}

void TestGEInterface::testStream(cx::StringMap args)
{
	TestGEInterface::setUp();

	cx::StreamerPtr imageSender = cx::ImageStreamerFactory().getFromArguments(args);
	REQUIRE(imageSender);
	REQUIRE(imageSender->getType().compare(args["type"]) == 0);

	cx::DirectlyLinkedSenderPtr grabberBridge(new cx::DirectlyLinkedSender());

	TestSenderController controller(NULL);
	controller.initialize(grabberBridge);

	REQUIRE(imageSender->startStreaming(grabberBridge));

	QTimer::singleShot(1*1000,   qApp, SLOT(quit()) );
	qApp->exec();

	imageSender->stopStreaming();

	imageSender.reset();
	grabberBridge.reset();

	controller.verify();

	TestGEInterface::tearDown();
}

void TestGEInterface::testGEStreamer()
{
	std::cout << std::endl << "*** Test GE 3D scanconverted stream. GPU scanconversion  ***" << std::endl;
	data_streaming::GEStreamer geStreamer;

	std::string openclpath = cx::findOpenCLPath("").toStdString();

	//Initialize GEStreamer    HFDPath, useHDF, sizeCompType,     imgSize, interpolation,       buffSize, clPath, useCL
	geStreamer.InitializeClientData("", false, data_streaming::AUTO, -1, data_streaming::Bilinear, 10,   openclpath,     true);

	//Setup the needed data stream types. The default is only scan converted data
	geStreamer.SetupExportParameters(true, false, false, false, false);
//	geStreamer.SetupExportParameters(true, true, true, true);

	//                                         (hostIp, streamPort, commandPort, testMode));
	REQUIRE(geStreamer.ConnectToScanner("127.0.0.1", 6543,    -1,         data_streaming::test3D));
//	REQUIRE(geStreamer.ConnectToScanner("bhgrouter.hopto.org", 6543,    -1,         data_streaming::noTest));

	geStreamer.WaitForImageData();
	vtkSmartPointer<data_streaming::vtkExportedStreamData> imgExportedStream = geStreamer.GetExportedStreamDataAndMoveToNextFrame();
	REQUIRE(imgExportedStream);//Got image?

	// Frame geometry
	data_streaming::frame_geometry frameGeometry = imgExportedStream->GetTissueGeometry();
//	std::cout << "depthStart : " << frameGeometry.depthStart << std::endl;
//	std::cout << "depthEnd : " << frameGeometry.depthEnd << std::endl;
//	std::cout << "vNyquist : " << frameGeometry.vNyquist << std::endl;
//	std::cout << "width : " << frameGeometry.width << std::endl;
	//Test mode parameters
	REQUIRE(frameGeometry.depthEnd == 100);
	REQUIRE(frameGeometry.width == 1);

	// Time stamp
	igstk::RealTimeClock::TimeStampType timeStamp = imgExportedStream->GetTimeStamp();
//	std::cout << "timeStamp : " << timeStamp << std::endl;
//	REQUIRE(similar(timeStamp, 0, 0.5));
	REQUIRE(timeStamp > 1000000);


	vtkSmartPointer<vtkImageData> img = imgExportedStream->GetScanConvertedImage();
	REQUIRE(img);//Got scan converted image?

	this->validateBMode3D(img);

	//Skip assert if OpenCL is turned off
#ifdef DATASTREAMING_USE_OPENCL
	std::cout << "Require OpenCL" << std::endl;
	REQUIRE(geStreamer.UsingOpenCL());
#endif

	/*img = imgExportedStream->GetTissueImage();
	REQUIRE(img);//Got image?
	img = imgExportedStream->GetBandwidthImage();
	REQUIRE(img);//Got image?
	img = imgExportedStream->GetFrequencyImage();
	REQUIRE(img);//Got image?*/

	geStreamer.DisconnectFromScanner();
}

int TestGEInterface::getValue(vtkSmartPointer<vtkImageData> img, int x, int y, int z)
{
	int val = (int)*reinterpret_cast<unsigned char*>(img->GetScalarPointer(x,y,z));
	return val;
}

void TestGEInterface::validateData(vtkSmartPointer<vtkImageData> img)
{
	unsigned char* volumePtr = reinterpret_cast<unsigned char*>(img->GetScalarPointer());
	REQUIRE(volumePtr); //Check if the pointer != NULL
}

void TestGEInterface::validateBMode3D(vtkSmartPointer<vtkImageData> img)
{
	int* dim =  img->GetDimensions();
	std::cout << "dim: " << dim[0] << " " << dim[1] << " " << dim[2] << std::endl;
	//std::cout << "value in " << dim[0]/3 << " " << dim[1]/3 <<  " " << dim[2]/3 << ": ";
	std::cout << getValue(img, dim[0]/3,dim[1]/3,dim[2]/3) << std::endl;
	REQUIRE( getValue(img, dim[0]/3,dim[1]/3,dim[2]/3) == 100);
}

} //namespace cxtest

#endif //CX_USE_ISB_GE
