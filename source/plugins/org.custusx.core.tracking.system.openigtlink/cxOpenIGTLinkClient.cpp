#include "cxOpenIGTLinkClient.h"

#include <QCoreApplication>
#include "igtlTransformMessage.h"
#include "cxLogger.h"

namespace cx
{

OpenIGTLinkClient::OpenIGTLinkClient(QObject *parent) :
    QObject(parent), mState(Idle)
{
    qRegisterMetaType<Transform3D>("Transform3D");
    mSocket = Socket::New();
}

void OpenIGTLinkClient::requestConnect(QString ip, int port)
{
    if(mState != Idle)
    {
        CX_LOG_CHANNEL_DEBUG("janne beate ") << "You need to be in state idle to connect";
        return;
    }

    CX_LOG_CHANNEL_DEBUG("janne beate ") << "trying to connect to " << ip << ":" << port;
    int r = mSocket->connectToHost(ip, port);
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

    if(this->socketIsConnected())
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
        QCoreApplication::processEvents();
        if(!this->socketIsConnected())
            break;

        if(!this->receiveHeader(headerMsg))
            break;

        if(!this->receiveBody(headerMsg))
            break;

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

bool OpenIGTLinkClient::socketIsConnected()
{
    if(!mSocket->connectionIsOk())
        mState = Idle;
    return true;
}

bool OpenIGTLinkClient::receiveHeader(const igtl::MessageHeader::Pointer headerMsg) const
{
    headerMsg->InitPack();

   if(!this->socketReceive(headerMsg->GetPackPointer(), headerMsg->GetPackSize()))
   {
       CX_LOG_ERROR() << "Could not receive header";
       return false;
   }
    headerMsg->Unpack();

    std::string deviceType = std::string(headerMsg->GetDeviceType());
    CX_LOG_CHANNEL_DEBUG("janne beate ") << "Received header of type: " << deviceType;

    return true;
}

bool OpenIGTLinkClient::receiveBody(const igtl::MessageBase::Pointer headerMsg)
{
    if(strcmp(headerMsg->GetDeviceType(), "TRANSFORM") == 0)
    {
        igtl::TransformMessage::Pointer body = igtl::TransformMessage::New();
        body->SetMessageHeader(headerMsg);
        body->AllocatePack();
        if(!this->socketReceive(body->GetPackBodyPointer(), body->GetPackBodySize()))
        {
            CX_LOG_ERROR() << "Could not receive body";
            return false;
        }

        int c = body->Unpack(1);
        if (c & igtl::MessageHeader::UNPACK_BODY)
        {
            // if CRC check is OK. Read transform data.
            igtl::Matrix4x4 matrix;
            body->GetMatrix(matrix);

            QString deviceName = body->GetDeviceName();
            Transform3D transform3D = Transform3D::fromFloatArray(matrix);
            igtl::TimeStamp::Pointer ts = igtl::TimeStamp::New();
            body->GetTimeStamp(ts);
            igtlUint64 timestamp = ts->GetTimeStampInNanoseconds(); //since epoc

            emit transform(deviceName, transform3D, timestamp);
        }
        else
        {
            CX_LOG_ERROR() << "Could  not unpack the body.";
        }
    }
    else
    {
        mSocket->skip(headerMsg->GetBodySizeToRead());
    }
    return true;
}

bool OpenIGTLinkClient::socketReceive(void *packPointer, int packSize) const
{
    int r = mSocket->receive(packPointer, packSize);
    if (r == 0)
    {
      mSocket->closeConnection();
      return false;
    }
    if (r != packSize)
    {
      return false;
    }
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
