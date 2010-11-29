#ifndef CXSERVER_H_
#define CXSERVER_H_

#include <QTcpServer>
//#include "igtlImageMessage.h"
#include "cxGrabber.h"

namespace cx
{
/**
 * \class Server
 *
 * \brief Server that is listening to default port 18333.
 *
 * \date 26. nov. 2010
 * \author: Janne Beate Bakeng, SINTEF
 */
class Server : public QTcpServer
{
  Q_OBJECT

public:
  Server(QObject* parent = NULL);
  virtual ~Server(){};

  virtual void start();
  virtual void stop();

  virtual void setPort(int port);
  int getPort();

signals:
  void frame(Frame frame);

protected:
  virtual void incomingConnection(int socketDescriptor) = 0;

private:
  int     mPort;
};

/**
 * \class OpenIGTLinkServer
 *
 * \brief Server that automatically starts listening to default port 18333 and
 * starts sending grabbed images (if available) using the IGTLink protocol.
 *
 * \date 26. nov. 2010
 * \author: Janne Beate Bakeng, SINTEF
 */
class OpenIGTLinkServer : public Server
{
  Q_OBJECT

public:
  OpenIGTLinkServer(QObject* parent = NULL);
  virtual ~OpenIGTLinkServer();

protected:
  virtual void incomingConnection(int socketDescriptor);

private:
};
}//namespace cx
#endif /* CXSERVER_H_ */
