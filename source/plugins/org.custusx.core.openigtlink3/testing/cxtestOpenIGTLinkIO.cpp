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
#include "vtkIGTLIOConnector.h"
#include "vtkIGTLIODevice.h"
#include "cxNetworkHandler.h"
#include "cxtestReceiver.h"

#include "cxLogger.h"
#include "cxImage.h"
#include "cxTransform3D.h"
#include "cxtestQueuedSignalListener.h"

namespace cxtest
{


void tryToConnect(vtkIGTLIOLogicPointer logic, vtkIGTLIOConnectorPointer connector)
{
	double timeout = 1;
	double starttime = vtkTimerLog::GetUniversalTime();

	while (vtkTimerLog::GetUniversalTime() - starttime < timeout)
	{
		logic->PeriodicProcess();

		if (connector->GetState() == vtkIGTLIOConnector::STATE_CONNECTED)
		{
			REQUIRE(true);
			break;
		}
		if (connector->GetState() == vtkIGTLIOConnector::STATE_OFF)
		{
			REQUIRE(false);
		}
	}

	REQUIRE(connector->GetState() == vtkIGTLIOConnector::STATE_CONNECTED);
}

void tryToReceiveEvents(vtkIGTLIOLogicPointer logic, vtkIGTLIOConnectorPointer connector, Receiver &receiver)
{
	double timeout = 1;
	double starttime = vtkTimerLog::GetUniversalTime();
	while (vtkTimerLog::GetUniversalTime() - starttime < timeout)
	{
		logic->PeriodicProcess();
	}

	int index = logic->GetNumberOfDevices();
	for(int i=0; i<index; ++i)
	{
		receiver.listen(logic->GetDevice(i));
	}

	starttime = vtkTimerLog::GetUniversalTime();
	while (vtkTimerLog::GetUniversalTime() - starttime < timeout)
	{
		logic->PeriodicProcess();
	}

	REQUIRE(receiver.mEventsReceived > 0);

}


TEST_CASE("OpenIGTLinkIO can connect to a plus server and receive messages", "[plugins][org.custusx.core.openigtlink3][manual]")
{
	vtkIGTLIOLogicPointer logic = vtkIGTLIOLogicPointer::New();

	vtkIGTLIOConnectorPointer connector = logic->CreateConnector();
	connector->SetTypeClient(connector->GetServerHostname(), connector->GetServerPort());
	connector->Start();

	tryToConnect(logic, connector);

	Receiver receiver(logic);

	tryToReceiveEvents(logic, connector, receiver);
}

TEST_CASE("NetworkHandler can connect to a plus server and receive messages", "[plugins][org.custusx.core.openigtlink3][manual]")
{
	vtkIGTLIOLogicPointer logic = vtkIGTLIOLogicPointer::New();

	vtkIGTLIOConnectorPointer connector = logic->CreateConnector();
	connector->SetTypeClient(connector->GetServerHostname(), connector->GetServerPort());
	connector->Start();

	tryToConnect(logic, connector);

	Receiver receiver(logic);

	tryToReceiveEvents(logic, connector, receiver);

	REQUIRE(receiver.image_received);
	REQUIRE(receiver.transform_received);

}



} //namespace cxtest
