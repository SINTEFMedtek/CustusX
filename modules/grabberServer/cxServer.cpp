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
    ssc::messageManager()->sendSuccess("Server is listening to port "+qstring_cast(this->serverPort()));
  else
    ssc::messageManager()->sendError("Server failed to start: "+qstring_cast(this->errorString()));
}

void Server::stop()
{
  this->close();
  ssc::messageManager()->sendSuccess("Server stopped listening and is closed.");
}

void Server::setPort(int port)
{
  if(this->isListening())
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
  session->start();
}

//------------------------------------------------------------------------------
}//namespace cx
