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

#include "cxOpenIGTLinkWidget.h"

#include <QPushButton>
#include "cxOpenIGTLinkClient.h"
#include "cxHelperWidgets.h"
#include "cxProfile.h"
#include "cxLogger.h"

namespace cx {

OpenIGTLinkWidget::OpenIGTLinkWidget(OpenIGTLinkClient *client, QWidget *parent) :
    BaseWidget(parent, "OpenIGTLinkWidget", "OpenIGTLink Connection")
{
    XmlOptionFile options = profile()->getXmlSettings().descend("OpenIGTLinkWidget");
    mOptionsElement = options.getElement();
    StringPropertyBasePtr ip = this->getIpOption(mOptionsElement);
    DoublePropertyBasePtr port = this->getPortOption(mOptionsElement);

    mConnectButton = new QPushButton("Connect", this);
    mConnectButton->setCheckable(true);
    connect(mConnectButton, &QPushButton::clicked, this, &OpenIGTLinkWidget::connectButtonClicked);
    connect(this, &OpenIGTLinkWidget::requestConnect, client, &OpenIGTLinkClient::requestConnect);
    connect(this, &OpenIGTLinkWidget::requestDisconnect, client, &OpenIGTLinkClient::requestDisconnect);
    connect(this, &OpenIGTLinkWidget::ipAndPort, client, &OpenIGTLinkClient::setIpAndPort);
    connect(client, &OpenIGTLinkClient::connected, this, &OpenIGTLinkWidget::clientConnected);
    connect(client, &OpenIGTLinkClient::disconnected, this, &OpenIGTLinkWidget::clientDisconnected);
    connect(client, &OpenIGTLinkClient::error, this, &OpenIGTLinkWidget::clientDisconnected);

    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(sscCreateDataWidget(this, ip));
    topLayout->addWidget(sscCreateDataWidget(this, port));
    topLayout->addWidget(mConnectButton);
    topLayout->addStretch();
}

OpenIGTLinkWidget::~OpenIGTLinkWidget()
{

}

QString OpenIGTLinkWidget::defaultWhatsThis() const
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

void OpenIGTLinkWidget::clientConnected()
{
    mConnectButton->blockSignals(true);
    mConnectButton->setEnabled(true);
    mConnectButton->setChecked(true);
    mConnectButton->setText("Disconnect");
    mConnectButton->blockSignals(false);
}

void OpenIGTLinkWidget::clientDisconnected()
{
    mConnectButton->blockSignals(true);
    mConnectButton->setEnabled(true);
    mConnectButton->setChecked(false);
    mConnectButton->setText("Connect");
    mConnectButton->blockSignals(false);
}

void OpenIGTLinkWidget::connectButtonClicked(bool checked)
{
    StringPropertyBasePtr ip = this->getIpOption(mOptionsElement);
    DoublePropertyBasePtr port = this->getPortOption(mOptionsElement);

    if(checked)
    {
        CX_LOG_CHANNEL_DEBUG("janne beate ") << "Widget requesting to connect to " << ip->getValue() << ":" << port->getValue();
        emit ipAndPort(ip->getValue(), port->getValue());
        emit requestConnect();
        mConnectButton->setText("Trying to connect...");
        mConnectButton->setEnabled(false);
    }
    else
    {
        emit requestDisconnect();
        mConnectButton->setText("Trying to disconnect...");
        mConnectButton->setEnabled(false);
    }
}

StringPropertyBasePtr OpenIGTLinkWidget::getIpOption(QDomElement root)
{
    StringPropertyPtr retval;
    QString defaultValue = "127.0.0.1";
    retval = StringProperty::initialize("openigtlink_ip", "Address", "TCP/IP Address", defaultValue, root);
    retval->setGroup("Connection");
    return retval;
}


DoublePropertyBasePtr OpenIGTLinkWidget::getPortOption(QDomElement root)
{
    DoublePropertyPtr retval;
    retval = DoubleProperty::initialize("openigtlink_port", "Port", "TCP/IP Port (default 18333)", 18333, DoubleRange(1024, 49151, 1), 0, root);
    retval->setGuiRepresentation(DoublePropertyBase::grSPINBOX);
    retval->setAdvanced(true);
    retval->setGroup("Connection");
    return retval;
}


} //namespace cx
