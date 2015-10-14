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

#ifndef TESTNETWORK_H
#define TESTNETWORK_H

#include "catch.hpp"
#include <boost/bind.hpp>
#include "cxNetworkConnection.h"
#include "cxNetworkConnectionHandle.h"
#include "cxtestQueuedSignalListener.h"
#include "cxSocketConnection.h"
#include "cxUtilHelpers.h"


namespace cxtest
{
class TestNetwork
{
public:
    TestNetwork();

    virtual ~TestNetwork();

    virtual void setup() = 0;
    virtual void testDefaultNetworkConnection();
    virtual void testDefaultConnectionInfo() = 0;
    virtual void testConnectionAndExpectState(cx::CX_SOCKETCONNECTION_STATE state, int wait_ms) = 0;
    virtual void testDisconnect() = 0;

protected:
    QString ip;
    int port;
    QString handleName;
    cx::NetworkConnectionHandlePtr handle;
    boost::function<void()> connect_function;
    boost::function<void()> disconnect_function;
};

class TestClientNetwork : public TestNetwork
{
public:
    virtual ~TestClientNetwork();
    virtual void setup();
    virtual void testDefaultConnectionInfo();
    virtual void testConnectionAndExpectState(cx::CX_SOCKETCONNECTION_STATE state, int wait_ms);
    virtual void testDisconnect();
};

class TestServerNetwork : public TestNetwork
{
public:
    virtual ~TestServerNetwork();
    virtual void setup();
    virtual void testDefaultConnectionInfo();
    virtual void testConnectionAndExpectState(cx::CX_SOCKETCONNECTION_STATE state, int wait_ms);
    virtual void testDisconnect();

private:
    void setAsServer(cx::NetworkConnection* connection);
};
}

#endif // TESTNETWORK_H
