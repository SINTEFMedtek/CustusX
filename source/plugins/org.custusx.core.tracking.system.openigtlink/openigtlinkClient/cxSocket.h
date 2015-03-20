#ifndef CXSOCKET_H
#define CXSOCKET_H

#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QAbstractSocket>
#include <QString>

QT_BEGIN_NAMESPACE
class QTcpSocket;
//Q_DECLARE_METATYPE(QAbstractSocket::SocketError)
QT_END_NAMESPACE

namespace cx
{

typedef boost::shared_ptr<class Socket> SocketPtr;

/**
 * @brief The Socket class socket functionallity
 * @date 18.03.2015
 * @author Janne Beate Bakeng, SINTEF
 */
class Socket : public QObject
{
    Q_OBJECT
public:
    Socket(QObject *parent);

    void requestConnectToHost(QString ip, int port) const;
    bool isConnected() const;
    QString getLastError() const;
    void requestCloseConnection() const;

    bool minBytesAvailable(int bytes) const;
    qint64 read(char *data, qint64 maxSizeBytes) const;
    qint64 skip(qint64 maxSizeBytes) const;

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
    void reveidReadyRead();

private:
    typedef boost::shared_ptr<QTcpSocket> QTcpSocketPtr;
    QTcpSocketPtr mSocket;
    bool mConnected;
};
}

#endif //CXSOCKET_H
