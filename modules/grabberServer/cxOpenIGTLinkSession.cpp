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
  connect(this, SIGNAL(frame()), sender, SLOT(receiveFrameSlot()), Qt::DirectConnection);

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
  //connect(this, SIGNAL(newFrame()), this, SLOT(sendOpenIGTLinkImage()), Qt::DirectConnection);
}

OpenIGTLinkSender::~OpenIGTLinkSender()
{}

void OpenIGTLinkSender::receiveFrameSlot(/*Frame*/)
{
  //TODO
  mGrabber;//->getFrame();

  //mMessage = this->convertFrame(Frame);
  //emit newFrame();
}

igtl::ImageMessage::Pointer OpenIGTLinkSender::convertFrame(/*something*/)
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
