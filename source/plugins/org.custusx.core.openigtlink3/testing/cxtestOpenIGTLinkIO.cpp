/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "catch.hpp"

#include <QEventLoop>
#include "vtkTimerLog.h"
#include "igtlioConnector.h"
#include "igtlioDevice.h"
#include "igtlioSession.h"
#include "igtlioCommandDevice.h"

#include "cxNetworkHandler.h"
#include "cxtestReceiver.h"

#include "cxLogger.h"
#include "cxImage.h"
#include "cxTransform3D.h"
#include "cxtestQueuedSignalListener.h"

#include "cxtestPlusReceiver.h"
#include "cxtestIOReceiver.h"

namespace
{
class igtlioServerClientFixture
{
public:
	igtlio::LogicPointer logic;
	igtlio::SessionPointer server;
	igtlio::SessionPointer client;

	void startServerAndClientAndConnect()
	{
		std::string ip = "localhost";
		int port = 18944;

		logic = igtlio::LogicPointer::New();

		// Server default port should be 18944
		// Verify this by not setting port on explicitly on server
		server = logic->StartServer();
		client = logic->ConnectToServer(ip, port);
		REQUIRE(server);
		REQUIRE(client);
	}
};
}

namespace cxtest
{

void tryToReceiveEvents(igtlio::LogicPointer logic, Receiver &receiver)
{
	Q_UNUSED(receiver);
	igtlio::ConnectorPointer connector = logic->GetConnector(0);

	double timeout = 1;
	double starttime = vtkTimerLog::GetUniversalTime();
	while (vtkTimerLog::GetUniversalTime() - starttime < timeout)
	{
		logic->PeriodicProcess();
	}
}

void listenToAllDevicesToCountMessages(igtlio::LogicPointer logic, Receiver &receiver)
{
	unsigned index = logic->GetNumberOfDevices();
	for(unsigned i=0; i<index; ++i)
	{
		receiver.listen(logic->GetDevice(i), true);
	}
}

bool isConnected(igtlio::LogicPointer logic)
{
	return logic->GetConnector(0)->GetState() == igtlio::Connector::STATE_CONNECTED;
}

//----------------------------------------------------------------------------------------------------------------------------------

TEST_CASE("Can connect to a plus server and receive messages", "[plugins][org.custusx.core.openigtlink3][manual]")
{
	//expecting server to be run with:
	// PlusServer.exe --config-file="C:\d\PlusB-bin\PlusLibData\ConfigFiles\PlusDeviceSet_Server_BkProFocusOem.xml"

	std::string ip = "10.218.140.107"; //ovs
	//std::string ip = "10.218.140.127"; //jbake
	int port = 18944; //-1

	igtlio::LogicPointer logic = igtlio::LogicPointer::New();

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

	igtlio::LogicPointer logic = igtlio::LogicPointer::New();

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

TEST_CASE("Connect client to server", "[plugins][org.custusx.core.openigtlink3][integration]")
{
	igtlioServerClientFixture fixture;
	fixture.startServerAndClientAndConnect();

	REQUIRE(fixture.client->GetConnector()->IsConnected());
	REQUIRE(fixture.client->GetConnector()->Stop());
	REQUIRE_FALSE(fixture.client->GetConnector()->IsConnected());
}

TEST_CASE("Stop and remove client and server connectors works", "[plugins][org.custusx.core.openigtlink3][integration]")
{
	igtlioServerClientFixture fixture;
	fixture.startServerAndClientAndConnect();

	igtlio::ConnectorPointer connector = fixture.client->GetConnector();
	REQUIRE(connector);

	REQUIRE(connector->GetState() == igtlio::Connector::STATE_CONNECTED);

	REQUIRE(connector->IsConnected());
	REQUIRE(connector->Stop());
	REQUIRE_FALSE(connector->IsConnected());
	REQUIRE_FALSE(connector->Stop());

	REQUIRE(connector->GetState() == igtlio::Connector::STATE_OFF);

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
	igtlio::LogicPointer logic = igtlio::LogicPointer::New();
	cx::NetworkHandlerPtr networkHandler= cx::NetworkHandlerPtr(new cx::NetworkHandler(logic));

	std::string ip = "localhost";

	igtlio::SessionPointer server = logic->StartServer();

	igtlio::SessionPointer client = networkHandler->requestConnectToServer(ip);
	REQUIRE(client);
	REQUIRE(client->GetConnector());
	REQUIRE(client->GetConnector()->IsConnected());

	networkHandler->disconnectFromServer();
	REQUIRE(client->GetConnector());
	REQUIRE_FALSE(client->GetConnector()->IsConnected());
}

} //namespace cxtest
