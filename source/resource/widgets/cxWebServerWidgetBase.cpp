/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxWebServerWidgetBase.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

namespace cx
{


WebServerWidgetBase::WebServerWidgetBase(cx::VisServicesPtr services, QWidget *parent) :
	QWidget(parent),
	mServices(services),
	mStartStopButton(new QPushButton(QIcon(":/icons/open_icon_library/media-playback-start-3.png"), "Start")),
	mVerticalLayout(new QVBoxLayout(this))
{
	mStartStopButton->setCheckable(true);

	mVerticalLayout->addWidget(new QLabel("Run Web Server"));
	mVerticalLayout->addWidget(mStartStopButton);
	//mVerticalLayout->addStretch();

	connect(mStartStopButton, &QPushButton::clicked, this, &WebServerWidgetBase::startStopSlot);
}

WebServerWidgetBase::~WebServerWidgetBase()
{
}

void WebServerWidgetBase::startStopSlot(bool checked)
{
//	CX_LOG_DEBUG() << "WebServerWidgetBase::startStopSlot: " << checked;
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

}//cx
