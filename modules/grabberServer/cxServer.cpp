#include "cxServer.h"

#include "sscMessageManager.h"
#include "sscTypeConversions.h"
#include "cxOpenIGTLinkSession.h"

namespace cx
{
//------------------------------------------------------------------------------
Server::Server(QObject* parent) :
    QTcpServer(parent),
    mPort(18333)
{
}

void Server::start()
{
  if(this->isListening())
    return;

  bool started = this->listen(QHostAddress::Any, mPort);

  if(started)
  {
    ssc::messageManager()->sendSuccess("Server is listening to port "+qstring_cast(this->serverPort()));
    emit open();
  }
  else
    ssc::messageManager()->sendError("Server failed to start: "+qstring_cast(this->errorString()));
}

void Server::stop()
{
  if(!this->isListening())
    return;

  this->close(); //TODO undefined behavior????
  ssc::messageManager()->sendSuccess("Server stopped listening and is closed.");
  emit closed();
}

bool Server::isOpen()
{
  return this->isListening();
}

void Server::setPort(int port)
{
  if(this->isOpen())
    this->stop();
  
  mPort = port;
  this->start();
}

int Server::getPort()
{
  return mPort;
}

//------------------------------------------------------------------------------

OpenIGTLinkServer::OpenIGTLinkServer(QObject* parent) :
    Server(parent)
{}

OpenIGTLinkServer::~OpenIGTLinkServer()
{}

void OpenIGTLinkServer::incomingConnection(int socketDescriptor)
{
  ssc::messageManager()->sendInfo("Incoming connection from socket descriptor "+qstring_cast(socketDescriptor));

  //create a thread
  OpenIGTLinkSession* session = new OpenIGTLinkSession(socketDescriptor);
  connect(session, SIGNAL(finished()), session, SLOT(deleteLater()));
  connect(this, SIGNAL(frame(Frame&)), session, SIGNAL(frame(Frame&)), Qt::DirectConnection);
  connect(session, SIGNAL(queueInfo(int, int)), this, SIGNAL(queueInfo(int, int)));
  session->start();
}

//------------------------------------------------------------------------------
}//namespace cx
