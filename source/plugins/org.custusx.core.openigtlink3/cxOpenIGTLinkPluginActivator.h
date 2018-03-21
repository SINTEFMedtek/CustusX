/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXOpenIGTLinkPluginActivator_H_
#define CXOpenIGTLinkPluginActivator_H_

#include "igtlioBaseConverter.h"

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"
#include <QThread>

#include "cxNetworkHandler.h"

namespace cx
{

typedef boost::shared_ptr<class NetworkConnectionHandle> NetworkConnectionHandlePtr;
typedef boost::shared_ptr<class OpenIGTLinkTrackingSystemService> OpenIGTLinkTrackingSystemServicePtr;
typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;
typedef boost::shared_ptr<class NetworkServiceImpl> NetworkServiceImplPtr;

/**
 * Activator for the OpenIGTLink service
 *
 * \ingroup org_custusx_core_openigtlink
 *
 * \date 2015-03-03
 * \author Janne Beate Bakeng
 */
class OpenIGTLinkPluginActivator :  public QObject, public ctkPluginActivator
{
    Q_OBJECT
    Q_INTERFACES(ctkPluginActivator)
	Q_PLUGIN_METADATA(IID "org_custusx_core_openigtlink3")

public:

    OpenIGTLinkPluginActivator();
    ~OpenIGTLinkPluginActivator();

    void start(ctkPluginContext* context);
    void stop(ctkPluginContext* context);

private:
    RegisteredServicePtr mRegistrationGui;
    NetworkHandlerPtr mNetworkHandler;
	RegisteredServicePtr mRegistrationTracking;
	RegisteredServicePtr mRegistrationStreaming;

    NetworkServiceImplPtr mNetworkConnections;
};

} // namespace cx

#endif /* CXOpenIGTLinkPluginActivator_H_ */
