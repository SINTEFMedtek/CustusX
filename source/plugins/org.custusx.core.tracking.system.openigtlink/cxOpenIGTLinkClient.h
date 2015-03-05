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
    void establishConnectionToServer();
    void listen();

signals:
    void packageArrived();

private:
    bool connectionIsOk();
    bool receiveHeader(igtl::MessageHeader::Pointer headerMsg);
    bool receiveBody(igtl::MessageHeader::Pointer headerMsg);

    State mState;
    igtl::ClientSocket::Pointer mSocket;
};

} //namespace cx

#endif // CXOPENIGTLINKCLIENT_H
