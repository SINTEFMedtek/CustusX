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

bool Socket::connectToHost(QString ip, int port)
{
    int r = mSocket->ConnectToServer(ip.toStdString().c_str(), port);
    return (r ? true : false);
}

void Socket::skip(int bytes)
{
    mSocket->Skip(bytes, 0);
}

int Socket::receive(void *packPointer, int packSize)
{
    int r = mSocket->Receive(packPointer, packSize);
    return r;
}

bool Socket::connectionIsOk()
{
    if(mSocket.IsNull() || mSocket->GetConnected() == 0) //server have disconnected...
    {
        CX_LOG_CHANNEL_WARNING("janne beate ") << "Server disconnected...";
        return false;
    }
    return true;
}

void Socket::close()
{
    mSocket->CloseSocket();
}

}//namespace cx
