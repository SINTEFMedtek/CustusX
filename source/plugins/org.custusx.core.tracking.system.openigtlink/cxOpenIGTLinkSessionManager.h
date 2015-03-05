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
    void connectAndListen();

private:
    void addMessageHandlers();
    void addMessageHandler(igtl::MessageHandler* handler);
    void listen();

    State mState;
    igtl::SessionManager::Pointer mSessionManager;

};

}//namespace cx
#endif // CXOPENIGTLINKSESSIONMANAGER_H
