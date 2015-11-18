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

#ifndef CXSOCKET_H
#define CXSOCKET_H

#include "cxResourceExport.h"

#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QAbstractSocket>
#include <QString>
#include "cxSocketConnection.h"

QT_BEGIN_NAMESPACE
class QTcpSocket;
QT_END_NAMESPACE

namespace cx
{

/**
 * Reimplement QTcpServer::incomingConnection() to
 * do nothing but emit a signal.
 *
 * Used internally by SocketConnection.
 */
class SingleConnectionTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    SingleConnectionTcpServer(QObject* parent);
//	void setSocket(QPointer<Socket> socket);
signals:
    void incoming(qintptr socketDescriptor);
protected:
    void incomingConnection(qintptr socketDescriptor);
private:
//	QPointer<Socket> mSocket;
};


class cxResource_EXPORT SocketConnector : public QObject
{
    Q_OBJECT
public:
    virtual ~SocketConnector() {}
//	SocketConnector(SocketConnection::ConnectionInfo info, QSocket* socket);

    virtual void activate() = 0;
    virtual void deactivate() = 0;
    virtual CX_SOCKETCONNECTION_STATE getState() = 0;
    virtual SocketConnection::ConnectionInfo getInfo() const = 0;
signals:
    void stateChanged(CX_SOCKETCONNECTION_STATE);
};

class cxResource_EXPORT SocketClientConnector : public SocketConnector
{
public:
    SocketClientConnector(SocketConnection::ConnectionInfo info, QTcpSocket* socket);
    virtual ~SocketClientConnector();

    virtual void activate();
    virtual void deactivate();
    virtual CX_SOCKETCONNECTION_STATE getState();
    virtual SocketConnection::ConnectionInfo getInfo() const { return mInfo; }

private:
    void internalConnected();
    void internalDisconnected();

    SocketConnection::ConnectionInfo mInfo;
    QTcpSocket* mSocket;

};

class cxResource_EXPORT SocketServerConnector : public SocketConnector
{
    Q_OBJECT
public:
    SocketServerConnector(SocketConnection::ConnectionInfo info, QTcpSocket* socket);
    virtual ~SocketServerConnector();

    virtual void activate();
    virtual void deactivate();
    virtual CX_SOCKETCONNECTION_STATE getState();
    virtual SocketConnection::ConnectionInfo getInfo() const { return mInfo; }

private:
    bool startListen();
    void stopListen();
    void incomingConnection(qintptr socketDescriptor);
    QStringList getAllServerHostnames();

    SocketConnection::ConnectionInfo mInfo;
    QPointer<class SingleConnectionTcpServer> mServer;
    QTcpSocket* mSocket;
};

}

#endif //CXSOCKET_H
