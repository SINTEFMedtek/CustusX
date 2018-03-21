/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxOpenIGTLinkStreamer.h"

#include "cxLogger.h"
#include "cxSender.h"

namespace cx
{

OpenIGTLinkStreamer::OpenIGTLinkStreamer()
{}

OpenIGTLinkStreamer::~OpenIGTLinkStreamer()
{}


void OpenIGTLinkStreamer::startStreaming(SenderPtr sender)
{
	mSender = sender;
}

void OpenIGTLinkStreamer::stopStreaming()
{
    mSender.reset();
		emit stoppedStreaming();
}

bool OpenIGTLinkStreamer::isStreaming()
{
    return true;
}

QString OpenIGTLinkStreamer::getType()
{
    return "OpenIGTLinkStreamer";
}

void OpenIGTLinkStreamer::receivedConnected()
{
}

void OpenIGTLinkStreamer::receivedDisconnected()
{
}

void OpenIGTLinkStreamer::receivedError()
{
}

void OpenIGTLinkStreamer::receivedImage(ImagePtr image)
{
	//CX_LOG_DEBUG() << "receivedImage";
    PackagePtr package(new Package());
    package->mImage = image;
    if(mSender)
        mSender->send(package);
}

void OpenIGTLinkStreamer::receivedProbedefinition(QString not_used, ProbeDefinitionPtr probedef)
{
	//CX_LOG_DEBUG() << "receivedProbedefinition";
    PackagePtr package(new Package());
    package->mProbe = probedef;
    if(mSender)
        mSender->send(package);
}


void OpenIGTLinkStreamer::streamSlot()
{

}

} // namespace cx


