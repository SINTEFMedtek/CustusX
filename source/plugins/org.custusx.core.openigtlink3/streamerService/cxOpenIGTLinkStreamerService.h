/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXOPENIGTLINKSTREAMERSERVICE_H
#define CXOPENIGTLINKSTREAMERSERVICE_H

#include "org_custusx_core_openigtlink3_Export.h"
#include <memory>
#include "cxStreamerService.h"
#include "cxOpenIGTLinkStreamer.h"


namespace cx
{
#define OPENIGTLINK3_STREAMER "openigtlink3_streamer"
#define OPENIGTLINK3_STREAMER_IP "ip_scanner_openigtlink"
#define OPENIGTLINK3_STREAMER_PORT "ip_port"

typedef std::shared_ptr<class NetworkHandler> NetworkHandlerPtr;


class org_custusx_core_openigtlink3_EXPORT OpenIGTLinkStreamerService : public StreamerService
{

public:
	explicit OpenIGTLinkStreamerService(NetworkHandlerPtr networkHandler);
	virtual ~OpenIGTLinkStreamerService();
	
	virtual QString getName();
	virtual QString getType() const;
	virtual std::vector<PropertyPtr> getSettings(QDomElement root);
	virtual StreamerPtr createStreamer(QDomElement root);

	virtual void stop();
	
private slots:
	void releaseConnection();

private:
	OpenIGTLinkStreamerPtr mStreamer;
	NetworkHandlerPtr mConnection;
	bool mConnectionAcquired;

	StringPropertyBasePtr getIPOption(QDomElement root);
	DoublePropertyBasePtr getStreamPortOption(QDomElement root);
};
} //namespace cx

#endif //CXOPENIGTLINKSTREAMERSERVICE_H
