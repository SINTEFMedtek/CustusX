#include "cxTestGEInterface.h"

#ifdef CX_USE_ISB_GE
#include <QTimer>
#include "GEStreamer.h"
//#include "sscVector3D.h"
#include "cxGrabberSender.h"
#include "cxTestGEInterfaceController.h"
#include "sscMessageManager.h"

void TestGEInterface::setUp()
{
//	cx::LogicManager::initialize();
	ssc::MessageManager::initialize();
}

void TestGEInterface::tearDown()
{
//	cx::LogicManager::shutdown();
	ssc::MessageManager::shutdown();
}

void TestGEInterface::testConstructor()
{
}

void TestGEInterface::testStreams()
{
	std::cout << std::endl << "*** Test GE all streams streams. CPU scanconversion ***" << std::endl;
	cx::StringMap args;
	args["type"] = "ISB_GE";
	args["test"] = "2D";
	args["useOpenCL"] = "0"; //Test only CPU scan conversion
	std::cout << std::endl << "--- Test GE 2D scanconverted stream. Auto size ---" << std::endl;
	this->testStream(args);
	args["imagesize"] = "500*500";
	std::cout << std::endl << "--- Test GE 2D scanconverted stream. Defined size ---" << std::endl;
	this->testStream(args); //set size

	args["test"] = "3D";
	args["imagesize"] = "auto";
	std::cout << std::endl << "--- Test GE 3D scanconverted stream. Auto size ---" << std::endl;
	this->testStream(args); //3D
	args["imagesize"] = "100*100*100";
	std::cout << std::endl << "--- Test GE 3D scanconverted stream. Defined size ---" << std::endl;
	this->testStream(args); //set size

//	args["test"] = "no";
//	args["ip"] = "bhgrouter.hopto.org";
//	std::cout << "---Custom test: Connect to simulator" << std::endl;
//	this->testStream(args);//Custom test
}

//Test currently needs the simulator to run with doppler, or be conected to the scanner
void TestGEInterface::testAllStreamsGPU()
{
	cx::StringMap args;
	args["type"] = "ISB_GE";
	args["test"] = "2D";
	//args["ip"] = "bhgrouter.hopto.org";
	//args["test"] = "no";
	args["useOpenCL"] = "1"; //Test GPU (OpenCL) scan conversion
	args["streams"] = "all";
	std::cout << std::endl << "--- Test GE 2D all streams. GPU scanconversion if possible ---" << std::endl;
	this->testStream(args);

}
void TestGEInterface::testScanConvertedStreamGPU()
{
	cx::StringMap args;
	args["type"] = "ISB_GE";
	args["test"] = "2D";
	args["useOpenCL"] = "1"; //Test GPU (OpenCL) scan conversion
	args["streams"] = "scanconverted";
	std::cout << std::endl << "--- Test GE 2D scanconverted stream. GPU scanconversion if possible ---" << std::endl;
	this->testStream(args);
}
void TestGEInterface::testTissueStreamGPU()
{
	cx::StringMap args;
	args["type"] = "ISB_GE";
	args["test"] = "2D";
	args["useOpenCL"] = "1"; //Test GPU (OpenCL) scan conversion
	args["streams"] = "tissue";
	std::cout << std::endl << "--- Test GE 2D tissue stream. GPU scanconversion if possible ---" << std::endl;
	this->testStream(args);
}
void TestGEInterface::testFrequencyStreamGPU()
{
	cx::StringMap args;
	args["type"] = "ISB_GE";
	args["test"] = "2D";
	args["useOpenCL"] = "1"; //Test GPU (OpenCL) scan conversion
	args["streams"] = "frequency";
	std::cout << std::endl << "--- Test GE 2D frequency stream. GPU scanconversion if possible ---" << std::endl;
	this->testStream(args);
}
void TestGEInterface::testBandwidthStreamGPU()
{
	cx::StringMap args;
	args["type"] = "ISB_GE";
	args["test"] = "2D";
	args["useOpenCL"] = "1"; //Test GPU (OpenCL) scan conversion
	args["streams"] = "bandwidth";
	std::cout << std::endl << "--- Test GE 2D bandwidth stream. GPU scanconversion if possible ---" << std::endl;
	this->testStream(args);
}
void TestGEInterface::testVelocityStreamGPU()
{
	cx::StringMap args;
	args["type"] = "ISB_GE";
	args["test"] = "2D";
	args["useOpenCL"] = "1"; //Test GPU (OpenCL) scan conversion
	args["streams"] = "velocity";
	std::cout << std::endl << "--- Test GE 2D velocity stream. ---" << std::endl;
	this->testStream(args);
}

void TestGEInterface::testAllStreamsGPUConsecutively()
{
	testAllStreamsGPU();
	testScanConvertedStreamGPU();
	testTissueStreamGPU();
	testFrequencyStreamGPU();
	testBandwidthStreamGPU();
	testVelocityStreamGPU();
}

