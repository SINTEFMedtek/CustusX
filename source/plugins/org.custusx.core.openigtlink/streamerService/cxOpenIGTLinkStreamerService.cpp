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

#include "cxOpenIGTLinkStreamerService.h"
#include "cxNetworkConnection.h"
#include "cxNetworkConnectionHandle.h"

namespace cx
{

OpenIGTLinkStreamerService::OpenIGTLinkStreamerService(NetworkConnectionHandlePtr connection) :
	mConnection(connection)
{
    NetworkConnection* client = mConnection->getNetworkConnection();
    mStreamer = OpenIGTLinkStreamerPtr(new OpenIGTLinkStreamer());

	connect(client, &NetworkConnection::connected, mStreamer.get(), &OpenIGTLinkStreamer::receivedConnected);
	connect(client, &NetworkConnection::disconnected, mStreamer.get(), &OpenIGTLinkStreamer::receivedDisconnected);
	connect(client, &NetworkConnection::error, mStreamer.get(), &OpenIGTLinkStreamer::receivedError);
	connect(client, &NetworkConnection::image, mStreamer.get(), &OpenIGTLinkStreamer::receivedImage);
	connect(client, &NetworkConnection::usstatusmessage, mStreamer.get(), &OpenIGTLinkStreamer::receivedUSStatusMessage);
	connect(client, &NetworkConnection::igtlimage, mStreamer.get(), &OpenIGTLinkStreamer::receiveIgtlImage);
}

OpenIGTLinkStreamerService::~OpenIGTLinkStreamerService()
{

}

QString OpenIGTLinkStreamerService::getName()
{
    return "OpenIGTLink streamer";
}

QString OpenIGTLinkStreamerService::getType() const
{
    return "openigtlink_streamer";
}

std::vector<PropertyPtr> OpenIGTLinkStreamerService::getSettings(QDomElement root)
{
    std::vector<PropertyPtr> retval;
    return retval;
}

StreamerPtr OpenIGTLinkStreamerService::createStreamer(QDomElement root)
{
    return mStreamer;
}

} //namespace cx
