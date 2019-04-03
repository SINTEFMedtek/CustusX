/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXWEBSERVERWIDGET_H_
#define CXWEBSERVERWIDGET_H_

#include "cxWebServerWidgetBase.h"

class QHttpServer;
class QHttpRequest;
class QHttpResponse;

namespace cx
{

typedef boost::shared_ptr<class RemoteAPI> RemoteAPIPtr;
typedef boost::shared_ptr<class HttpRequestHandler> HttpRequestHandlerPtr;

/**
 * Widget for use by the Webserver plugin
 *
 * \ingroup org_custusx_webserver
 *
 * \date 2019-03-29
 * \author Ole Vegard Solberg
 */
class WebServerWidget : public WebServerWidgetBase
{
//	Q_OBJECT
public:
	WebServerWidget(cx::VisServicesPtr services, QWidget* parent = nullptr);
	virtual ~WebServerWidget();

protected:
	virtual void startServer();
	virtual void stopServer();

private:
	HttpRequestHandlerPtr mRequestHandler;
	RemoteAPIPtr mAPI;
	QHttpServer *mServer;
	quint16 mPort;

	void initServer();
	void shutdownServer();
	QString defaultWhatsThis() const;
};

} /* namespace cx */

#endif /* CXWEBSERVERWIDGET_H_ */
