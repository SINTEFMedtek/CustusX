/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxOpenIGTLinkStreamerService.h"
//#include "cxNetworkConnection.h"
#include "cxNetworkHandler.h"

namespace cx
{

OpenIGTLinkStreamerService::OpenIGTLinkStreamerService(NetworkHandlerPtr networkHandler) :
	mConnection(networkHandler)
{
    mStreamer = OpenIGTLinkStreamerPtr(new OpenIGTLinkStreamer());

	connect(mConnection.get(), &NetworkHandler::connected, mStreamer.get(), &OpenIGTLinkStreamer::receivedConnected);
	connect(mConnection.get(), &NetworkHandler::disconnected, mStreamer.get(), &OpenIGTLinkStreamer::receivedDisconnected);
	connect(mConnection.get(), &NetworkHandler::image, mStreamer.get(), &OpenIGTLinkStreamer::receivedImage);
}

OpenIGTLinkStreamerService::~OpenIGTLinkStreamerService()
{

}

QString OpenIGTLinkStreamerService::getName()
{
	return "OpenIGTLink streamer 3";
}

QString OpenIGTLinkStreamerService::getType() const
{
	return "openigtlink_streamer3";
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
