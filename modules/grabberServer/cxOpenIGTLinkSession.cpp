#include "cxOpenIGTLinkSession.h"

#include <QTcpSocket>
#include <QHostAddress>
#include "sscMessageManager.h"
#include "sscTypeConversions.h"

namespace cx
{
//------------------------------------------------------------------------------

OpenIGTLinkSession::OpenIGTLinkSession(int socketDesctiptor, QObject* parent) :
    mSocketDescriptor(socketDesctiptor)
{}

OpenIGTLinkSession::~OpenIGTLinkSession()
{}

void OpenIGTLinkSession::run()
{
  //QThread::currentThread().print();

  mSocket = new QTcpSocket();
  mSocket->setSocketDescriptor(mSocketDescriptor);
  connect(mSocket, SIGNAL(disconnected()), this, SLOT(quit()), Qt::DirectConnection); // quit thread when disconnected

  QString clientName = mSocket->localAddress().toString();
  ssc::messageManager()->sendInfo("Connected to "+qstring_cast(clientName)+". Session started.");

  OpenIGTLinkSender* sender = new OpenIGTLinkSender(mSocket);
  connect(this, SIGNAL(frame(Frame)), sender, SLOT(receiveFrameSlot(Frame)), Qt::DirectConnection);

  this->exec();

  ssc::messageManager()->sendInfo("Disconnected from "+qstring_cast(clientName)+". Session ended.");
  delete sender;
  delete mSocket;
}

//------------------------------------------------------------------------------

OpenIGTLinkSender::OpenIGTLinkSender(QTcpSocket* socket, QObject* parent) :
    QObject(parent),
    mSocket(socket)
{
  //connect(this, SIGNAL(frameUpdated()), this, SLOT(sendOpenIGTLinkImage()), Qt::DirectConnection);
}

OpenIGTLinkSender::~OpenIGTLinkSender()
{}

void OpenIGTLinkSender::receiveFrameSlot(Frame frame)
{
  //I now receive frames
  //ssc::messageManager()->sendInfo("Received frame: timestamp:"+qstring_cast(frame.mTimestamp));

  //TODO
  //convert
  //put on queue
  //emit signal

  //mMessage = this->convertFrame(Frame);
  //Queue->push_back()
  //emit frame(Frame);
}

igtl::ImageMessage::Pointer OpenIGTLinkSender::convertFrame(Frame frame)
{
  //TODO
}

void OpenIGTLinkSender::sendOpenIGTLinkImage(igtl::ImageMessage::Pointer message)
{
  message->Pack();
  mSocket->write(reinterpret_cast<const char*>(message->GetPackPointer()), message->GetPackSize());
}

//------------------------------------------------------------------------------

}//namespace cx
