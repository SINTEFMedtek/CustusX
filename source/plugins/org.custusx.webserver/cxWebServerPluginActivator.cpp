/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#include "cxWebServerPluginActivator.h"

#include <QtPlugin>
#include <iostream>

//#include "cxNetworkServiceImpl.h"
#include "cxRegisteredService.h"
#include <QtConcurrent>
#include <QNetworkAccessManager>
#include <QHostAddress>
#include <qhttpserver.h>
#include <qhttprequest.h>
#include <qhttpresponse.h>
//#include "cxLogger.h"
#include "cxScreenVideoProvider.h"

#include "cxPatientModelService.h"

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
	std::cout << "STARTING WEB SERVER" << std::endl;
	server = new QHttpServer;
	connect(server, SIGNAL(newRequest(QHttpRequest*, QHttpResponse*)),
			this, SLOT(handle_request(QHttpRequest*, QHttpResponse*)));

	mScreenVideo = new ScreenVideoProvider(VisServices::create(context));
	// let's go
	server->listen(8085);

	//mRegistration = RegisteredService::create<NetworkServiceImpl>(context, NetworkService_iid);
}

void NetworkPluginActivator::handle_stream(QHttpRequest *req, QHttpResponse *resp)
{
	std::cout << "GOT WEB SERVER REQUEST: " << req->path().toStdString() << std::endl;
	new Responder(req, resp);
}

void NetworkPluginActivator::handle_users_reply(QHttpRequest *req, QHttpResponse *resp)
{
	std::cout << "GOT WEB SERVER REQUEST: " << req->path().toStdString() << std::endl;

	QRegExp exp("^/user/([a-z]+)$");
	if( exp.indexIn(req->path()) != -1 )
	{
		resp->setHeader("Content-Type", "text/html");
		resp->writeHead(200);

		QString name = exp.capturedTexts()[1];
		QString body = tr("<html><head><title>Greeting App</title></head><body><h1>Hello %1!</h1></body></html>");
		resp->end(body.arg(name).toUtf8());
	}
	else
	{
		resp->writeHead(403);
		resp->end(QByteArray("You aren't allowed here!"));
	}

}

void NetworkPluginActivator::handle_request(QHttpRequest *req, QHttpResponse *resp)
{
	std::cout << "GOT WEB SERVER REQUEST: " << req->path().toStdString() << std::endl;

	if (req->path().startsWith("/user"))
	{
		this->handle_stream(req, resp);
	}
	else if (req->path() == "/image/screenshot")
	{
		this->handle_screenshot(req, resp);
	}
	else if (req->path() == "/image/view3D")
	{
		this->handle_view3D(req, resp);
	}
	else if (req->path() == "/layout?list")
	{
//		this->handle_listLayout(req, resp);
	}
	else
	{
		this->handle_default(req, resp);
	}
}

void NetworkPluginActivator::handle_default(QHttpRequest *req, QHttpResponse *resp)
{
	std::cout << "handle_default" << std::endl;

	QString body("CustusX Web server - experimental!");
	QByteArray ba = body.toUtf8();

	resp->setHeader("Content-Type", "text/plain");
	resp->setHeader("Content-Length", QString::number(ba.size()));
	resp->writeHead(200); // everything is OK
	resp->write(ba);
	resp->end();
}

void NetworkPluginActivator::handle_view3D(QHttpRequest *req, QHttpResponse *resp)
{
	std::cout << "handle_view3D" << std::endl;

	QPixmap pm = mScreenVideo->grabScreen(0);
//	mScreenVideo->saveScreenShot(pm, "webimage");
	QByteArray ba = mScreenVideo->generatePNGEncoding(pm);

	resp->setHeader("Content-Type", "image/png");
	resp->setHeader("Content-Length", QString::number(ba.size()));
	resp->writeHead(200); // everything is OK
	resp->write(ba);
	resp->end();
}

void NetworkPluginActivator::handle_screenshot(QHttpRequest *req, QHttpResponse *resp)
{
	std::cout << "handle_screenshot" << std::endl;

	QPixmap pm = mScreenVideo->grabScreen(0);
//	mScreenVideo->saveScreenShot(pm, "webimage");
	QByteArray ba = mScreenVideo->generatePNGEncoding(pm);

	resp->setHeader("Content-Type", "image/png");
	resp->setHeader("Content-Length", QString::number(ba.size()));
	resp->writeHead(200); // everything is OK
	resp->write(ba);
	resp->end();
}

void NetworkPluginActivator::handle_helloworld(QHttpRequest *req, QHttpResponse *resp)
{
	std::cout << "GOT WEB SERVER REQUEST: " << req->path().toStdString() << std::endl;

	resp->setHeader("Content-Length", "11");
	resp->writeHead(200); // everything is OK
	resp->write("Hello World");
	resp->end();
}

void NetworkPluginActivator::stop(ctkPluginContext* context)
{
	mRegistration.reset();
	Q_UNUSED(context);
}





Responder::Responder(QHttpRequest *req, QHttpResponse *resp)
	: m_req(req)
	, m_resp(resp)
{
	QRegExp exp("^/user/([a-z]+$)");
	if (exp.indexIn(req->path()) == -1)
	{
		resp->writeHead(403);
		resp->end(QByteArray("You aren't allowed here!"));
		/// @todo There should be a way to tell request to stop streaming data
		return;
	}

	resp->setHeader("Content-Type", "text/html");
	resp->writeHead(200);

	QString name = exp.capturedTexts()[1];
	QString bodyStart = tr("<html><head><title>BodyData App</title></head><body><h1>Hello %1!</h1><p>").arg(name);
	resp->write(bodyStart.toUtf8());

	connect(req, SIGNAL(data(const QByteArray&)), this, SLOT(accumulate(const QByteArray&)));
	connect(req, SIGNAL(end()), this, SLOT(reply()));
	connect(m_resp, SIGNAL(done()), this, SLOT(deleteLater()));
}

Responder::~Responder()
{
	std::cout << "Responder::~Responder" << std::endl;
}

void Responder::accumulate(const QByteArray &data)
{
	std::cout << "Responder::accumulate" << std::endl;
	m_resp->write(data);
}

void Responder::reply()
{
	std::cout << "Responder::reply" << std::endl;
	m_resp->end(QByteArray("</p></body></html>"));
}


} // namespace cx



