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

#include "cxNetworkConnectionWidget.h"

#include <QPushButton>
#include "cxNetworkConnection.h"
#include "cxHelperWidgets.h"
#include "cxProfile.h"
#include "cxLogger.h"
#include "boost/bind.hpp"
#include "cxNetworkConnectionHandle.h"


namespace cx {

OpenIGTLinkConnectionWidget::OpenIGTLinkConnectionWidget(NetworkConnectionHandlePtr client, QWidget *parent) :
    BaseWidget(parent, "OpenIGTLinkConnectionWidget", "OpenIGTLink Connection"),
    mClient(client)
{
	StringPropertyBasePtr ip = mClient->getIpOption();
	DoublePropertyBasePtr port = mClient->getPortOption();
	StringPropertyBasePtr dialects = mClient->getDialectOption();
	StringPropertyBasePtr role = mClient->getRoleOption();

    mConnectButton = new QPushButton("Connect", this);
	mConnectButton->setCheckable(true);
    connect(mConnectButton, &QPushButton::clicked, this, &OpenIGTLinkConnectionWidget::connectButtonClicked);

	connect(mClient->client(), &NetworkConnection::stateChanged, this, &OpenIGTLinkConnectionWidget::onStateChanged);

    QVBoxLayout* topLayout = new QVBoxLayout(this);

	mOptionsWidget = new QWidget(this);
	QVBoxLayout* optionsLayout = new QVBoxLayout(mOptionsWidget);
	optionsLayout->setMargin(0);
	topLayout->addWidget(mOptionsWidget);
	optionsLayout->addWidget(sscCreateDataWidget(this, role));
	QHBoxLayout* hostipLayout = new QHBoxLayout;
	optionsLayout->addLayout(hostipLayout);
	hostipLayout->addWidget(sscCreateDataWidget(this, ip));
	hostipLayout->addWidget(sscCreateDataWidget(this, port));
	optionsLayout->addWidget(sscCreateDataWidget(this, dialects));

    topLayout->addWidget(mConnectButton);
    topLayout->addStretch();
//	CX_LOG_CHANNEL_DEBUG("CA") << "OpenIGTLinkConnectionWidget end create " << client->getUid();

	this->onStateChanged(mClient->client()->getState());

}

OpenIGTLinkConnectionWidget::~OpenIGTLinkConnectionWidget()
{

}

QString OpenIGTLinkConnectionWidget::defaultWhatsThis() const
{
    return  "<html>"
            "<h3>Connect to an OpenIGTLink server</h3>"
            "<p>"
            "Specify the ip address and port of the server you want to connect to. "
            "</p>"
            "<p>"
            "</p>"
            "<p><i></i></p>"
            "</html>";
}

void OpenIGTLinkConnectionWidget::onStateChanged(CX_SOCKETCONNECTION_STATE state)
{
//	CX_LOG_CHANNEL_DEBUG("CA") << "Changed connection state to " << string_cast(state);

	QString status = qstring_cast(state);
	QString action = (state==scsINACTIVE) ? "Connect" : "Disconnect";

	mConnectButton->blockSignals(true);
	mConnectButton->setEnabled(state != scsCONNECTING);
	mConnectButton->setChecked(state != scsINACTIVE);
	mConnectButton->setText(QString("%1 (%2)").arg(action).arg(status));
	mConnectButton->blockSignals(false);

	mOptionsWidget->setEnabled(state == scsINACTIVE);
}

void OpenIGTLinkConnectionWidget::connectButtonClicked(bool checked)
{
    if(checked)
    {
		boost::function<void()> connect = boost::bind(&NetworkConnection::requestConnect, mClient->client());
		mClient->client()->invoke(connect);
    }
    else
    {
		boost::function<void()> disconnect = boost::bind(&NetworkConnection::requestDisconnect, mClient->client());
		mClient->client()->invoke(disconnect);
    }
}





} //namespace cx
