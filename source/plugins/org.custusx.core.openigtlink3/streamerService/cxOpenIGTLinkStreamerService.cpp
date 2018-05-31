/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

	connect(mStreamer.get(), &OpenIGTLinkStreamer::stoppedStreaming, this, &OpenIGTLinkStreamerService::stopTrackingAndOpenIGTLinkClientIfStartedFromThisObject);
}

OpenIGTLinkStreamerService::~OpenIGTLinkStreamerService()
{
	this->stopTrackingAndOpenIGTLinkClientIfStartedFromThisObject();
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
	retval.push_back(this->trackAndStream(root));

	return retval;
}

StreamerPtr OpenIGTLinkStreamerService::createStreamer(QDomElement root)
{
	this->startTracking(root);
	return mStreamer;
}

void OpenIGTLinkStreamerService::stop()
{
	this->stopTrackingAndOpenIGTLinkClientIfStartedFromThisObject();
}

void OpenIGTLinkStreamerService::stopTrackingAndOpenIGTLinkClientIfStartedFromThisObject()
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
	if(this->trackAndStream(root)->getValue())
	{
		mStartedTrackingAndOpenIGTLinkFromHere = true;
		this->configureTracking(root);
		// Trying to connect will cause several tests to fail,
		// because ClientSocket::ConnectToServer in OpenIGTLink/OpenIGTLink/Source/igtlClientSocket.cxx
		// will print error messages when it cannot connect.
		// Default value for trackAndStream (named start_tracking in settings xml file,
		// and defined as OPENIGTLINK3_STREAMER_START_TRACKING in code) is therefore set to false.
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

BoolPropertyBasePtr OpenIGTLinkStreamerService::trackAndStream(QDomElement root)
{
	BoolPropertyPtr retval;
	// Default value need to be false to prevent tests from failing
	retval = BoolProperty::initialize(OPENIGTLINK3_STREAMER_START_TRACKING, "Also Start OpenIGTLink client and Tracking",
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
	retval = StringProperty::initialize(OPENIGTLINK3_STREAMER_IP, "Address", "TCP/IP Address",
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
