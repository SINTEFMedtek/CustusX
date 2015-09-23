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

#include "cxConnectionHandle.h"

#include "cxSender.h"
#include "cxTime.h"
#include <QThread>

#include "cxPlusDialect.h"
#include "cxCustusDialect.h"
#include "cxRASDialect.h"
#include "igtl_header.h"
#include "cxIGTLinkConversionImage.h"
#include "cxIGTLinkConversionPolyData.h"
#include "cxXmlOptionItem.h"
#include "cxProfile.h"
#include "cxStringProperty.h"
#include "cxDoubleProperty.h"
#include "cxOpenIGTLinkClient.h"

namespace cx
{


NetworkConnectionHandle::NetworkConnectionHandle(QString threadname)
{
	mThread.reset(new QThread());
	mThread->setObjectName(threadname);
	mClient.reset(new NetworkConnection(threadname));
	connect(mClient.get(), &NetworkConnection::connectionInfoChanged, this, &NetworkConnectionHandle::onConnectionInfoChanged);
	mClient->moveToThread(mThread.get());

	XmlOptionFile options = profile()->getXmlSettings().descend(mClient->getUid());
	mOptionsElement = options.getElement();

	mIp = this->createIpOption();
	mPort = this->createPortOption();
	mDialects = this->createDialectOption();
	mRole = this->createRoleOption();

	this->onPropertiesChanged();

	mThread->start();
}

void NetworkConnectionHandle::onPropertiesChanged()
{
	NetworkConnection::ConnectionInfo info;
	info.role = mRole->getValue();
	info.host = mIp->getValue();
	info.port = mPort->getValue();
	info.protocol = mDialects->getValue();

	mClient->setConnectionInfo(info);
}

void NetworkConnectionHandle::onConnectionInfoChanged()
{
	NetworkConnection::ConnectionInfo info = mClient->getConnectionInfo();
	mRole->setValue(info.role);
	mIp->setValue(info.host);
	mPort->setValue(info.port);
	mDialects->setValue(info.protocol);
}

NetworkConnectionHandle::~NetworkConnectionHandle()
{
	mThread->quit();
	mThread->wait();

	mClient.reset();
	// thread-delete implicitly at end.
}

NetworkConnection* NetworkConnectionHandle::client()
{
	return mClient.get();
}

StringPropertyBasePtr NetworkConnectionHandle::createDialectOption()
{
	StringPropertyPtr retval;
	QStringList dialectnames;
	if(mClient)
		dialectnames = mClient->getAvailableDialects();
	retval = StringProperty::initialize("protocol", "Connect to", "Protocol to use during connection.",
										mClient->getConnectionInfo().protocol,
										dialectnames, mOptionsElement);
	retval->setValueRange(dialectnames);
	retval->setGroup("Connection");
	connect(retval.get(), &Property::changed, this, &NetworkConnectionHandle::onPropertiesChanged);
	return retval;
}

StringPropertyBasePtr NetworkConnectionHandle::createIpOption()
{
	StringPropertyPtr retval;
	retval = StringProperty::initialize("address", "Address", "Network Address",
										mClient->getConnectionInfo().host,
										mOptionsElement);
	retval->setGroup("Connection");
	connect(retval.get(), &Property::changed, this, &NetworkConnectionHandle::onPropertiesChanged);
	return retval;
}


DoublePropertyBasePtr NetworkConnectionHandle::createPortOption()
{
	DoublePropertyPtr retval;
	int defval = mClient->getConnectionInfo().port;
	retval = DoubleProperty::initialize("port", "Port",
										"Network Port (default "+QString::number(defval)+")",
										defval,
										DoubleRange(1024, 49151, 1), 0,
										mOptionsElement);
	retval->setGuiRepresentation(DoublePropertyBase::grSPINBOX);
	retval->setAdvanced(true);
	retval->setGroup("Connection");
	connect(retval.get(), &Property::changed, this, &NetworkConnectionHandle::onPropertiesChanged);
	return retval;
}

StringPropertyBasePtr NetworkConnectionHandle::createRoleOption()
{
	StringPropertyPtr retval;
	QStringList values = QStringList() << "client" << "server";
	retval = StringProperty::initialize("role", "Role",
										"Act as client or server in the network connection",
										mClient->getConnectionInfo().role,
										values, mOptionsElement);
	retval->setGroup("Connection");
	connect(retval.get(), &Property::changed, this, &NetworkConnectionHandle::onPropertiesChanged);
	return retval;
}

}//namespace cx
