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

#include "cxtestTestNetwork.h"

cxtest::TestNetwork::TestNetwork() :
    ip("localhost"),
    port(18944)
{}

cxtest::TestNetwork::~TestNetwork()
{
    handle.reset();
}

void cxtest::TestNetwork::testDefaultNetworkConnection()
{
    cx::NetworkConnection* connection = handle->getNetworkConnection();
    REQUIRE(connection);
    REQUIRE(connection->getState() == cx::scsINACTIVE);
    REQUIRE(connection->getUid() == handleName);
    REQUIRE(connection->getAvailableDialects().size() == 0);
}

cxtest::TestClientNetwork::~TestClientNetwork()
{}

void cxtest::TestClientNetwork::setup()
{
    handleName = "client_handle";
    handle.reset(new cx::NetworkConnectionHandle(handleName, cx::XmlOptionFile()));
    REQUIRE(handle);
    CHECK(handle.unique());
    cx::NetworkConnection* connection = handle->getNetworkConnection();
    connect_function = boost::bind(&cx::NetworkConnection::requestConnect, connection);
    disconnect_function = boost::bind(&cx::NetworkConnection::requestDisconnect, connection);
}

void cxtest::TestClientNetwork::testDefaultConnectionInfo()
{
    cx::NetworkConnection* connection = handle->getNetworkConnection();
    cx::SocketConnection::ConnectionInfo connection_info = connection->getConnectionInfo();
    //check that default values have not changed
    CHECK(connection_info.role == "client");
    CHECK(connection_info.protocol.isEmpty());
    CHECK(connection_info.host == ip);
    CHECK(connection_info.port == port);
    CHECK(connection_info.isClient());
    CHECK_FALSE(connection_info.isServer());
    REQUIRE_FALSE(connection_info.getDescription().isEmpty());
}

void cxtest::TestClientNetwork::testConnectionAndExpectState(cx::CX_SOCKETCONNECTION_STATE state, int wait_ms)
{
    cx::NetworkConnection* connection = handle->getNetworkConnection();
    connection->invoke(connect_function);
    REQUIRE(waitForQueuedSignal(connection, SIGNAL(stateChanged(CX_SOCKETCONNECTION_STATE))));
    cx::sleep_ms(wait_ms);
    REQUIRE(connection->getState() == state);
}

void cxtest::TestClientNetwork::testDisconnect()
{
    cx::NetworkConnection* client_connection = handle->getNetworkConnection();
    client_connection->invoke(disconnect_function);
    REQUIRE(waitForQueuedSignal(client_connection, SIGNAL(stateChanged(CX_SOCKETCONNECTION_STATE))));
}

cxtest::TestServerNetwork::~TestServerNetwork()
{}

void cxtest::TestServerNetwork::setup()
{
    handleName = "server_handle";
    handle.reset(new cx::NetworkConnectionHandle(handleName, cx::XmlOptionFile()));
    REQUIRE(handle);
    CHECK(handle.unique());
    cx::NetworkConnection* server_connection = handle->getNetworkConnection();
    REQUIRE(server_connection);
    this->setAsServer(server_connection);
    connect_function = boost::bind(&cx::NetworkConnection::requestConnect, server_connection);
    disconnect_function = boost::bind(&cx::NetworkConnection::requestDisconnect, server_connection);
}

void cxtest::TestServerNetwork::testDefaultConnectionInfo()
{
    cx::NetworkConnection* connection = handle->getNetworkConnection();
    cx::SocketConnection::ConnectionInfo connection_info = connection->getConnectionInfo();
    //check that default values have not changed
    CHECK(connection_info.role == "server");
    CHECK(connection_info.protocol.isEmpty());
    CHECK(connection_info.host == ip);
    CHECK(connection_info.port == port);
    CHECK(connection_info.isServer());
    CHECK_FALSE(connection_info.isClient());
    REQUIRE_FALSE(connection_info.getDescription().isEmpty());
}

void cxtest::TestServerNetwork::testConnectionAndExpectState(cx::CX_SOCKETCONNECTION_STATE state, int wait_ms)
{
    cx::NetworkConnection* connection = handle->getNetworkConnection();
    connection->invoke(connect_function);
    REQUIRE(cxtest::waitForQueuedSignal(connection, SIGNAL(stateChanged(CX_SOCKETCONNECTION_STATE))));
    //a bit hard to time which state the server will be in since it is in another thread, give it some time
    cx::sleep_ms(wait_ms);
    REQUIRE( connection->getState() == state);
}

void cxtest::TestServerNetwork::testDisconnect()
{
    cx::NetworkConnection* connection = handle->getNetworkConnection();
    connection->invoke(disconnect_function);
    REQUIRE(cxtest::waitForQueuedSignal(connection, SIGNAL(stateChanged(CX_SOCKETCONNECTION_STATE))));
}

void cxtest::TestServerNetwork::setAsServer(cx::NetworkConnection *connection)
{
    cx::SocketConnection::ConnectionInfo connection_info = connection->getConnectionInfo();
    connection_info.role = "server";
    connection->setConnectionInfo(connection_info);
}
