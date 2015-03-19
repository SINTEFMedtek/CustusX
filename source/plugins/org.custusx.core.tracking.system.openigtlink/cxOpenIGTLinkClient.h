#ifndef CXOPENIGTLINKCLIENT_H
#define CXOPENIGTLINKCLIENT_H

#include <QObject>
#include "igtlMessageHeader.h"
#include "cxSocket.h"
#include "cxTransform3D.h"

namespace cx {

class OpenIGTLinkClient : public QObject
{
    Q_OBJECT
public:
    enum State{
        Idle, Connected, Listening
    };

    explicit OpenIGTLinkClient(QObject *parent = 0);

public slots:
    void requestConnect(QString ip, int port);
    void requestDisconnect();
    void requestStartProcessingMessages();
    void requestStopProcessingMessages();

signals:
    void connected();
    void disconnected();
    void startedProcessingMessages();
    void stoppedProcessingMessages();

    void transform(QString devicename, Transform3D transform, double timestamp);

private:
    bool socketIsConnected();
    bool receiveHeader(const igtl::MessageHeader::Pointer headerMsg) const;
    bool receiveBody(const igtl::MessageHeader::Pointer headerMsg);
    bool socketReceive(void *packPointer, int packSize) const;

    void internalConnected();
    void internalDisconnected();
    void internalStartedProcessingMessages();
    void internalStoppedProcessingMessages();

    State mState;
    SocketPtr mSocket;
};

} //namespace cx

#endif // CXOPENIGTLINKCLIENT_H
