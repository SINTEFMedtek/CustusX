/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXNETWORKPLUGINACTIVATOR_H_
#define CXNETWORKPLUGINACTIVATOR_H_

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"

class QHttpServer;
class QHttpRequest;
class QHttpResponse;

namespace cx
{
//class ScreenVideoProvider;
typedef boost::shared_ptr<class RemoteAPI> RemoteAPIPtr;
typedef boost::shared_ptr<class HttpRequestHandler> HttpRequestHandlerPtr;
//typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;

/**
 */
class NetworkPluginActivator :  public QObject, public ctkPluginActivator
{
	Q_OBJECT
	Q_INTERFACES(ctkPluginActivator)
    Q_PLUGIN_METADATA(IID "org_custusx_webserver")

public:

    NetworkPluginActivator();
    ~NetworkPluginActivator();

	void start(ctkPluginContext* context);
	void stop(ctkPluginContext* context);


private:
	HttpRequestHandlerPtr mRequestHandler;
	RemoteAPIPtr mAPI;
	QHttpServer *server;
};

} // namespace cx

#endif /* CXNETWORKPLUGINACTIVATOR_H_ */
