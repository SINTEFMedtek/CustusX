/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "catch.hpp"

#include <QEventLoop>
#include <boost/make_shared.hpp>
#include "vtkTimerLog.h"
#include "igtlioConnector.h"
#include "igtlioDevice.h"
#include "igtlioSession.h"

#include "cxNetworkHandler.h"
#include "cxtestReceiver.h"

#include "cxLogger.h"
#include "cxImage.h"
#include "cxTransform3D.h"
#include "cxtestQueuedSignalListener.h"
#include "cxNetworkHandler.h"
#include "cxProbeSector.h"
#include "cxUtilHelpers.h"

#include "cxtestPlusReceiver.h"
#include "cxtestIOReceiver.h"
#include "cxtestVideoGraphicsFixture.h"

namespace
{
class igtlioServerClientFixture
{
public:
	igtlioLogicPointer logic;
	igtlioSessionPointer server;
	igtlioSessionPointer client;

	void startServerAndClientAndConnect()
	{
		std::string ip = "localhost";
		int port = 18944;

		logic = igtlioLogicPointer::New();

		// Server default port should be 18944
		// Verify this by not setting port on explicitly on server
		server = logic->StartServer();
		client = logic->ConnectToServer(ip, port);
		REQUIRE(server);
		REQUIRE(client);
	}
};

class NetworkHandlerTester : public cx::NetworkHandler
{
public:
	NetworkHandlerTester(igtlioLogicPointer logic) :
		NetworkHandler(logic)
	{}

	vtkImageDataPtr getMask()
	{
		return mUSMask;
	}
	void setProbeDefinition(cx::ProbeDefinitionPtr probeDefinition)
	{
		mProbeDefinition = probeDefinition;
	}
	bool testConvertZeroesInsideSectorToOnes(cx::ImagePtr image, int threashold, int newValue)
	{
		return this->convertZeroesInsideSectorToOnes(image, threashold, newValue);
	}
	bool testCreateMask()
	{
		return this->createMask();
	}
};

} //namespace

