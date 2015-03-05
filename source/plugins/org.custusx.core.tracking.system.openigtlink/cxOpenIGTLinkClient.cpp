#include "cxOpenIGTLinkClient.h"

#include "cxLogger.h"

namespace cx
{

OpenIGTLinkClient::OpenIGTLinkClient(QObject *parent) :
    QObject(parent), mState(Idle)
{
}

void OpenIGTLinkClient::establishConnectionToServer()
{
    if(mState == Idle)
    {
        const char* hostname = "10.218.140.127";
        int port = 18944;

        mSocket = igtl::ClientSocket::New();
        int r = mSocket->ConnectToServer(hostname, port);

        if (r != 0)
        {
          CX_LOG_CHANNEL_DEBUG("janne beate ") << "Cannot connect to the server.";
        }
        mState = Connected;

    } else {
        CX_LOG_CHANNEL_DEBUG("janne beate ") << "Unhandled state transition...";
    }
    CX_LOG_CHANNEL_INFO("janne beate ") << "Connection established.";
}

void OpenIGTLinkClient::listen()
{
    mState = Listening;

    // Create a message buffer to receive header
    igtl::MessageHeader::Pointer headerMsg;
    headerMsg = igtl::MessageHeader::New();

    CX_LOG_CHANNEL_INFO("janne beate ") << "Listening.";
    while(mState == Listening)
    {
        if(!this->connectionIsOk())
            break;

        if(!this->receiveHeader(headerMsg))
            break;

        if(!this->receiveBody(headerMsg))
            break;
    }
}

bool OpenIGTLinkClient::connectionIsOk()
{
    if(mSocket.IsNull() || mSocket->GetConnected() == 0) //server have disconnected...
    {
        CX_LOG_CHANNEL_WARNING("janne beate ") << "Server disconnected... Listening -> Idle";
        mState = Idle;
        return false;
    }
    return true;
}

bool OpenIGTLinkClient::receiveHeader(igtl::MessageHeader::Pointer headerMsg)
{
    headerMsg->InitPack();

    int r = mSocket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
    if (r == 0)
    {
      mSocket->CloseSocket();
      return false;
    }
    if (r != headerMsg->GetPackSize())
    {
      return false;
    }
    headerMsg->Unpack();

    std::string deviceType = std::string(headerMsg->GetDeviceType());
    CX_LOG_CHANNEL_DEBUG("janne beate ") << "Received header of type: " << deviceType;

    mSocket->Skip(headerMsg->GetBodySizeToRead(), 0);

    emit packageArrived();
    return true;
}

bool OpenIGTLinkClient::receiveBody(igtl::MessageBase::Pointer headerMsg)
{
    return false;
}


}//namespace cx
