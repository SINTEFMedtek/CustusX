/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxOpenIGTLinkStreamerService.h"
#include <memory>
#include "cxNetworkHandler.h"
#include "cxLogger.h"

namespace cx
{

OpenIGTLinkStreamerService::OpenIGTLinkStreamerService(NetworkHandlerPtr networkHandler) :
	mConnection(networkHandler),
	mConnectionAcquired(false)
{
	mStreamer = OpenIGTLinkStreamerPtr(new OpenIGTLinkStreamer());

	connect(mConnection.get(), &NetworkHandler::connected, mStreamer.get(), &OpenIGTLinkStreamer::receivedConnected);
	connect(mConnection.get(), &NetworkHandler::disconnected, mStreamer.get(), &OpenIGTLinkStreamer::receivedDisconnected);
	connect(mConnection.get(), &NetworkHandler::image, mStreamer.get(), &OpenIGTLinkStreamer::receivedImage);
	connect(mConnection.get(), &NetworkHandler::probedefinition, mStreamer.get(), &OpenIGTLinkStreamer::receiveProbedefinition);

	connect(mStreamer.get(), &OpenIGTLinkStreamer::stoppedStreaming, this, &OpenIGTLinkStreamerService::releaseConnection);
	connect(mStreamer.get(), &OpenIGTLinkStreamer::stoppedStreaming, mConnection.get(), &NetworkHandler::resendProbedefinition);
}

OpenIGTLinkStreamerService::~OpenIGTLinkStreamerService()
{
	this->releaseConnection();
}

QString OpenIGTLinkStreamerService::getName()
{
	return "OpenIGTLink 3 streamer";
}

QString OpenIGTLinkStreamerService::getType() const
{
	return OPENIGTLINK3_STREAMER;
}

std::vector<PropertyPtr> OpenIGTLinkStreamerService::getSettings(QDomElement root)
{
	std::vector<PropertyPtr> retval;
	retval.push_back(this->getIPOption(root));
	retval.push_back(this->getStreamPortOption(root));
	return retval;
}

StreamerPtr OpenIGTLinkStreamerService::createStreamer(QDomElement root)
{
	// Tracking over the same connection is started with the tracking service
	mConnectionAcquired = true;
	mConnection->acquireConnection("video", this->getIPOption(root)->getValue(), int(this->getStreamPortOption(root)->getValue()));
	return mStreamer;
}

void OpenIGTLinkStreamerService::stop()
{
	this->releaseConnection();
}

void OpenIGTLinkStreamerService::releaseConnection()
{
	if (mConnectionAcquired)
	{
		mConnectionAcquired = false;
		mConnection->releaseConnection("video");
	}
}

StringPropertyBasePtr OpenIGTLinkStreamerService::getIPOption(QDomElement root)
{
	StringPropertyPtr retval;
	QString defaultValue = "127.0.0.1";
	retval = StringProperty::initialize(OPENIGTLINK3_STREAMER_IP, "Address", "TCP/IP Address",
												defaultValue, root);
	retval->setGroup("Connection");
	return retval;
}

DoublePropertyBasePtr OpenIGTLinkStreamerService::getStreamPortOption(QDomElement root)
{
	DoublePropertyPtr retval;
	retval = DoubleProperty::initialize(OPENIGTLINK3_STREAMER_PORT, "Port", "TCP/IP Port (default 18944)",
												18944, DoubleRange(1024, 49151, 1), 0, root);
	retval->setGuiRepresentation(DoublePropertyBase::grSPINBOX);
	retval->setAdvanced(true);
	retval->setGroup("Connection");
	return retval;
}

} //namespace cx
