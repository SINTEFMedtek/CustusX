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
#include "cxHttpRequestHandler.h"

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

HttpRequestHandler::HttpRequestHandler(RemoteAPIPtr api) : mApi(api)
{
//	mScreenVideo = new ScreenVideoProvider(VisServices::create(context));
}

void HttpRequestHandler::handle_request(QHttpRequest *req, QHttpResponse *resp)
{
	/*
	   /                                                  : default page: short server info
	   /shot/screen                                       : get screenshot
	   /layout/list                                       : return list of all layouts
	   /layout/create?width=536,height=320,layout=mg_def  : create streamer display of given size and layout
	   /layout/shot                                       : get image of layout
	   /layout/start-stream?port=8086                     : start streamer on port
	   /layout/stop-stream?                               : stop streamer on port
	*/

	/*
	   GET    /                                              : default page: short server info
	   GET    /screen                                        : get screenshot
	   GET    /layout/                                       : return list of all layouts

	   PUT    /layout/display?width=536,height=320,layout=mg_def  : create layout display of given size and layout
	   GET    /layout/display                                  : get image of layout
	   DELETE /layout/display                                  : delete display

	   PUT    /layout/display/stream?port=8086                 : start streamer on port
	   DELETE /layout/display/stream                           : stop streamer on port
	*/

	std::cout << "GOT WEB SERVER REQUEST: " << req->path().toStdString() << std::endl;

//	if (req->path().startsWith("/user"))
//	{
//		this->handle_stream(req, resp);
//	}
	if (req->path() == "/image/screenshot")
	{
		this->handle_screenshot(req, resp);
	}
	else if (req->path() == "/image/viewshot")
	{
		this->handle_viewshot(req, resp);
//		this->handle_view3D(req, resp);
	}
	else if (req->path() == "/image/view3D")
	{
		mScreenVideo->showSecondaryLayout();
		this->handle_default(req, resp);
//		this->handle_view3D(req, resp);
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

//void HttpRequestHandler::handle_stream(QHttpRequest *req, QHttpResponse *resp)
//{
//	std::cout << "GOT WEB SERVER REQUEST: " << req->path().toStdString() << std::endl;
//	new Responder(req, resp);
//}

void HttpRequestHandler::handle_users_reply(QHttpRequest *req, QHttpResponse *resp)
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


void HttpRequestHandler::handle_default(QHttpRequest *req, QHttpResponse *resp)
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

void HttpRequestHandler::handle_view3D(QHttpRequest *req, QHttpResponse *resp)
{
	std::cout << "handle_view3D" << std::endl;

	QImage image = mScreenVideo->grabScreen(0).toImage();
	mScreenVideo->saveScreenShot(image, "webimage");
	QByteArray ba = mScreenVideo->generatePNGEncoding(image);

	resp->setHeader("Content-Type", "image/png");
	resp->setHeader("Content-Length", QString::number(ba.size()));
	resp->writeHead(200); // everything is OK
	resp->write(ba);
	resp->end();
}

void HttpRequestHandler::handle_viewshot(QHttpRequest *req, QHttpResponse *resp)
{
	std::cout << "handle_viewshot" << std::endl;

	QImage image = mScreenVideo->grabSecondaryLayout();
	mScreenVideo->saveScreenShot(image, "webimage_view");
	QByteArray ba = mScreenVideo->generatePNGEncoding(image);

	resp->setHeader("Content-Type", "image/png");
	resp->setHeader("Content-Length", QString::number(ba.size()));
	resp->writeHead(200); // everything is OK
	resp->write(ba);
	resp->end();
}

void HttpRequestHandler::handle_screenshot(QHttpRequest *req, QHttpResponse *resp)
{
	std::cout << "handle_screenshot" << std::endl;

	QImage image = mScreenVideo->grabScreen(0).toImage();
//	mScreenVideo->saveScreenShot(pm, "webimage");
	QByteArray ba = mScreenVideo->generatePNGEncoding(image);

	resp->setHeader("Content-Type", "image/png");
	resp->setHeader("Content-Length", QString::number(ba.size()));
	resp->writeHead(200); // everything is OK
	resp->write(ba);
	resp->end();
}

void HttpRequestHandler::handle_helloworld(QHttpRequest *req, QHttpResponse *resp)
{
	std::cout << "GOT WEB SERVER REQUEST: " << req->path().toStdString() << std::endl;

	resp->setHeader("Content-Length", "11");
	resp->writeHead(200); // everything is OK
	resp->write("Hello World");
	resp->end();
}

} // namespace cx
