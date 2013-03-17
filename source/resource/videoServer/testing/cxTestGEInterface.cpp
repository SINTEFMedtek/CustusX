#include "cxTestGEInterface.h"

#ifdef CX_USE_ISB_GE
#include <QTimer>
#include "GEStreamer.h"
//#include "sscVector3D.h"
#include "cxImageSenderFactory.h"
#include "cxGrabberSender.h"
#include "cxTestGEInterfaceController.h"

void TestGEInterface::setUp()
{
//	cx::LogicManager::initialize();
}

void TestGEInterface::tearDown()
{
//	cx::LogicManager::shutdown();
}

void TestGEInterface::testConstructor()
{
}

void TestGEInterface::testInit()
{
	cx::StringMap args;
	args["type"] = "ISB_GE";
	args["test"] = "2D";
	args["useOpenCL"] = "0";
	cx::ImageSenderPtr imageSender = cx::ImageSenderFactory().getFromArguments(args);
	CPPUNIT_ASSERT(imageSender);
	CPPUNIT_ASSERT(imageSender->getType().compare("ISB_GE") == 0);

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
	data_streaming::GEStreamer geStreamer;

	//Initialize GEStreamer    HFDPath, useHDF, imgSize, interpolation,       buffSize, clPath, useCL
	geStreamer.InitializeClientData("", false, 500*500, data_streaming::Bilinear, 10,   "",     false);

	//Setup the needed data stream types. The default is only scan converted data
	geStreamer.SetupExportParameters(true, false, false, false);
//	geStreamer.SetupExportParameters(true, true, true, true);

	//                                         (hostIp, streamPort, commandPort, testMode));
	CPPUNIT_ASSERT(geStreamer.ConnectToScanner("127.0.0.1", 6543,    -1,         data_streaming::test3D));


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
	std::cout << getValue(img, 0,0,0) << std::endl;
	std::cout << getValue(img, 5,5,5) << std::endl;
	std::cout << getValue(img, 7,7,7) << std::endl;
	std::cout << getValue(img, 9,9,9) << std::endl;

}

#endif //CX_USE_ISB_GE