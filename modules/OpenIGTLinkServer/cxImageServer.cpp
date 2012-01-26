/*
 * cxImageServer.cpp
 *
 *  \date Oct 30, 2010
 *      \author christiana
 */
#include "cxImageServer.h"
#include "cxImageSession.h"
#include <iostream>

namespace cx
{

ImageServer::ImageServer(QObject* parent) : QTcpServer(parent)
{
}

void ImageServer::startListen(int port)
{
  bool started = this->listen(QHostAddress::Any, port);

  if (started)
    std::cout << "Server is listening to port " << this->serverPort() << std::endl;
  else
    std::cout << "Server failed to start: " << this->errorString().toStdString() << std::endl;
}

ImageServer::~ImageServer()
{
}

void ImageServer::incomingConnection(int socketDescriptor)
{
  std::cout << "Server: Incoming connection..." << std::endl;

  ImageSession* session = new ImageSession(socketDescriptor);
  connect(session, SIGNAL(finished()), session, SLOT(deleteLater()));
  session->start();
}


}
