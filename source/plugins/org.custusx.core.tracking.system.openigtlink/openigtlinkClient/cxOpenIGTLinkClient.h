#ifndef CXOPENIGTLINKCLIENT_H
#define CXOPENIGTLINKCLIENT_H

#include "org_custusx_core_tracking_system_openigtlink_Export.h"

#include <QObject>
#include "igtlMessageHeader.h"
#include "cxSocket.h"
#include "cxTransform3D.h"

namespace cx {

class org_custusx_core_tracking_system_openigtlink_EXPORT OpenIGTLinkClient : public QObject
{
    Q_OBJECT
public:

    explicit OpenIGTLinkClient(QObject *parent = 0);

    void setIpAndPort(QString ip, int port=18944); //not threadsafe

public slots:
    //TODO all request slost must be smarter
    // what should happen if someone requests that the client connects when it is already connected?
    // they might be waiting for a signal

    void requestConnect();
    void requestDisconnect();

signals:
    void connected();
    void disconnected();

    void transform(QString devicename, Transform3D transform, double timestamp);

private slots:
    void internalConnected();
    void internalDisconnected();
    void internalDataAvailable();

private:
    bool socketIsConnected();
    bool enoughBytesAvailableOnSocket(int bytes) const;
    bool receiveHeader(const igtl::MessageHeader::Pointer headerMsg) const;
    bool receiveBody(const igtl::MessageHeader::Pointer headerMsg);
    bool socketReceive(void *packPointer, int packSize) const;

    SocketPtr mSocket;
    igtl::MessageHeader::Pointer mHeader;
    bool mHeaderReceived;
    QString mIp;
    int mPort;
};

} //namespace cx

#endif // CXOPENIGTLINKCLIENT_H
