/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
#include <QApplication>

namespace cx
{

HttpRequestHandler::HttpRequestHandler(RemoteAPIPtr api) : mApi(api)
{
}

void HttpRequestHandler::handle_request(QHttpRequest *req, QHttpResponse *resp)
{
	mRequests.push_front(RequestType(req,resp));
	req->storeBody();
	connect(req, SIGNAL(end()), this, SLOT(onRequestSuccessful()));
}

void HttpRequestHandler::onRequestSuccessful()
{
	for (QList<RequestType>::iterator iter = mRequests.begin(); iter!=mRequests.end(); )
	{
		if (iter->req->successful())
		{
			this->handle_complete_request(iter->req, iter->resp);
			iter = mRequests.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void HttpRequestHandler::handle_complete_request(QHttpRequest *req, QHttpResponse *resp)
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
	// curl -H 'Content-Type: application/json' -X PUT -d '{"width":600,"height":300,"layout":"LAYOUT_US_Acquisition"}' http://localhost:8085/layout/display
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
	this->reply_notfound(resp); // implement in subclass
}

void HttpRequestHandler::delete_stream(QHttpResponse *resp)
{
	this->reply_notfound(resp); // implement in subclass
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
				 "%1 REST API"
                 "<p>"
                 "Arguments are passed using JSON format."
                 "</p>"
                 ""
                 "<table border=\"1\">"
                 "<tr><th>method</th><th>resource</th><th>description</th><th>arguments</th></tr>"
                 ""
                 "<tr><td>GET</td><td>/</td><td>main page: short server info</td><td>html page</td></tr>"
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
				 "%2"
                 ""
                 "</table>"
                 "</body>"
                 "");
	body = body
			.arg(qApp->applicationDisplayName())
			.arg(this->getAdditionalMainPageDescription());

    QByteArray ba = body.toUtf8();

    resp->setHeader("Content-Type", "text/html");
    resp->setHeader("Content-Length", QString::number(ba.size()));
    resp->writeHead(200); // everything is OK
    resp->write(ba);
    resp->end();
}

void HttpRequestHandler::handle_screen(QHttpRequest *req, QHttpResponse *resp)
{
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
//    GET    /screen                                        : get screenshot

	// disabled: security problem
//    if (req->method()==QHttpRequest::HTTP_GET)
//    {
//        this->reply_screenshot(resp);
//    }
//    else
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


} // namespace cx
