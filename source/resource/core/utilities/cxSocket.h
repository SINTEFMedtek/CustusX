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

QT_BEGIN_NAMESPACE
class QTcpSocket;
QT_END_NAMESPACE

namespace cx
{

typedef boost::shared_ptr<class Socket> SocketPtr;

/**
 * @brief The Socket class socket functionallity
 * @date 18.03.2015
 * @author Janne Beate Bakeng, SINTEF
 */
class cxResource_EXPORT Socket : public QObject
{
    Q_OBJECT
public:
    Socket(QObject *parent);

    void requestConnectToHost(QString ip, int port) const;
    bool tryConnectToHostAndWait(QString ip, int port) const;
    bool isConnected() const;
    QString getLastError() const;
    void requestCloseConnection() const;

    bool minBytesAvailable(int bytes) const;
    qint64 bytesAvailable() const;
    qint64 read(char *data, qint64 maxSizeBytes) const;
    qint64 skip(qint64 maxSizeBytes) const;

    qint64 write(const char* data, qint64 maxSizeBytes) const;

    bool waitForBytesWritten(int msecs = 3000);
    bool waitForReadyRead(int msecs = 3000);

signals:
    void connected();
    void disconnected();
    void readyRead();
    void error();

private slots:
    void receivedConnected();
    void receivedDisconnected();
    void receivedError(QAbstractSocket::SocketError socketError);
    void receivedHostFound();
    void receivedStateChanged(QAbstractSocket::SocketState socketState);
    void receiveReadyRead();
    void receiveBytesWritten(qint64 bytes);
    void receiveAboutToClose();

private:
    typedef boost::shared_ptr<QTcpSocket> QTcpSocketPtr;
    QTcpSocketPtr mSocket;
    bool mConnected;
};
}

#endif //CXSOCKET_H
