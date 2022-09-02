/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXWEBSERVERGUIEXTENDERSERVICE_H_
#define CXWEBSERVERGUIEXTENDERSERVICE_H_

#include "cxGUIExtenderService.h"
#include "org_custusx_webserver_Export.h"
class ctkPluginContext;

namespace cx
{

typedef boost::shared_ptr<class RemoteAPI> RemoteAPIPtr;
typedef boost::shared_ptr<class HttpRequestHandler> HttpRequestHandlerPtr;

/**
 * Implementation of WebServer service.
 *
 * \ingroup org_custusx_webserver
 *
 * \date 2019-03-29
 * \author Ole Vegard Solberg
 */
class org_custusx_webserver_EXPORT WebServerGUIExtenderService : public GUIExtenderService
{
	Q_INTERFACES(cx::GUIExtenderService)
public:
	WebServerGUIExtenderService(ctkPluginContext *context);
	virtual ~WebServerGUIExtenderService() {};

	std::vector<CategorizedWidget> createWidgets() const;

private:
	VisServicesPtr mVisServices;
};
typedef boost::shared_ptr<WebServerGUIExtenderService> WebServerGUIExtenderServicePtr;

} /* namespace cx */

#endif /* CXWEBSERVERGUIEXTENDERSERVICE_H_ */

