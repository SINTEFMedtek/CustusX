/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxWebServerWidget.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

#include <qhttpserver.h>
#include <qhttprequest.h>
#include <qhttpresponse.h>
#include "cxHttpRequestHandler.h"
#include "cxRemoteAPI.h"

#include "cxLogger.h"

namespace cx
{

WebServerWidget::WebServerWidget(VisServicesPtr services, QWidget* parent) :
	QWidget(parent),
	mVisServices(services),
	mVerticalLayout(new QVBoxLayout(this)),
	mStartStopButton(new QPushButton(QIcon(":/icons/open_icon_library/media-playback-start-3.png"), "Start"))
{
	this->setObjectName("WebServerWidget");
	this->setWindowTitle("Web Server");
	this->setWhatsThis(this->defaultWhatsThis());

	mStartStopButton->setCheckable(true);

	mVerticalLayout->addWidget(new QLabel("Run Web Server"));
	mVerticalLayout->addWidget(mStartStopButton);
	mVerticalLayout->addStretch();

	this->initServer();

	connect(mStartStopButton, &QPushButton::clicked, this, &WebServerWidget::startStopSlot);
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
	mAPI.reset(new RemoteAPI(mVisServices));
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

void WebServerWidget::startStopSlot(bool checked)
{
//	CX_LOG_DEBUG() << "WebServerWidget::startStopSlot: " << checked;
	if(checked)
	{
		this->startServer();
		mStartStopButton->setIcon(QIcon(":/icons/open_icon_library/media-playback-stop-3.png"));
		mStartStopButton->setText("Stop");
	}
	else
	{
		this->stopServer();
		mStartStopButton->setIcon(QIcon(":/icons/open_icon_library/media-playback-start-3.png"));
		mStartStopButton->setText("Start");
		this->shutdownServer();
	}
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
