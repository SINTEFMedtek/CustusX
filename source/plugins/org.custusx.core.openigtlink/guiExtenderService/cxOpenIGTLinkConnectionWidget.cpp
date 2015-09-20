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

#include "cxOpenIGTLinkConnectionWidget.h"

#include <QPushButton>
#include "cxOpenIGTLinkClient.h"
#include "cxHelperWidgets.h"
#include "cxProfile.h"
#include "cxLogger.h"
#include "boost/bind.hpp"

namespace cx {

OpenIGTLinkConnectionWidget::OpenIGTLinkConnectionWidget(OpenIGTLinkClient *client, QWidget *parent) :
    BaseWidget(parent, "OpenIGTLinkConnectionWidget", "OpenIGTLink Connection"),
    mClient(client)
{
	CX_LOG_CHANNEL_DEBUG("CA") << "OpenIGTLinkConnectionWidget create " << client->getUid();
	XmlOptionFile options = profile()->getXmlSettings().descend(client->getUid());
    mOptionsElement = options.getElement();
    StringPropertyBasePtr ip = this->getIpOption(mOptionsElement);
    DoublePropertyBasePtr port = this->getPortOption(mOptionsElement);
    StringPropertyBasePtr dialects = this->getDialectOption(mOptionsElement, mClient);
	StringPropertyBasePtr role = this->getRoleOption(mOptionsElement);

    mConnectButton = new QPushButton("Connect", this);
	mConnectButton->setCheckable(true);
    connect(mConnectButton, &QPushButton::clicked, this, &OpenIGTLinkConnectionWidget::connectButtonClicked);

	// these connections facilitate cross-thread communication
//    connect(this, &OpenIGTLinkConnectionWidget::requestConnect, mClient, &OpenIGTLinkClient::requestConnect);
//    connect(this, &OpenIGTLinkConnectionWidget::requestDisconnect, mClient, &OpenIGTLinkClient::requestDisconnect);
//	connect(this, &OpenIGTLinkConnectionWidget::setConnectionInfo, mClient, &OpenIGTLinkClient::setConnectionInfo);

//    connect(mClient, &OpenIGTLinkClient::connected, this, &OpenIGTLinkConnectionWidget::clientConnected);
//    connect(mClient, &OpenIGTLinkClient::disconnected, this, &OpenIGTLinkConnectionWidget::clientDisconnected);
//    connect(mClient, &OpenIGTLinkClient::error, this, &OpenIGTLinkConnectionWidget::clientDisconnected);
	connect(mClient, &OpenIGTLinkClient::stateChanged, this, &OpenIGTLinkConnectionWidget::onStateChanged);

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
	CX_LOG_CHANNEL_DEBUG("CA") << "OpenIGTLinkConnectionWidget end create " << client->getUid();
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
	CX_LOG_CHANNEL_DEBUG("CA") << "Changed connection state to " << string_cast(state);

	QString status = qstring_cast(state);
	QString action = (state==scsINACTIVE) ? "Connect" : "Disconnect";

	mConnectButton->blockSignals(true);
	mConnectButton->setEnabled(state != scsCONNECTING);
	mConnectButton->setChecked(state != scsINACTIVE);
	mConnectButton->setText(QString("%1 (%2)").arg(action).arg(status));
	mConnectButton->blockSignals(false);

	mOptionsWidget->setEnabled(state == scsINACTIVE);
}

//void OpenIGTLinkConnectionWidget::clientConnected()
//{
////    mConnectButton->blockSignals(true);
////    mConnectButton->setEnabled(true);
////    mConnectButton->setChecked(true);
////    mConnectButton->setText("Disconnect");
////    mConnectButton->blockSignals(false);

////	mOptionsWidget->setEnabled(false);
//}

//void OpenIGTLinkConnectionWidget::clientDisconnected()
//{
////    mConnectButton->blockSignals(true);
////    mConnectButton->setEnabled(true);
////    mConnectButton->setChecked(false);
////    mConnectButton->setText("Connect");
////    mConnectButton->blockSignals(false);

////	mOptionsWidget->setEnabled(true);
//}

void OpenIGTLinkConnectionWidget::connectButtonClicked(bool checked)
{
//    StringPropertyBasePtr ip = this->getIpOption(mOptionsElement);
//    DoublePropertyBasePtr port = this->getPortOption(mOptionsElement);
    StringPropertyBasePtr dialects = this->getDialectOption(mOptionsElement, mClient);
    mClient->setDialect(dialects->getValue());

	OpenIGTLinkClient::ConnectionInfo info;
	info.role = this->getRoleOption(mOptionsElement)->getValue();
	info.host = this->getIpOption(mOptionsElement)->getValue();
	info.port = this->getPortOption(mOptionsElement)->getValue();
	info.protocol = this->getDialectOption(mOptionsElement, mClient)->getValue();

    if(checked)
    {
//		CX_LOG_CHANNEL_DEBUG("CA") << "set port " << info.port;
		boost::function<void()> configure = boost::bind(&OpenIGTLinkClient::setConnectionInfo, mClient, info);
		mClient->invoke(configure);
//        emit ipAndPort(ip->getValue(), port->getValue());
		boost::function<void()> connect = boost::bind(&OpenIGTLinkClient::requestConnect, mClient);
		mClient->invoke(connect);
//        emit requestConnect();
//        mConnectButton->setText("Trying to connect...");
//        mConnectButton->setEnabled(false);
    }
    else
    {
		boost::function<void()> disconnect = boost::bind(&OpenIGTLinkClient::requestDisconnect, mClient);
		mClient->invoke(disconnect);
//        emit requestDisconnect();
//        mConnectButton->setText("Trying to disconnect...");
//        mConnectButton->setEnabled(false);
    }
}

StringPropertyBasePtr OpenIGTLinkConnectionWidget::getDialectOption(QDomElement root, OpenIGTLinkClient* client)
{
    StringPropertyPtr retval;
    QStringList dialectnames;
    if(client)
        dialectnames = client->getAvailableDialects();
	retval = StringProperty::initialize("protocol", "Dialect", "OpenIGTLinkDialect", dialectnames.front(), dialectnames, root);
    retval->setValueRange(dialectnames);
    retval->setGroup("Connection");
    return retval;
}

StringPropertyBasePtr OpenIGTLinkConnectionWidget::getIpOption(QDomElement root)
{
    StringPropertyPtr retval;
    QString defaultValue = "127.0.0.1";
	retval = StringProperty::initialize("address", "Address", "TCP/IP Address", defaultValue, root);
    retval->setGroup("Connection");
    return retval;
}


DoublePropertyBasePtr OpenIGTLinkConnectionWidget::getPortOption(QDomElement root)
{
    DoublePropertyPtr retval;
    double defaultValue = 18944;
	retval = DoubleProperty::initialize("port", "Port",
										"TCP/IP Port (default "+QString::number(defaultValue)+")",
										defaultValue,
										DoubleRange(1024, 49151, 1), 0, root);
    retval->setGuiRepresentation(DoublePropertyBase::grSPINBOX);
    retval->setAdvanced(true);
    retval->setGroup("Connection");
//	CX_LOG_CHANNEL_DEBUG("CA") << "port_from_init " << retval->getValue();
    return retval;
}

StringPropertyBasePtr OpenIGTLinkConnectionWidget::getRoleOption(QDomElement root)
{
	StringPropertyPtr retval;
	QStringList values = QStringList() << "client" << "server";
	QString value = values.front();
	retval = StringProperty::initialize("role", "Role",
										"Act as client or server in the TCP/IP connection",
										value, values, root);
	retval->setGroup("Connection");
	return retval;
}




} //namespace cx
