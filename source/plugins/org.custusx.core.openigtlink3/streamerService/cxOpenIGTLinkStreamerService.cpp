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

#include <QFileInfo>

#include "cxNetworkHandler.h"
#include "cxLogger.h"
#include "cxProfile.h"
#include "cxTrackingService.h"
#include "cxOpenIGTLinkTrackingSystemService.h"

namespace cx
{

OpenIGTLinkStreamerService::OpenIGTLinkStreamerService(NetworkHandlerPtr networkHandler, TrackingServicePtr trackingService) :
	mConnection(networkHandler),
	mTrackingService(trackingService),
	mStartedTrackingAndOpenIGTLinkFromHere(false)
{
    mStreamer = OpenIGTLinkStreamerPtr(new OpenIGTLinkStreamer());

	connect(mConnection.get(), &NetworkHandler::connected, mStreamer.get(), &OpenIGTLinkStreamer::receivedConnected);
	connect(mConnection.get(), &NetworkHandler::disconnected, mStreamer.get(), &OpenIGTLinkStreamer::receivedDisconnected);
	connect(mConnection.get(), &NetworkHandler::image, mStreamer.get(), &OpenIGTLinkStreamer::receivedImage);

	connect(mStreamer.get(), &OpenIGTLinkStreamer::stoppedStreaming, this, &OpenIGTLinkStreamerService::stopTrackingAndOpenIGTLinkClient);
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
	mCombinedFunctionality = this->getCombinedFunctionality(root);
		std::vector<PropertyPtr> retval;
		retval.push_back(this->getIPOption(root));
		retval.push_back(this->getStreamPortOption(root));
		retval.push_back(mCombinedFunctionality);

    return retval;
}

StreamerPtr OpenIGTLinkStreamerService::createStreamer(QDomElement root)
{
	this->startTracking(root);
	return mStreamer;
}

void OpenIGTLinkStreamerService::stopTrackingAndOpenIGTLinkClient()
{
	if(mStartedTrackingAndOpenIGTLinkFromHere)
	{
		mConnection->disconnectFromServer();

		OpenIGTLinkTrackingSystemServicePtr trackingSystemService = this->getOpenIGTLinkTrackingSystemService();
		if(trackingSystemService)
			trackingSystemService->setState(Tool::tsNONE);

		mStartedTrackingAndOpenIGTLinkFromHere = false;
	}
}

void OpenIGTLinkStreamerService::startTracking(QDomElement root)
{
	if(this->getCombinedFunctionality(root)->getValue())
	{
		mStartedTrackingAndOpenIGTLinkFromHere = true;
		this->configureTracking(root);
		// Trying to connect will case several tests to fail,
		// because ClientSocket::ConnectToServer in OpenIGTLink/OpenIGTLink/Source/igtlClientSocket.cxx
		// will print error messages when it cannot connect.
		// Default value for CombinedFunctionality (named start_tracking in settings xml file) are therefore set to false
		mConnection->requestConnectToServer(this->getIPOption(root)->getValue().toStdString(),
																				int(this->getStreamPortOption(root)->getValue()));
	}
}

void OpenIGTLinkStreamerService::configureTracking(QDomElement root)
{
	Q_UNUSED(root);
	OpenIGTLinkTrackingSystemServicePtr trackingSystemService = this->getOpenIGTLinkTrackingSystemService();
	if(trackingSystemService)
	{
		QFileInfo fileInfo(profile()->getToolConfigFilePath());
		trackingSystemService->setConfigurationFile(fileInfo.filePath());//Set full path to current tool config file
		trackingSystemService->setState(Tool::tsCONFIGURED);
	}
}

OpenIGTLinkTrackingSystemServicePtr OpenIGTLinkStreamerService::getOpenIGTLinkTrackingSystemService()
{
	std::vector<TrackingSystemServicePtr> trackingSystems = mTrackingService->getTrackingSystems();
	for (unsigned i = 0; i < trackingSystems.size(); ++i)
	{
		OpenIGTLinkTrackingSystemServicePtr trackingSystemService = boost::dynamic_pointer_cast<OpenIGTLinkTrackingSystemService>(trackingSystems[i]);
		if(trackingSystemService)
			return trackingSystemService;
	}
	return OpenIGTLinkTrackingSystemServicePtr();
}

BoolPropertyBasePtr OpenIGTLinkStreamerService::getCombinedFunctionality(QDomElement root)
{
	BoolPropertyPtr retval;
	// Default value need to be false to prevent tests from failing
	retval = BoolProperty::initialize("start_tracking", "Also Start OpenIGTLink client and Tracking",
																		"Combined functionality: \n"
																		"Run both tracking and streaming over OpenIGTLink",
																		false, root);
	retval->setGroup("Connection");

	return retval;
}

StringPropertyBasePtr OpenIGTLinkStreamerService::getIPOption(QDomElement root)
{
	StringPropertyPtr retval;
	QString defaultValue = "127.0.0.1";
	retval = StringProperty::initialize("ip_scanner", "Address", "TCP/IP Address",
												defaultValue, root);
	retval->setGroup("Connection");
	return retval;
}

DoublePropertyBasePtr OpenIGTLinkStreamerService::getStreamPortOption(QDomElement root)
{
	DoublePropertyPtr retval;
	retval = DoubleProperty::initialize("ip_port", "Port", "TCP/IP Port (default 18944)",
												18944, DoubleRange(1024, 49151, 1), 0, root);
	retval->setGuiRepresentation(DoublePropertyBase::grSPINBOX);
	retval->setAdvanced(true);
	retval->setGroup("Connection");
	return retval;
}

} //namespace cx
