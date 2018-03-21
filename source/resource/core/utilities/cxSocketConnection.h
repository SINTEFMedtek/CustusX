/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXSOCKETCONNECTION_H
#define CXSOCKETCONNECTION_H

#include <QPointer>
#include <QTcpServer>
#include <QMutex>
#include "cxEnumConverter.h"
#include "boost/shared_ptr.hpp"

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

SNW_DECLARE_ENUM_STRING_CONVERTERS(cxResource_EXPORT, cx, CX_SOCKETCONNECTION_STATE);

/** Insert into every method that is only allowed to be called inside
  * the object's own thread.
  */
#define assertRunningInObjectThread() \
{ \
    if (QThread::currentThread()!=this->thread()) \
    { \
        CX_LOG_ERROR() \
            << QString("Method should be called in the object's thread [%1] only, current thread = [%2]") \
            .arg(this->thread()->objectName()) \
            .arg(QThread::currentThread()->objectName()); \
    }		\
} \



namespace cx {

typedef boost::shared_ptr<class SocketConnector> SocketConnectorPtr;

class cxResource_EXPORT SocketConnection : public QObject
{
    Q_OBJECT
public:
    explicit SocketConnection(QObject *parent = 0);

    struct cxResource_EXPORT ConnectionInfo
    {
        QString role;
        QString protocol;
        QString host;
        int port;

        bool operator==(const ConnectionInfo& rhs) const;
        bool isServer() const;
        bool isClient() const;
        bool isLocalhostConnection() const;
        QString getDescription() const;
    };

    /**
     * Return the connection info for the *next* connection.
     * Any existing connection might have other values.
     */
    ConnectionInfo getConnectionInfo(); ///< thread-safe
    CX_SOCKETCONNECTION_STATE getState(); ///< thread-safe
    virtual void setConnectionInfo(ConnectionInfo info); ///< thread-safe

    virtual void requestConnect(); ///< not thread-safe: use invoke
    virtual void requestDisconnect(); ///< not thread-safe: use invoke

public slots:
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
    void internalError(QAbstractSocket::SocketError socketError);
    virtual void internalDataAvailable() = 0;

protected:
    virtual void setProtocol(QString protocolname) = 0;
    SocketConnectorPtr createConnector(ConnectionInfo info);
    bool socketIsConnected();
    bool enoughBytesAvailableOnSocket(int bytes) const;
    bool socketReceive(void *packPointer, int packSize) const;
    QStringList getAllServerHostnames();
    void setCurrentConnectionInfo();
    void stateChange(CX_SOCKETCONNECTION_STATE newState);
    CX_SOCKETCONNECTION_STATE computeState();

    QTcpSocket* mSocket;

    CX_SOCKETCONNECTION_STATE mCurrentState;
    QMutex mNextConnectionInfoMutex;
    ConnectionInfo mNextConnectionInfo; ///< info to be used for the next connect(), mutexed.

protected:
    SocketConnectorPtr mConnector;
};

} //namespace cx

#endif // CXSOCKETCONNECTION_H
