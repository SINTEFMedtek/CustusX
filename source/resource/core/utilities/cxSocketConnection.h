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

#ifndef CXSOCKETCONNECTION_H
#define CXSOCKETCONNECTION_H

#include "cxSocket.h"
#include <QPointer>
class QTcpServer;
#include "cxEnumConverter.h"

#include <QTcpServer>

namespace cx {
enum CX_SOCKETCONNECTION_STATE
{
	scsINACTIVE,
	scsCONNECTED,
	scsLISTENING,
	scsCONNECTING,
	scsCOUNT
};
} // namespace cx

SNW_DECLARE_ENUM_STRING_CONVERTERS(cx, CX_SOCKETCONNECTION_STATE);



namespace cx {


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
	void setSocket(QPointer<Socket> socket);
signals:
	void incoming(qintptr socketDescriptor);
protected:
	void incomingConnection(qintptr socketDescriptor);
private:
	QPointer<Socket> mSocket;
};

class cxResource_EXPORT SocketConnection : public QObject
{
    Q_OBJECT
public:
    explicit SocketConnection(QObject *parent = 0);

	struct ConnectionInfo
	{
		QString role;
		QString protocol;
		QString host;
		int port;

		bool operator==(const ConnectionInfo& rhs) const
		{
			return (this->role == rhs.role)
					&& (this->protocol == rhs.protocol)
					&& (this->host == rhs.host)
					&& (this->port == rhs.port);
		}

		bool isServer() const { return role.toLower()=="server"; }
		bool isClient() const { return !this->isServer(); }
		QString getDescription() const
		{
			QString postfix;
			QString name = host;
			if (isServer())
					name = "listen";
			if (!protocol.isEmpty())
				postfix = QString("[%1]").arg(protocol);
			return QString("%1:%2%3").arg(name).arg(port).arg(postfix);
		}
	};

	ConnectionInfo getConnectionInfo();
	CX_SOCKETCONNECTION_STATE getState();

public slots:
	virtual void setConnectionInfo(ConnectionInfo info); ///< thread-safe
	void requestConnect(); ///< not thread-safe: use invoke
//    void tryConnectAndWait();
	void requestDisconnect(); ///< not thread-safe: use invoke
	bool sendData(const char* data, qint64 maxSize); ///< not thread-safe

signals:
	void connectionInfoChanged();
	void stateChanged(CX_SOCKETCONNECTION_STATE status);
    void connected();
    void disconnected();
    void error();

private slots:
    void internalConnected();
    void internalDisconnected();
	void internalError();
	virtual void internalDataAvailable();

protected:
    bool socketIsConnected();
    bool enoughBytesAvailableOnSocket(int bytes) const;
    bool socketReceive(void *packPointer, int packSize) const;
	QStringList getAllServerHostnames();
	bool startListen();
	void stopListen();
	void incomingConnection(qintptr socketDescriptor);
	void setCurrentConnectionInfo();
	void stateChange(CX_SOCKETCONNECTION_STATE newState);

	Socket* mSocket;
	QPointer<SingleConnectionTcpServer> mServer;

	CX_SOCKETCONNECTION_STATE mCurrentState;
	ConnectionInfo mCurrentConnectionInfo; ///< info for the currently active connection
	QMutex mNextConnectionInfoMutex;
	ConnectionInfo mNextConnectionInfo; ///< info to be used for the next connect(), mutexed.
};

} //namespace cx

#endif // CXSOCKETCONNECTION_H
