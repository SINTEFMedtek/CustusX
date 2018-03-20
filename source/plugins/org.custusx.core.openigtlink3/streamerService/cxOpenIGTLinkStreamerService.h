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
typedef boost::shared_ptr<class NetworkHandler> NetworkHandlerPtr;


class org_custusx_core_openigtlink3_EXPORT OpenIGTLinkStreamerService : public StreamerService
{

public:
	OpenIGTLinkStreamerService(NetworkHandlerPtr networkHandler);
    ~OpenIGTLinkStreamerService();

    virtual QString getName();
    virtual QString getType() const;
    virtual std::vector<PropertyPtr> getSettings(QDomElement root);
    virtual StreamerPtr createStreamer(QDomElement root);

private:
    OpenIGTLinkStreamerPtr mStreamer;
	NetworkHandlerPtr mConnection;
};
typedef boost::shared_ptr<OpenIGTLinkStreamerService> OpenIGTLinkStreamerServicePtr;
} //namespace cx

#endif //CXOPENIGTLINKSTREAMERSERVICE_H
