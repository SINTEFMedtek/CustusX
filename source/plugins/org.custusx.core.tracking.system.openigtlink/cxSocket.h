#ifndef CXSOCKET_H
#define CXSOCKET_H

#include <boost/shared_ptr.hpp>
#include <QString>
#include "igtlClientSocket.h"

namespace cx
{

typedef boost::shared_ptr<class Socket> SocketPtr;

/**
 * @brief The Socket class socket functionallity
 * @date 18.03.2015
 * @author Janne Beate Bakeng, SINTEF
 */
class Socket
{
public:
    static SocketPtr New();

    Socket();

    bool connectToHost(QString ip, int port) const;
    bool connectionIsOk() const;
    void closeConnection() const;

    int receive(void *packPointer, int packSize) const;
    void skip(int bytes) const;

private:
    typedef igtl::ClientSocket SocketBase;
    typedef SocketBase::Pointer SocketBasePtr;
    SocketBasePtr mSocket;
};
}

#endif //CXSOCKET_H
