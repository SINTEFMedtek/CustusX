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

#include "cxOpenIGTLinkTrackingSystemService.h"
#include "cxOpenIGTLinkStreamerService.h"
#include "cxtestQueuedSignalListener.h"
#include "cxUtilHelpers.h"
#include "cxtestUtilities.h"
#include "cxReporter.h"
#include "cxNetworkConnection.h"
#include "cxOpenIGTLinkGuiExtenderService.h"
#include "cxNetworkServiceImpl.h"
#include "cxNetworkConnectionHandle.h"
#include <boost/bind.hpp>

namespace cxtest
{

TEST_CASE("OpenIGTLinkTrackingSystemService: Check that the openigtlink tracking service can be created and destroyed", "[unit][plugins][org.custusx.core.tracking.system.openigtlink]")
{
	cx::OpenIGTLinkTrackingSystemServicePtr service = cx::OpenIGTLinkTrackingSystemServicePtr(new cx::OpenIGTLinkTrackingSystemService(cx::NetworkConnectionHandlePtr()));
    REQUIRE(service);
    CHECK(service.unique());
    service.reset();
}

TEST_CASE("OpenIGTLinkGuiExtender: Check that the openigtlink gui extender service can be created and destroyed", "[unit][org.custusx.core.tracking.system.openigtlink][hide][not_working]")
{
    cx::NetworkServiceImplPtr connections(new cx::NetworkServiceImpl());

    //NOT WORKING, cannot send in NULL as context(?)
	cx::OpenIGTLinkGuiExtenderServicePtr gui(new cx::OpenIGTLinkGuiExtenderService(NULL, connections));
    REQUIRE(gui);
    CHECK(gui.unique());
    gui.reset();
	connections.reset();
}

TEST_CASE("OpenIGTLinkTrackingSystemService: Check that the plugin can connect and stream from a plus server", "[manual][plugins][org.custusx.core.tracking.system.openigtlink]")
{
	cx::NetworkConnectionHandlePtr client_connection_handle(new cx::NetworkConnectionHandle("test", cx::XmlOptionFile()));
	cx::NetworkConnection::ConnectionInfo info;
    info.host = "10.218.140.138";
	info.port = 18944;
	info.protocol = "PlusServer";
    cx::NetworkConnection* client_connection = client_connection_handle->getNetworkConnection();
    client_connection->setConnectionInfo(info);

    cx::OpenIGTLinkTrackingSystemServicePtr service(new cx::OpenIGTLinkTrackingSystemService(client_connection_handle));
    REQUIRE(service);
    REQUIRE(service->getState() == cx::Tool::tsNONE);

    boost::function<void()> client_connect = boost::bind(&cx::NetworkConnection::requestConnect, client_connection);
    boost::function<void()> client_disconnect = boost::bind(&cx::NetworkConnection::requestDisconnect, client_connection);
    client_connection->invoke(client_connect);
    while(client_connection->getState() != cx::scsCONNECTED)
    {
        cxtest::waitForQueuedSignal(client_connection, SIGNAL(stateChanged(CX_SOCKETCONNECTION_STATE)));
    }
    REQUIRE(client_connection->getState() == cx::scsCONNECTED);

    while(service->getState() != cx::Tool::tsTRACKING)
    {
        cxtest::waitForQueuedSignal(service.get(), SIGNAL(stateChanged()));
    }
    REQUIRE(service->getState() == cx::Tool::tsTRACKING);

    cx::sleep_ms(500); //just let some packages be sendt
    //let 6 packages arrive
    for(int i=0; i<6; ++i)
    {
        cxtest::waitForQueuedSignal(client_connection, SIGNAL(stateChanged(CX_SOCKETCONNECTION_STATE)));
    }

    CHECK(service->getTools().size() != 0); //this means at least on transform has arrived from the plus server

    client_connection->invoke(client_disconnect);
    cxtest::waitForQueuedSignal(client_connection, SIGNAL(stateChanged(CX_SOCKETCONNECTION_STATE)));

    //service.reset();
    //client_connection_handle.reset();

}

TEST_CASE("OpenIGTLinkStreamingService: Check that the openigtlink streaming service can be created and destroyed", "[org.custusx.core.tracking.system.openigtlink]")
{
	cx::NetworkConnectionHandlePtr connection(new cx::NetworkConnectionHandle("test", cx::XmlOptionFile()));
    cx::OpenIGTLinkStreamerServicePtr service = cx::OpenIGTLinkStreamerServicePtr(new cx::OpenIGTLinkStreamerService(connection));
    REQUIRE(service);
    CHECK(service.unique());
    service.reset();
}

} //namespace cxtest