void TestGEInterface::testStream(cx::StringMap args)
{
	cx::ImageSenderPtr imageSender = cx::ImageSenderFactory().getFromArguments(args);
	CPPUNIT_ASSERT(imageSender);
	CPPUNIT_ASSERT(imageSender->getType().compare(args["type"]) == 0);

	cx::GrabberSenderDirectLinkPtr grabberBridge(new cx::GrabberSenderDirectLink());

	TestGEInterfaceController controller(NULL);
	controller.initialize(grabberBridge);

	CPPUNIT_ASSERT(imageSender->startStreaming(grabberBridge));

	//	QTimer::singleShot(1*1000,   qApp, SLOT(quit()) );
	QTimer::singleShot(500,   qApp, SLOT(quit()) );
	qApp->exec();

	imageSender->stopStreaming();

	imageSender.reset();
	grabberBridge.reset();

	controller.verify();
}

void TestGEInterface::testGEStreamer()
{
	//std::cout << std::endl << "*** Test GE 3D scanconverted stream. GPU scanconversion if possible ***" << std::endl;
	std::cout << std::endl << "*** Test GE 3D scanconverted stream. CPU scanconversion for now ***" << std::endl;
	data_streaming::GEStreamer geStreamer;

	//Initialize GEStreamer    HFDPath, useHDF, sizeCompType,     imgSize, interpolation,       buffSize, clPath, useCL
	geStreamer.InitializeClientData("", false, data_streaming::AUTO, -1, data_streaming::Bilinear, 10,   "",     false);

	//Setup the needed data stream types. The default is only scan converted data
	geStreamer.SetupExportParameters(true, false, false, false, false);
//	geStreamer.SetupExportParameters(true, true, true, true);

	//                                         (hostIp, streamPort, commandPort, testMode));
	CPPUNIT_ASSERT(geStreamer.ConnectToScanner("127.0.0.1", 6543,    -1,         data_streaming::test3D));
//	CPPUNIT_ASSERT(geStreamer.ConnectToScanner("bhgrouter.hopto.org", 6543,    -1,         data_streaming::noTest));


	geStreamer.WaitForImageData();
	vtkSmartPointer<data_streaming::vtkExportedStreamData> imgExportedStream = geStreamer.GetExportedStreamDataAndMoveToNextFrame();
	CPPUNIT_ASSERT(imgExportedStream);//Got image?

	// Frame geometry
	data_streaming::frame_geometry frameGeometry = imgExportedStream->GetTissueGeometry();
//	std::cout << "depthStart : " << frameGeometry.depthStart << std::endl;
//	std::cout << "depthEnd : " << frameGeometry.depthEnd << std::endl;
//	std::cout << "vNyquist : " << frameGeometry.vNyquist << std::endl;
//	std::cout << "width : " << frameGeometry.width << std::endl;
	//Test mode parameters
	CPPUNIT_ASSERT(frameGeometry.depthEnd == 100);
	CPPUNIT_ASSERT(frameGeometry.width == 1);

	// Time stamp
	igstk::RealTimeClock::TimeStampType timeStamp = imgExportedStream->GetTimeStamp();
//	std::cout << "timeStamp : " << timeStamp << std::endl;
//	CPPUNIT_ASSERT(ssc::similar(timeStamp, 0, 0.5));
	CPPUNIT_ASSERT(timeStamp > 1000000);


	vtkSmartPointer<vtkImageData> img = imgExportedStream->GetScanConvertedImage();
	CPPUNIT_ASSERT(img);//Got scan converted image?

	this->validateBMode3D(img);

	/*img = imgExportedStream->GetTissueImage();
	CPPUNIT_ASSERT(img);//Got image?
	img = imgExportedStream->GetBandwidthImage();
	CPPUNIT_ASSERT(img);//Got image?
	img = imgExportedStream->GetFrequencyImage();
	CPPUNIT_ASSERT(img);//Got image?*/

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
	CPPUNIT_ASSERT(volumePtr); //Check if the pointer != NULL
}

void TestGEInterface::validateBMode3D(vtkSmartPointer<vtkImageData> img)
{
	int* dim =  img->GetDimensions();
	std::cout << "dim: " << dim[0] << " " << dim[1] << " " << dim[2] << std::endl;
	//std::cout << "value in " << dim[0]/3 << " " << dim[1]/3 <<  " " << dim[2]/3 << ": ";
	std::cout << getValue(img, dim[0]/3,dim[1]/3,dim[2]/3) << std::endl;
	CPPUNIT_ASSERT( getValue(img, dim[0]/3,dim[1]/3,dim[2]/3) == 100);
}

#endif //CX_USE_ISB_GE
