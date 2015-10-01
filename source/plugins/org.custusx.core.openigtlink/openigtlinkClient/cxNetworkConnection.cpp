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

#include "cxNetworkConnection.h"

#include "cxSender.h"
#include "cxTime.h"
#include <QThread>

#include "cxProtocol.h"
#include "cxOpenIGTLinkProtocol.h"
#include "cxPlusProtocol.h"
#include "cxCustusProtocol.h"
#include "cxRASProtocol.h"

#include "cxXmlOptionItem.h"
#include "cxProfile.h"
#include "cxStringProperty.h"
#include "cxDoubleProperty.h"

#include "cxUtilHelpers.h"

namespace cx
{

NetworkConnection::NetworkConnection(QString uid, QObject *parent) :
    SocketConnection(parent),
	mUid(uid)
{
    qRegisterMetaType<Transform3D>("Transform3D");
    qRegisterMetaType<ImagePtr>("ImagePtr");
	qRegisterMetaType<ImagePtr>("MeshPtr");
	qRegisterMetaType<ProbeDefinitionPtr>("ProbeDefinitionPtr");


	ConnectionInfo info = this->getConnectionInfo();

    //TODO move to the correct plugin init
    info.protocol = this->initProtocol(ProtocolPtr(new CustusProtocol()))->getName();
    this->initProtocol(ProtocolPtr(new PlusProtocol()));
    this->initProtocol(ProtocolPtr(new OpenIGTLinkProtocol()));
    this->initProtocol(ProtocolPtr(new RASProtocol()));

	SocketConnection::setConnectionInfo(info);
    this->setProtocol(info.protocol);
}

NetworkConnection::~NetworkConnection()
{
}

ProtocolPtr NetworkConnection::initProtocol(ProtocolPtr value)
{
	mAvailableDialects[value->getName()] = value;
	return value;
}

void NetworkConnection::setConnectionInfo(ConnectionInfo info)
{
	SocketConnection::setConnectionInfo(info);
    this->setProtocol(info.protocol);
}

void NetworkConnection::invoke(boost::function<void()> func)
{
	QMetaObject::invokeMethod(this,
							  "onInvoke",
							  Qt::QueuedConnection,
							  Q_ARG(boost::function<void()>, func));
}

void NetworkConnection::onInvoke(boost::function<void()> func)
{
	func();
}

QStringList NetworkConnection::getAvailableDialects() const
{
    QStringList retval;
    DialectMap::const_iterator it = mAvailableDialects.begin();
    for( ; it!=mAvailableDialects.end() ; ++it)
    {
        retval << it->first;
    }
    return retval;
}

void NetworkConnection::setProtocol(QString protocolname)
{
    QMutexLocker locker(&mMutex);
    if(mProtocol && (protocolname == mProtocol->getName()))
        return;

    ProtocolPtr protocol = mAvailableDialects[protocolname];
    if(!protocol)
    {
        CX_LOG_ERROR() << "\"" << protocolname << "\" is an unknown opentigtlink dialect.";
        return;
    }

    if(mProtocol)
    {
        disconnect(mProtocol.get(), &Protocol::image, this, &NetworkConnection::image);
        disconnect(mProtocol.get(), &Protocol::mesh, this, &NetworkConnection::mesh);
        disconnect(mProtocol.get(), &Protocol::transform, this, &NetworkConnection::transform);
        disconnect(mProtocol.get(), &Protocol::calibration, this, &NetworkConnection::calibration);
        //TODO
//        disconnect(mDialect.get(), &OpenIGTLinkProtocol::probedefinition, this, &NetworkConnection::probedefinition);
//        disconnect(mDialect.get(), &OpenIGTLinkProtocol::usstatusmessage, this, &NetworkConnection::usstatusmessage);
//        disconnect(mDialect.get(), &OpenIGTLinkProtocol::igtlimage, this, &NetworkConnection::igtlimage);
    }

    mProtocol = protocol;
    connect(protocol.get(), &Protocol::image, this, &NetworkConnection::image);
    connect(protocol.get(), &Protocol::mesh, this, &NetworkConnection::mesh);
    connect(protocol.get(), &Protocol::transform, this, &NetworkConnection::transform);
    connect(protocol.get(), &Protocol::calibration, this, &NetworkConnection::calibration);
    //TODO
//    connect(dialect.get(), &OpenIGTLinkProtocol::probedefinition, this, &NetworkConnection::probedefinition);
//    connect(dialect.get(), &OpenIGTLinkProtocol::usstatusmessage, this, &NetworkConnection::usstatusmessage);
//    connect(dialect.get(), &OpenIGTLinkProtocol::igtlimage, this, &NetworkConnection::igtlimage);

    CX_LOG_CHANNEL_SUCCESS(CX_OPENIGTLINK_CHANNEL_NAME) << "IGTL Dialect set to " << protocolname;

}

void NetworkConnection::sendMessage(ImagePtr image)
{
	QMutexLocker locker(&mMutex);
    char *pointer = NULL;
    int size = 0;

    mProtocol->encode(image, pointer, size);

    mSocket->write(pointer, size);
}

void NetworkConnection::sendMessage(MeshPtr mesh)
{
	QMutexLocker locker(&mMutex);
    char *pointer = NULL;
    int size = 0;

    mProtocol->encode(mesh, pointer, size);

    mSocket->write(pointer, size);
}

void NetworkConnection::internalDataAvailable()
{
    //CX_LOG_DEBUG() << "START receive";
    if(!this->socketIsConnected())
        return;

    bool done = false;
    if(mProtocol->readyToReceiveData())
    {
        //CX_LOG_DEBUG() << "A ready to receive data " << mProtocol->readyToReceiveData();
        PackPtr pack = mProtocol->getPack();
        if(this->socketReceive(pack->pointer, pack->size))
        {
            //CX_LOG_DEBUG() << "B";
            pack->notifyDataArrived();
            done = true;
        }
    }
    //CX_LOG_DEBUG() << "END receive";

}

}//namespace cx
