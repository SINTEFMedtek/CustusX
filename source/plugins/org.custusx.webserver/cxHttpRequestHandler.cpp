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
#include "cxLogger.h"

#include "cxPatientModelService.h"
#include "cxRemoteAPI.h"
#include <QPixmap>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>

namespace cx
{

HttpRequestHandler::HttpRequestHandler(RemoteAPIPtr api) : mApi(api)
{
}

void HttpRequestHandler::handle_request(QHttpRequest *req, QHttpResponse *resp)
{
    /* v2 - insert into front page
	   GET    /                                              : default page: short server info
	   GET    /screen                                        : get screenshot
	   GET    /layout/                                       : return list of all layouts

	   PUT    /layout/display?width=536,height=320,layout=mg_def  : create layout display of given size and layout
	   GET    /layout/display                                  : get image of layout
	   DELETE /layout/display                                  : delete display

	   PUT    /layout/display/stream?port=8086                 : start streamer on port
	   DELETE /layout/display/stream                           : stop streamer on port
	*/

    CX_LOG_DEBUG() << QString("Web server request [%1] from [%2]")
                      .arg(req->path())
                      .arg(req->remoteAddress());

    if (req->path() == "/")
    {
        this->process_mainpage(req, resp);
    }
    else if (req->path().startsWith("/screen"))
	{
        this->handle_screen(req, resp);
	}
    else if (req->path().startsWith("/layout"))
	{
        this->handle_layout(req, resp);
	}
	else
	{
        this->reply_notfound(resp);
	}
}

void HttpRequestHandler::handle_layout(QHttpRequest *req, QHttpResponse *resp)
{
    std::cout << "handle_layout" << std::endl;
    CX_ASSERT(req->path().startsWith("/layout"));

    if (req->path() == "/layout")
    {
        this->process_layout(req, resp);
    }
    else if (req->path()=="/layout/display/stream")
    {
        this->process_stream(req, resp);
    }
    else if (req->path() == "/layout/display")
    {
        this->process_display(req, resp);
    }
    else
    {
        this->reply_notfound(resp);
    }
}

void HttpRequestHandler::process_layout(QHttpRequest *req, QHttpResponse *resp)
{
    CX_ASSERT(req->path()=="/layout");
    std::cout << "process_layout" << std::endl;
//    GET    /layout/                                       : return list of all layouts

    if (req->method()==QHttpRequest::HTTP_GET)
    {
        this->reply_layout_list(resp);
    }
    else
    {
        this->reply_method_not_allowed(resp);
    }
}

void HttpRequestHandler::process_stream(QHttpRequest *req, QHttpResponse *resp)
{
    CX_ASSERT(req->path()=="/layout/display/stream");
    std::cout << "process_stream" << std::endl;
//    PUT    /layout/display/stream?port=8086                 : start streamer on port
//    DELETE /layout/display/stream                           : stop streamer on port

    if (req->method()==QHttpRequest::HTTP_PUT)
    {
        this->create_stream(req, resp);
    }
    else if (req->method()==QHttpRequest::HTTP_DELETE)
    {
        this->delete_stream(resp);
    }
    else
    {
        this->reply_method_not_allowed(resp);
    }
}

void HttpRequestHandler::process_display(QHttpRequest *req, QHttpResponse *resp)
{
    CX_ASSERT(req->path()=="/layout/display");
    std::cout << "process_display" << std::endl;

//    PUT    /layout/display?width=536,height=320,layout=mg_def  : create layout display of given size and layout
//    GET    /layout/display                                  : get image of layout
//    DELETE /layout/display                                  : delete display

    if (req->method()==QHttpRequest::HTTP_GET)
    {
        this->get_display_image(resp);
    }
    else if (req->method()==QHttpRequest::HTTP_PUT)
    {
        this->create_display(req, resp);
    }
    else if (req->method()==QHttpRequest::HTTP_DELETE)
    {
        this->delete_display(resp);
    }
    else
    {
        this->reply_method_not_allowed(resp);
    }

}

void HttpRequestHandler::reply_layout_list(QHttpResponse *resp)
{
    QStringList layouts = mApi->getAvailableLayouts();

//    {
//        "Layouts": [
//            "ACS_3D",
//            "AnyDual_3D"
//        ]
//    }

    QJsonObject root;
    root.insert("Layouts", QJsonArray::fromStringList(layouts));
    QByteArray ba = QJsonDocument(root).toJson();

    resp->setHeader("Content-Type", "application/json");
    resp->setHeader("Content-Length", QString::number(ba.size()));
    resp->writeHead(200); // everything is OK
    resp->write(ba);
    resp->end();
}

void HttpRequestHandler::get_display_image(QHttpResponse *resp)
{
    QImage image = mApi->grabLayout();
    QByteArray ba = this->generatePNGEncoding(image);

    resp->setHeader("Content-Type", "image/png");
    resp->setHeader("Content-Length", QString::number(ba.size()));
    resp->writeHead(200); // everything is OK
    resp->write(ba);
    resp->end();
}

void HttpRequestHandler::create_display(QHttpRequest *req, QHttpResponse *resp)
{
    // example test line:
    // curl -H 'Content-Type: application/json' -X PUT -d '{"width":"600","height":"300","layout":"LAYOUT_GROUP_RT"}' http://localhost:8085/layout/display
//    QByteArray body = req->body();
    QJsonDocument doc = QJsonDocument::fromJson(req->body());
    CX_LOG_CHANNEL_DEBUG("CA") << "JSON: " << QString(req->body());
    QSize size;
    size.setWidth(doc.object()["width"].toInt());
    size.setHeight(doc.object()["height"].toInt());
    QString layout = doc.object()["layout"].toString();
    CX_LOG_CHANNEL_DEBUG("CA") << "size " << size.width() << "," << size.height() << ", layout " << layout;

    mApi->createLayoutWidget(size, layout);

    resp->writeHead(200); // everything is OK
    resp->end();
}

void HttpRequestHandler::delete_display(QHttpResponse *resp)
{
    mApi->closeLayoutWidget();
}

QByteArray HttpRequestHandler::generatePNGEncoding(QImage image)
{
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG"); // writes image into ba in PNG format//	QString ending = "png";
    return ba;
}

void HttpRequestHandler::create_stream(QHttpRequest *req, QHttpResponse *resp)
{
    this->reply_notfound(resp); // TODO: create streamer
}

void HttpRequestHandler::delete_stream(QHttpResponse *resp)
{
    this->reply_notfound(resp); // TODO: delete streamer
}

void HttpRequestHandler::process_mainpage(QHttpRequest *req, QHttpResponse *resp)
{
    CX_ASSERT(req->path()=="/");

    if (req->method()==QHttpRequest::HTTP_GET)
    {
        this->reply_mainpage(resp);
    }
    else
    {
        this->reply_method_not_allowed(resp);
    }
}

void HttpRequestHandler::reply_mainpage(QHttpResponse *resp)
{
    QString body("</body>"
                 "CustusX REST API"
                 "<p>"
                 "Arguments are passed using JSON format."
                 "</p>"
                 ""
                 "<table border=\"1\">"
                 "<tr><th>method</th><th>resource</th><th>description</th><th>arguments</th></tr>"
                 ""
                 "<tr><td>GET</td><td>/</td><td>main page: short server info</td><td>html page</td></tr>"
                 "<tr><td>GET</td><td>/screen</td><td>get screenshot</td><td>png image</td></tr>"
                 "<tr><td>GET</td><td>/layout</td><td>return list of all layouts</td><td>layouts</td></tr>"
                 ""
                 "<tr>"
                 "<td>PUT</td><td>/layout/display</td>"
                 "<td>create layout display of given size and layout.</td>"
                 "<td>width=(int),height=(int),layout=(uid)</td>"
                 "</tr>"
                 "<tr><td>GET</td><td>/layout/display</td><td>get image of layout</td><td>png image</td></tr>"
                 "<tr><td>DELETE</td><td>/layout/display</td><td>delete display</td></tr>"
                 ""
                 "<tr>"
                 "<td>PUT</td><td>/layout/display/stream</td>"
                 "<td>start streamer on port </td>"
                 "<td>port=(int)</td>"
                 "</tr>"
                 "<tr><td>DELETE</td><td>/layout/display/stream</td><td>stop streamer</td></tr>"
                 ""
                 "</table>"
                 "</body>"
                 "");

    QByteArray ba = body.toUtf8();

    resp->setHeader("Content-Type", "text/html");
    resp->setHeader("Content-Length", QString::number(ba.size()));
    resp->writeHead(200); // everything is OK
    resp->write(ba);
    resp->end();
}

void HttpRequestHandler::handle_screen(QHttpRequest *req, QHttpResponse *resp)
{
    std::cout << "handle_screen" << std::endl;
    CX_ASSERT(req->path().startsWith("/screen"));

    if (req->path() == "/screen")
    {
        this->process_screen(req, resp);
    }
    else
    {
        this->reply_notfound(resp);
    }
}

void HttpRequestHandler::reply_notfound(QHttpResponse *resp)
{
    resp->writeHead(404);
    resp->end(QByteArray("Not found"));
}

void HttpRequestHandler::reply_method_not_allowed(QHttpResponse *resp)
{
    resp->writeHead(405);
    resp->end(QByteArray("Method Not Allowed"));
}

void HttpRequestHandler::process_screen(QHttpRequest *req, QHttpResponse *resp)
{
    CX_ASSERT(req->path()=="/screen");
    std::cout << "handle_screenshot" << std::endl;
//    GET    /screen                                        : get screenshot

    if (req->method()==QHttpRequest::HTTP_GET)
    {
        this->reply_screenshot(resp);
    }
    else
    {
        this->reply_method_not_allowed(resp);
    }
}

void HttpRequestHandler::reply_screenshot(QHttpResponse *resp)
{
    QImage image = mApi->grabScreen();
    QByteArray ba = this->generatePNGEncoding(image);

    resp->setHeader("Content-Type", "image/png");
    resp->setHeader("Content-Length", QString::number(ba.size()));
    resp->writeHead(200); // everything is OK
    resp->write(ba);
    resp->end();
}

//void HttpRequestHandler::handle_helloworld(QHttpRequest *req, QHttpResponse *resp)
//{
//	std::cout << "GOT WEB SERVER REQUEST: " << req->path().toStdString() << std::endl;

//	resp->setHeader("Content-Length", "11");
//	resp->writeHead(200); // everything is OK
//	resp->write("Hello World");
//	resp->end();
//}

//void HttpRequestHandler::handle_stream(QHttpRequest *req, QHttpResponse *resp)
//{
//	std::cout << "GOT WEB SERVER REQUEST: " << req->path().toStdString() << std::endl;
//	new Responder(req, resp);
//}

//void HttpRequestHandler::handle_users_reply(QHttpRequest *req, QHttpResponse *resp)
//{
//	std::cout << "GOT WEB SERVER REQUEST: " << req->path().toStdString() << std::endl;

//	QRegExp exp("^/user/([a-z]+)$");
//	if( exp.indexIn(req->path()) != -1 )
//	{
//		resp->setHeader("Content-Type", "text/html");
//		resp->writeHead(200);

//		QString name = exp.capturedTexts()[1];
//		QString body = tr("<html><head><title>Greeting App</title></head><body><h1>Hello %1!</h1></body></html>");
//		resp->end(body.arg(name).toUtf8());
//	}
//	else
//	{
//		resp->writeHead(403);
//		resp->end(QByteArray("You aren't allowed here!"));
//	}

//}


} // namespace cx
