/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxPlusConnectWidget.h"

#include <QHBoxLayout>
#include <QPushButton>
#include "cxLogger.h"
#include "cxOpenIGTLinkStreamerService.h"
#include "cxVisServices.h"
#include "cxVideoService.h"
#include "cxBoolProperty.h"
#include "cxProfile.h"

#define START_TEXT "Start PLUS server and connect tracking and streaming"
#define STOP_TEXT "Stop PLUS server and disconnect tracking and streaming"

namespace cx
{
PlusConnectWidget::PlusConnectWidget(VisServicesPtr services, QWidget* parent) :
  BaseWidget(parent, "plus_connect_widget" ,"Connect to PLUS"),
  mServices(services),
	mPlusRunning(false),
	mExternalProcess(new QProcess(this))
{

  QHBoxLayout* layout = new QHBoxLayout(this);

	mConnectButton = new QPushButton(START_TEXT);
	mConnectButton->setToolTip("Remove all saved clip planes from the selected volume");
	connect(mConnectButton, &QPushButton::clicked, this, &PlusConnectWidget::connectButtonClickedSlot);
	//clearButton->setEnabled(false);
	layout->addWidget(mConnectButton);

//	layout->addStretch();
}

void PlusConnectWidget::connectButtonClickedSlot()
{
	//TODO: Use OpenIGTLinkStreamerService
	if(mPlusRunning)
	{
		CX_LOG_INFO() << "Stopping PLUS server and disconnecting";
		if(this->stopPlus())
		{
			mConnectButton->setText(START_TEXT);
			mPlusRunning = false;
		}
		else
			CX_LOG_WARNING() << "Failed to stop/disconnect PLUS server";
	}
	else
	{
		CX_LOG_INFO() << "Starting PLUS server and connecting";
		if(this->startPlus())
		{
			mConnectButton->setText(STOP_TEXT);
			mPlusRunning = true;
		}
		else
			CX_LOG_WARNING() << "Failed to start/connect PLUS server";
	}
}
bool PlusConnectWidget::stopPlus()
{
	StreamerServicePtr streamerService = this->getStreamerService();
	if(!streamerService)
		return false;

	//TODO trigger stopTrackingAndOpenIGTLinkClientIfStartedFromThisObject
//	streamerService->stopTrackingAndOpenIGTLinkClientIfStartedFromThisObject();

//	streamerService->stop();

	return true;
}

bool PlusConnectWidget::startPlus()
{
	StreamerServicePtr streamerService = this->getStreamerService();
	if(!streamerService)
		return false;

	this->turnOnStartTrackingInOpenIGTLinkStreamer(streamerService);
	streamerService->createStreamer(this->getXmlVideoElement());

	return true;
}

QDomElement PlusConnectWidget::getXmlVideoElement()
{
	XmlOptionFile xmlFile = profile()->getXmlSettings().descend("video");
	QDomElement element = xmlFile.getElement("video");
	return element;
}

void PlusConnectWidget::turnOnStartTrackingInOpenIGTLinkStreamer(StreamerServicePtr streamerService)
{
	QDomElement element = this->getXmlVideoElement();

	std::vector<PropertyPtr> settings = streamerService->getSettings(element);
	for(unsigned i = 0; i < settings.size(); ++i)
	{
		if (settings[i]->getUid().contains("start_tracking"))
		{
			CX_LOG_DEBUG() << "Turning on track and stream in OpenIGTLinkStreamer";
			settings[i]->setValueFromVariant(true);
		}
	}
}

StreamerServicePtr PlusConnectWidget::getStreamerService()
{
	StreamerServicePtr streamer = mServices->video()->getStreamerService(OPENIGTLINK3_STREAMER);
	if(!streamer)
		CX_LOG_WARNING() << "PlusConnectWidget::getOpenIGTLinkStreamerService(): Cannot get StreamerServicePtr";

	return streamer;
}

}//namespace cx
