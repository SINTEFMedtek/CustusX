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
#include "cxStreamerService.h"
#include "cxOpenIGTLinkStreamer.h"


namespace cx
{
#define OPENIGTLINK3_STREAMER "openigtlink3_streamer"

typedef boost::shared_ptr<class NetworkHandler> NetworkHandlerPtr;
typedef boost::shared_ptr<class OpenIGTLinkTrackingSystemService> OpenIGTLinkTrackingSystemServicePtr;


class org_custusx_core_openigtlink3_EXPORT OpenIGTLinkStreamerService : public StreamerService
{

public:
	OpenIGTLinkStreamerService(NetworkHandlerPtr networkHandler, TrackingServicePtr trackingService);
	virtual ~OpenIGTLinkStreamerService();
	
	virtual QString getName();
	virtual QString getType() const;
	virtual std::vector<PropertyPtr> getSettings(QDomElement root);
	virtual StreamerPtr createStreamer(QDomElement root);

	BoolPropertyBasePtr trackAndStream(QDomElement root);
	virtual void stop();
	
public slots:
	void stopTrackingAndOpenIGTLinkClientIfStartedFromThisObject();

private:
	OpenIGTLinkStreamerPtr mStreamer;
	NetworkHandlerPtr mConnection;
	TrackingServicePtr mTrackingService;
	bool mStartedTrackingAndOpenIGTLinkFromHere;

	StringPropertyBasePtr getIPOption(QDomElement root);
	DoublePropertyBasePtr getStreamPortOption(QDomElement root);
	OpenIGTLinkTrackingSystemServicePtr getOpenIGTLinkTrackingSystemService();
	void startTracking(QDomElement root);
	void configureTracking(QDomElement root);
};
} //namespace cx

#endif //CXOPENIGTLINKSTREAMERSERVICE_H
