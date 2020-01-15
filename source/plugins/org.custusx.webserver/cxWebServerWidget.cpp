/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxWebServerWidget.h"

#include <qhttpserver.h>
#include <qhttprequest.h>
#include <qhttpresponse.h>
#include "cxHttpRequestHandler.h"
#include "cxRemoteAPI.h"

#include "cxLogger.h"

namespace cx
{

WebServerWidget::WebServerWidget(VisServicesPtr services, QWidget* parent) :
	WebServerWidgetBase(services, parent)
{
	this->setObjectName("WebServerWidget");
	this->setWindowTitle("Web Server");
	this->setWhatsThis(this->defaultWhatsThis());

	this->initServer();
}

WebServerWidget::~WebServerWidget()
{
	this->shutdownServer();
}

void WebServerWidget::initServer()
{
	mPort = 8085;
	CX_LOG_INFO() << QString("Starting REST server on port %1...").arg(mPort);
	mServer = new QHttpServer(this);
	mAPI.reset(new RemoteAPI(mServices));
	mRequestHandler.reset(new HttpRequestHandler(mAPI));
	connect(mServer, SIGNAL(newRequest(QHttpRequest*, QHttpResponse*)),
					mRequestHandler.get(), SLOT(handle_request(QHttpRequest*, QHttpResponse*)));

}

void WebServerWidget::shutdownServer()
{
	mRequestHandler.reset();
	mAPI.reset();

	mServer->close();
	delete mServer;
	mServer = nullptr;
	//Various crash at this point:  pointer being freed was not allocated, or seg. fault.
}

void WebServerWidget::startServer()
{
	mServer->listen(mPort);
}

void WebServerWidget::stopServer()
{
	mServer->close();
}

QString WebServerWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>WebServer plugin.</h3>"
			"<p>Used to run a WebServer from CustusX</p>"
      "</html>";
}



} /* namespace cx */
