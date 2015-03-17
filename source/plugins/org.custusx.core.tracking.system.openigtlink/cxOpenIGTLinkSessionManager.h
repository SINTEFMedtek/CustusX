#ifndef CXOPENIGTLINKSESSIONMANAGER_H
#define CXOPENIGTLINKSESSIONMANAGER_H

#include <QObject>

#include "igtlSessionManager.h"
#include "igtlMessageHandler.h"

namespace cx {


class OpenIGTLinkSessionManager : public QObject
{
    Q_OBJECT
public:
    enum State{
        Idle, Connected, Listening
    };

    explicit OpenIGTLinkSessionManager(QObject *parent = 0);

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
    void addMessageHandlers();
    void addMessageHandler(igtl::MessageHandler* handler);
    void listen();

    void internalConnected();
    void internalDisconnected();
    void internalStartedProcessingMessages();
    void internalStoppedProcessingMessages();

    State mState;
    igtl::SessionManager::Pointer mSessionManager;

};

}//namespace cx
#endif // CXOPENIGTLINKSESSIONMANAGER_H
