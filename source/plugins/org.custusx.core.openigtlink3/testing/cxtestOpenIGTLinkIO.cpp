/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

namespace cxtest
{


void checkIfConnected(igtlio::LogicPointer logic)
{
	double timeout = 2;
	double starttime = vtkTimerLog::GetUniversalTime();

	igtlio::ConnectorPointer connector = logic->GetConnector(0);

	while (vtkTimerLog::GetUniversalTime() - starttime < timeout)
	{
		logic->PeriodicProcess();

		if (connector->GetState() == igtlio::Connector::STATE_CONNECTED)
		{
			REQUIRE(true);
			break;
		}
		if (connector->GetState() == igtlio::Connector::STATE_OFF)
		{
			REQUIRE(false);
		}
	}

	REQUIRE(connector->GetState() == static_cast<int>(igtlio::Connector::STATE_CONNECTED));
}

void tryToReceiveEvents(igtlio::LogicPointer logic, Receiver &receiver)
{
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
	int index = logic->GetNumberOfDevices();
	for(int i=0; i<index; ++i)
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
	std::string ip = "localhost";
	int port = 18944;

	igtlio::LogicPointer logic = igtlio::LogicPointer::New();

	int defaultPort = -1;
	igtlio::SessionPointer server = logic->StartServer(defaultPort);//Verify that default port == 18944
	igtlio::SessionPointer client = logic->ConnectToServer(ip, port);
	REQUIRE(server);
	REQUIRE(client);

	REQUIRE(client->GetConnector()->Stop());
	REQUIRE_FALSE(client->GetConnector()->IsConnected());
}

TEST_CASE("Stop and remove client and server connectors works", "[plugins][org.custusx.core.openigtlink3][integration]")
{
	std::string ip = "localhost";
	int port = 18944;

	igtlio::LogicPointer logic = igtlio::LogicPointer::New();

	igtlio::SessionPointer server = logic->StartServer(port);
	igtlio::SessionPointer client = logic->ConnectToServer(ip, port);
	REQUIRE(server);
	REQUIRE(client);

	igtlio::ConnectorPointer connector = client->GetConnector();
	REQUIRE(connector);
	REQUIRE(connector->IsConnected());
	REQUIRE(connector->Stop());
	REQUIRE_FALSE(connector->IsConnected());
	REQUIRE_FALSE(connector->Stop());

	connector = server->GetConnector();
	REQUIRE(connector);
	//Server connector is not connected?

	REQUIRE(logic->RemoveConnector(client->GetConnector()));
	REQUIRE_FALSE(logic->RemoveConnector(client->GetConnector()));

	REQUIRE(logic->RemoveConnector(server->GetConnector()));
	REQUIRE_FALSE(logic->RemoveConnector(server->GetConnector()));
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
