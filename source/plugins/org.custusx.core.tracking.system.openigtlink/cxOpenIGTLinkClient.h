#ifndef CXOPENIGTLINKCLIENT_H
#define CXOPENIGTLINKCLIENT_H

#include <QObject>
#include "igtlClientSocket.h"
#include "igtlMessageHeader.h"

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

private:
    bool connectionIsOk();
    bool receiveHeader(igtl::MessageHeader::Pointer headerMsg);
    bool receiveBody(igtl::MessageHeader::Pointer headerMsg);

    void internalConnected();
    void internalDisconnected();
    void internalStartedProcessingMessages();
    void internalStoppedProcessingMessages();

    State mState;
    igtl::ClientSocket::Pointer mSocket;
};

} //namespace cx

#endif // CXOPENIGTLINKCLIENT_H
