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
#include "cxOpenIGTLinkStreamer.h"

#include "cxLogger.h"
#include "cxSender.h"

namespace cx
{

OpenIGTLinkStreamer::OpenIGTLinkStreamer()
{}

OpenIGTLinkStreamer::~OpenIGTLinkStreamer()
{}


bool OpenIGTLinkStreamer::startStreaming(SenderPtr sender)
{
	mSender = sender;
	return true;
}

void OpenIGTLinkStreamer::stopStreaming()
{
	mSender.reset();
}

QString OpenIGTLinkStreamer::getType()
{
    return "OpenIGTLinkStreamer";
}

void OpenIGTLinkStreamer::receivedConnected()
{
    CX_LOG_CHANNEL_DEBUG("janne beate ") << "Connected received";
}

void OpenIGTLinkStreamer::receivedDisconnected()
{
    CX_LOG_CHANNEL_DEBUG("janne beate ") << "Disconnect received";
}

void OpenIGTLinkStreamer::receivedError()
{
    CX_LOG_CHANNEL_DEBUG("janne beate ") << "Error received";
}

void OpenIGTLinkStreamer::receivedImage(ImagePtr image)
{
    PackagePtr package(new Package());
    package->mImage = image;
    if(mSender)
        mSender->send(package);
}

void OpenIGTLinkStreamer::receiveIgtlImage(IGTLinkImageMessage::Pointer igtlimage)
{
    PackagePtr package(new Package());
    package->mIgtLinkImageMessage = igtlimage;
    // if us status not sent, do it here
    if (mUnsentUSStatusMessage)
    {
        package->mIgtLinkUSStatusMessage = mUnsentUSStatusMessage;
        mUnsentUSStatusMessage = IGTLinkUSStatusMessage::Pointer();
    }
    if(mSender)
    {
        CX_LOG_DEBUG() << "sender custus pakke";
        mSender->send(package);
    }
}

void OpenIGTLinkStreamer::receivedUSStatusMessage(IGTLinkUSStatusMessage::Pointer message)
{
    mUnsentUSStatusMessage = message;
}

void OpenIGTLinkStreamer::streamSlot()
{

}

} // namespace cx


