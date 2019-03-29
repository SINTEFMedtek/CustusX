/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxWebServerPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxRegisteredService.h"
#include <QtConcurrent>
#include <QNetworkAccessManager>
#include <QHostAddress>
#include <qhttpserver.h>
#include <qhttprequest.h>
#include <qhttpresponse.h>
#include "cxScreenVideoProvider.h"

#include "cxPatientModelService.h"
#include "cxHttpRequestHandler.h"
#include "cxRemoteAPI.h"
#include "cxLogger.h"

#include <QPixmap>

namespace cx
{

NetworkPluginActivator::NetworkPluginActivator()
{
    std::cout << "Created NetworkPluginActivator" << std::endl;
}

NetworkPluginActivator::~NetworkPluginActivator()
{}

void NetworkPluginActivator::start(ctkPluginContext* context)
{
    int port = 8085;
    CX_LOG_INFO() << QString("Starting REST server on port %1...").arg(port);
	server = new QHttpServer;
	mAPI.reset(new RemoteAPI(VisServices::create(context)));
	mRequestHandler.reset(new HttpRequestHandler(mAPI));
	connect(server, SIGNAL(newRequest(QHttpRequest*, QHttpResponse*)),
			mRequestHandler.get(), SLOT(handle_request(QHttpRequest*, QHttpResponse*)));

	// let's go
    server->listen(port);

}

void NetworkPluginActivator::stop(ctkPluginContext* context)
{
	mRequestHandler.reset();
	mAPI.reset();

	server->close();
	delete server;
	server = NULL;

	Q_UNUSED(context);
}


} // namespace cx



