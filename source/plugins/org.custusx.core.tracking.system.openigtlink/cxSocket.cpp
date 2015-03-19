#include "cxSocket.h"
#include "cxLogger.h"

namespace cx{

SocketPtr Socket::New()
{
    return SocketPtr(new Socket);
}

Socket::Socket()
{
    mSocket = SocketBase::New();
}

bool Socket::connectToHost(QString ip, int port) const
{
    int r = mSocket->ConnectToServer(ip.toStdString().c_str(), port);
    return (r ? true : false);
}

bool Socket::connectionIsOk() const
{
    if(mSocket.IsNull() || mSocket->GetConnected() == 0) //server have disconnected...
    {
        CX_LOG_CHANNEL_WARNING("janne beate ") << "Server disconnected...";
        return false;
    }
    return true;
}

void Socket::closeConnection() const
{
    mSocket->CloseSocket();
}

int Socket::receive(void *packPointer, int packSize) const
{
    int r = mSocket->Receive(packPointer, packSize);
    return r;
}

void Socket::skip(int bytes) const
{
    mSocket->Skip(bytes, 0);
}
}//namespace cx
