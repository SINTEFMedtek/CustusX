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
#include "cxIGTLinkStreamerService.h"

#include "cxStringDataAdapterXml.h"
#include "cxDoubleDataAdapterXml.h"
#include "cxIGTLinkClientStreamer.h"

namespace cx
{

QString IGTLinkStreamerService::getName()
{
	return "OpenIGTLink_new"; // change to "Remote Server"
}

std::vector<DataAdapterPtr> IGTLinkStreamerService::getSettings(QDomElement root)
{
	std::vector<DataAdapterPtr> retval;
	retval.push_back(this->getIPOption(root));
	retval.push_back(this->getStreamPortOption(root));
	return retval;
}

StreamerPtr IGTLinkStreamerService::createStreamer(QDomElement root)
{
	boost::shared_ptr<IGTLinkClientStreamer> streamer(new IGTLinkClientStreamer());
	streamer->setAddress(this->getIPOption(root)->getValue(),
						 this->getStreamPortOption(root)->getValue());
	return streamer;

}

ReceiverPtr IGTLinkStreamerService::createReceiver(QDomElement root)
{
	return ReceiverPtr();
}

StringDataAdapterPtr IGTLinkStreamerService::getIPOption(QDomElement root)
{
	StringDataAdapterXmlPtr retval;
	QString defaultValue = "127.0.0.1";
	retval = StringDataAdapterXml::initialize("ip_scanner", "Address", "TCP/IP Address",
											  defaultValue, root);
	retval->setGroup("Connection");
	//		connect(retval.get(), &StringDataAdapterXml::valueWasSet,
	//		        this, &IGTLinkStreamerService::sendOptions);
	return retval;
}


DoubleDataAdapterPtr IGTLinkStreamerService::getStreamPortOption(QDomElement root)
{
	DoubleDataAdapterXmlPtr retval;
	retval = DoubleDataAdapterXml::initialize("ip_port", "Port", "TCP/IP Port (default 18333)",
											  18333, DoubleRange(1024, 49151, 1), 0, root);
	retval->setGuiRepresentation(DoubleDataAdapter::grSPINBOX);
	retval->setAdvanced(true);
	retval->setGroup("Connection");
	//		connect(retval.get(), &DoubleDataAdapterXml::valueWasSet,
	//		        this, &IGTLinkStreamerService::sendOptions);
	return retval;
}

} // namespace cx
