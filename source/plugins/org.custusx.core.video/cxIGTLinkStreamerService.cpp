/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxIGTLinkStreamerService.h"

#include "cxStringProperty.h"
#include "cxDoubleProperty.h"
#include "cxIGTLinkClientStreamer.h"

namespace cx
{

QString IGTLinkStreamerService::getName()
{
	return "Remote Server";
}

QString IGTLinkStreamerService::getType() const
{
	return "remote_server_streamer";
}

std::vector<PropertyPtr> IGTLinkStreamerService::getSettings(QDomElement root)
{
	std::vector<PropertyPtr> retval;
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


StringPropertyBasePtr IGTLinkStreamerService::getIPOption(QDomElement root)
{
	StringPropertyPtr retval;
	QString defaultValue = "127.0.0.1";
	retval = StringProperty::initialize("ip_scanner", "Address", "TCP/IP Address",
											  defaultValue, root);
	retval->setGroup("Connection");
	return retval;
}


DoublePropertyBasePtr IGTLinkStreamerService::getStreamPortOption(QDomElement root)
{
	DoublePropertyPtr retval;
	retval = DoubleProperty::initialize("ip_port", "Port", "TCP/IP Port (default 18333)",
											  18333, DoubleRange(1024, 49151, 1), 0, root);
	retval->setGuiRepresentation(DoublePropertyBase::grSPINBOX);
	retval->setAdvanced(true);
	retval->setGroup("Connection");
	return retval;
}

} // namespace cx
