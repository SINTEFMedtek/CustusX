#include "cxOpenIGTLinkClient.h"

#include <QCoreApplication>
#include "cxLogger.h"

namespace cx
{

OpenIGTLinkClient::OpenIGTLinkClient(QObject *parent) :
    QObject(parent), mState(Idle)
{
    mSocket = igtl::ClientSocket::New();
}

void OpenIGTLinkClient::requestConnect(QString ip, int port)
{
    if(mState != Idle)
    {
        CX_LOG_CHANNEL_DEBUG("janne beate ") << "You need to be in state idle to connect";
        return;
    }

    int r = mSocket->ConnectToServer(ip.toStdString().c_str(), port);
    if (r != 0)
    {
      CX_LOG_CHANNEL_DEBUG("janne beate ") << "Cannot connect to the server.";
    }
    this->internalConnected();
}

void OpenIGTLinkClient::requestDisconnect()
{
    if(mState != Connected)
    {
        CX_LOG_CHANNEL_DEBUG("janne beate ") << "You need to be in state connected to disconnect";
        return;
    }

    if(this->connectionIsOk())
    {
        //Is this the right way to disconnect???
        mSocket = NULL;
    }
    this->internalDisconnected();
}

void OpenIGTLinkClient::requestStartProcessingMessages()
{
    if(mState != Connected)
    {
        CX_LOG_CHANNEL_DEBUG("janne beate ") << "You need to be in state connected to start processing";
        return;
    }

    this->internalStartedProcessingMessages();

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

        //TODO
        //emit packages
        QCoreApplication::processEvents();
    }
    this->internalStoppedProcessingMessages();
}

void OpenIGTLinkClient::requestStopProcessingMessages()
{
    if(mState != Listening)
    {
        CX_LOG_CHANNEL_DEBUG("janne beate ") << "You need to be in state listening to stop processing";
        return;
    }
    this->internalStoppedProcessingMessages();
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

    return true;
}

bool OpenIGTLinkClient::receiveBody(igtl::MessageBase::Pointer headerMsg)
{
    //TODO implement
    return true;
}

void OpenIGTLinkClient::internalConnected()
{
    mState = Connected;
    emit connected();
}

void OpenIGTLinkClient::internalDisconnected()
{
    mState = Idle;
    emit disconnected();
}

void OpenIGTLinkClient::internalStartedProcessingMessages()
{
    mState = Listening;
    emit startedProcessingMessages();
}

void OpenIGTLinkClient::internalStoppedProcessingMessages()
{
    mState = Connected;
    emit stoppedProcessingMessages();
}


}//namespace cx