namespace cxtest
{

void tryToReceiveEvents(igtlioLogicPointer logic, Receiver &receiver)
{
	Q_UNUSED(receiver);
	igtlioConnectorPointer connector = logic->GetConnector(0);

	double timeout = 1;
	double starttime = vtkTimerLog::GetUniversalTime();
	while (vtkTimerLog::GetUniversalTime() - starttime < timeout)
	{
		logic->PeriodicProcess();
	}
}

void listenToAllDevicesToCountMessages(igtlioLogicPointer logic, Receiver &receiver)
{
	unsigned index = logic->GetNumberOfDevices();
	for(unsigned i=0; i<index; ++i)
	{
		receiver.listen(logic->GetDevice(i), true);
	}
}

bool isConnected(igtlioLogicPointer logic)
{
	return logic->GetConnector(0)->GetState() == igtlioConnector::STATE_CONNECTED;
}

//----------------------------------------------------------------------------------------------------------------------------------

TEST_CASE("Can connect to a plus server and receive messages", "[plugins][org.custusx.core.openigtlink3][manual]")
{
	//expecting server to be run with:
	// PlusServer.exe --config-file="C:\d\PlusB-bin\PlusLibData\ConfigFiles\PlusDeviceSet_Server_BkProFocusOem.xml"

	std::string ip = "10.218.140.107"; //ovs
	//std::string ip = "10.218.140.127"; //jbake
	int port = 18944; //-1

	igtlioLogicPointer logic = igtlioLogicPointer::New();

	PlusReceiver receiver(logic);
	receiver.connect(ip, port);

	tryToReceiveEvents(logic, receiver);

	REQUIRE(isConnected(logic));
	tryToReceiveEvents(logic, receiver);
	listenToAllDevicesToCountMessages(logic, receiver);
	tryToReceiveEvents(logic, receiver);

//	REQUIRE(receiver.number_of_events_received > 0);
//	REQUIRE(receiver.image_received);
	//REQUIRE(receiver.transform_received);

	//receiver.sendString();

	//receiver.send_RequestDepthAndGain();

	receiver.send_RequestDepth();
	receiver.send_RequestGain();
	receiver.send_RequestUsSectorParameters();
//	receiver.send_RequestDeviceIds();
//	receiver.send_RequestChannelIds();
//	receiver.send_RequestDeviceChannelIds();
//	receiver.send_RequestInputDeviceIds();
	tryToReceiveEvents(logic, receiver);
	//REQUIRE(receiver.string_received);
	REQUIRE(receiver.command_respons_received);

}

TEST_CASE("Can connect to a igtlioQtClient server", "[plugins][org.custusx.core.openigtlink3][manual]")
{
	//expecting a igtlioQtClient to run

	std::string ip = "localhost";
	int port = 18944;

	igtlioLogicPointer logic = igtlioLogicPointer::New();

	IOReceiver receiver(logic);
	receiver.connect(ip, port);

	tryToReceiveEvents(logic, receiver);
	REQUIRE(isConnected(logic));

	tryToReceiveEvents(logic, receiver);
	listenToAllDevicesToCountMessages(logic, receiver);
	tryToReceiveEvents(logic, receiver);

	receiver.sendCommand_Get_Parameter_Depth();

	tryToReceiveEvents(logic, receiver);
	REQUIRE(receiver.command_respons_received);

}

//Hangs occasionally on Windows
TEST_CASE("Connect client to server", "[plugins][org.custusx.core.openigtlink3][integration][not_win64]")
{
	igtlioServerClientFixture fixture;
	fixture.startServerAndClientAndConnect();

	REQUIRE(fixture.client->GetConnector()->IsConnected());
	REQUIRE(fixture.client->GetConnector()->Stop());
	REQUIRE_FALSE(fixture.client->GetConnector()->IsConnected());
}

//Hangs occasionally on Windows
TEST_CASE("Stop and remove client and server connectors works", "[plugins][org.custusx.core.openigtlink3][integration][not_win64]")
{
	igtlioServerClientFixture fixture;
	fixture.startServerAndClientAndConnect();

	igtlioConnectorPointer connector = fixture.client->GetConnector();
	REQUIRE(connector);

	REQUIRE(connector->GetState() == igtlioConnector::STATE_CONNECTED);

	REQUIRE(connector->IsConnected());
	REQUIRE(connector->Stop());
	REQUIRE_FALSE(connector->IsConnected());
	REQUIRE_FALSE(connector->Stop());

	REQUIRE(connector->GetState() == igtlioConnector::STATE_OFF);

	connector = fixture.server->GetConnector();
	REQUIRE(connector);
	//Server connector is not connected?

	REQUIRE(fixture.logic->RemoveConnector(fixture.client->GetConnector()));
	REQUIRE_FALSE(fixture.logic->RemoveConnector(fixture.client->GetConnector()));

	REQUIRE(fixture.logic->RemoveConnector(fixture.server->GetConnector()));
	REQUIRE_FALSE(fixture.logic->RemoveConnector(fixture.server->GetConnector()));
}

TEST_CASE("Connect/disconnect using NetworkHandler, use default network port", "[plugins][org.custusx.core.openigtlink3][integration]")
{
	igtlioLogicPointer logic = igtlioLogicPointer::New();
	cx::NetworkHandlerPtr networkHandler= cx::NetworkHandlerPtr(new cx::NetworkHandler(logic));

	std::string ip = "localhost";

	igtlioSessionPointer server = logic->StartServer();

	igtlioSessionPointer client = networkHandler->requestConnectToServer(ip);
	REQUIRE(client);
	REQUIRE(client->GetConnector());
	REQUIRE(client->GetConnector()->IsConnected());

	networkHandler->disconnectFromServer();
	REQUIRE(client->GetConnector());
	REQUIRE_FALSE(client->GetConnector()->IsConnected());
}

// Reused code from cxtestVideoGraphics
TEST_CASE_METHOD(cxtest::VideoGraphicsFixture, "VideoGraphics: Test US sector zero conversion", "[plugins][org.custusx.core.openigtlink3][unit][visualization]")
{
	QString imageFilename = "US_small.mhd";
	vtkImageDataPtr videoImage0 = this->readImageData(imageFilename, "input image");
	vtkImageDataPtr expected = this->readImageData("US_small_sector_masked.png", "input expected");

	cx::ProbeDefinition probeDefinition = this->readProbeDefinition(imageFilename);
	cx::ProbeDefinitionPtr probeDefinitionPtr = boost::make_shared<cx::ProbeDefinition>(probeDefinition);
	REQUIRE(probeDefinitionPtr);

	igtlioLogicPointer logic = igtlioLogicPointer::New();
	NetworkHandlerTester networkHandler(logic);
	networkHandler.setProbeDefinition(probeDefinitionPtr);

	cx::ImagePtr image = cx::ImagePtr(new cx::Image(imageFilename, videoImage0));
	vtkImageDataPtr vtkImage = image->getBaseVtkImageData();
	CHECK(networkHandler.testCreateMask());
	REQUIRE(networkHandler.getMask());

	unsigned char* imagePtr = static_cast<unsigned char*> (vtkImage->GetScalarPointer());
	unsigned char* maskPtr = static_cast<unsigned char*> (networkHandler.getMask()->GetScalarPointer());
	Eigen::Array3i dims(vtkImage->GetDimensions());

	unsigned pos = 200 + 200 * dims[0]; // (x, y)
	CHECK(maskPtr[pos] != 0);
	CHECK(imagePtr[pos] != 0);
	imagePtr[pos] = 0;
	CHECK(imagePtr[pos] == 0);

	CHECK(networkHandler.testConvertZeroesInsideSectorToOnes(image, 0, 255));
	CHECK(imagePtr[pos] == 255);

	 //Renders and saves images for visual inspaction, but CHECK will fail
//	this->renderImageAndCompareToExpected(image->getBaseVtkImageData(), expected);
//	cx::sleep_ms(3000);
}

TEST_CASE_METHOD(cxtest::VideoGraphicsFixture, "VideoGraphics: Test US sector zero conversion speed", "[plugins][org.custusx.core.openigtlink3][unit][visualization]")
{
	QString imageFilename = "US_small.mhd";
	vtkImageDataPtr videoImage0 = this->readImageData(imageFilename, "input image");
	vtkImageDataPtr expected = this->readImageData("US_small_sector_masked.png", "input expected");

	cx::ProbeDefinition probeDefinition = this->readProbeDefinition(imageFilename);
	cx::ProbeDefinitionPtr probeDefinitionPtr = boost::make_shared<cx::ProbeDefinition>(probeDefinition);
	REQUIRE(probeDefinitionPtr);

	igtlioLogicPointer logic = igtlioLogicPointer::New();
	NetworkHandlerTester networkHandler(logic);
	networkHandler.setProbeDefinition(probeDefinitionPtr);

	cx::ImagePtr image = cx::ImagePtr(new cx::Image(imageFilename, videoImage0));
	vtkImageDataPtr vtkImage = image->getBaseVtkImageData();
	CHECK(networkHandler.testCreateMask());
	REQUIRE(networkHandler.getMask());

	QTime clock;
	unsigned times = 100;
	int timeMs = 0;
	for(int i = 0; i < times; ++i)
	{
		image = cx::ImagePtr(new cx::Image(imageFilename, videoImage0));
		clock.start();
		networkHandler.testConvertZeroesInsideSectorToOnes(image, 0, 255);
		timeMs += clock.elapsed();
	}
	int averageTime = timeMs/times;
	CX_LOG_DEBUG() << "Average image conversion time (sampled separately) for " << times << " images: " << averageTime << " ms.";
	CHECK(averageTime < 10);

	timeMs = 0;
	clock.start();
	for(int i = 0; i < times; ++i)
	{
//		image = cx::ImagePtr(new cx::Image(imageFilename, videoImage0));
		networkHandler.testConvertZeroesInsideSectorToOnes(image, 0, 255);
	}
	timeMs += clock.elapsed();
	averageTime = timeMs/times;
	CX_LOG_DEBUG() << "Average image conversion time (whole for loop, skipped image reload) for " << times << " images: " << averageTime << " ms.";
	CHECK(averageTime < 10);
}

} //namespace cxtest
