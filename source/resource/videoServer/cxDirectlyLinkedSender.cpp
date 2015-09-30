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

#include "cxDirectlyLinkedSender.h"

#include "cxIGTLinkConversion.h"
#include "cxLogger.h"

namespace cx
{

bool DirectlyLinkedSender::isReady() const
{
	return true;
}

void DirectlyLinkedSender::send(IGTLinkImageMessage::Pointer msg)
{
	if (!msg || !this->isReady())
		return;
	CX_LOG_CHANNEL_DEBUG("CA") << "DirectlyLinkedSender::send - decode image message";
	IGTLinkConversion converter;
	this->send(converter.decode(msg));
	if (mUnsentUSStatusMessage)
	{
		this->send(converter.decode(mUnsentUSStatusMessage, msg, mUSStatus));
		mUnsentUSStatusMessage = IGTLinkUSStatusMessage::Pointer();
	}
}

void DirectlyLinkedSender::send(IGTLinkUSStatusMessage::Pointer msg)
{
	if (!msg || !this->isReady())
		return;
	mUnsentUSStatusMessage = msg;
}

void DirectlyLinkedSender::send(ImagePtr msg)
{
	if (!this->isReady())
		return;

	mImage = msg;
	// decode color format:
	IGTLinkConversion converter;
	mImage = converter.decode(msg);

	emit newImage();
}

void DirectlyLinkedSender::send(ProbeDefinitionPtr msg)
{
	if (!this->isReady())
		return;
	// decode color format:
	IGTLinkConversion converter;
	mUSStatus = converter.decode(msg);
	emit newUSStatus();
}

ImagePtr DirectlyLinkedSender::popImage()
{
	return mImage;
}
ProbeDefinitionPtr DirectlyLinkedSender::popUSStatus()
{
	return mUSStatus;
}

}
