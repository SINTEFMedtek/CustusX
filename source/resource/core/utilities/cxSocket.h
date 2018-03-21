/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
