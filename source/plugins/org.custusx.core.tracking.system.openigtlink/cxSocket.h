#ifndef CXSOCKET_H
#define CXSOCKET_H

#include <boost/shared_ptr.hpp>
#include <QString>
#include "igtlClientSocket.h"

namespace cx {
typedef boost::shared_ptr<class Socket> SocketPtr;
/**
 * @brief The Socket class
 * @date 18.03.2015
 * @author Janne Beate Bakeng, SINTEF
 */
class Socket
{
public:
    static SocketPtr New();

    Socket();

    bool connectToHost(QString ip, int port);
    bool getConnected();
    void skip(int bytes);
    int receive(void *packPointer, int packSize);
    bool connectionIsOk();
    void close();

private:
    typedef igtl::ClientSocket SocketBase;
    typedef SocketBase::Pointer SocketBasePtr;
    SocketBasePtr mSocket;
};
}

#endif //CXSOCKET_H
