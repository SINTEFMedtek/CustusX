#ifndef CXSERVER_H_
#define CXSERVER_H_

#include <QTcpServer>
#include <boost/shared_ptr.hpp>
#include "cxGrabber.h"

namespace cx
{
/**
 * \class Server
 *
 * \brief Abstract interface to a server that is listening to default port 18333.
 *
 * \date 26. nov. 2010
 * \\author Janne Beate Bakeng, SINTEF
 */
class Server : public QTcpServer
{
  Q_OBJECT

public:
  Server(QObject* parent = NULL);
  virtual ~Server(){};

  virtual void start(); ///< Start listening to the port.
  virtual void stop(); ///< Stop listening.
  virtual bool isOpen(); ///< Is listening or not.

  virtual void setPort(int port); ///< Changes the port the server listens to. Will automatically try to start listening to this port.
  int getPort(); ///< The port the server is currently listening to.

signals:
  void frame(Frame& frame); ///< Emitted whenever a new frame arrived.
  void queueInfo(int size, int dropped); ///< Emitted whenever the queue size changes
  void open(); ///< Emitted when the server is listening to a port.
  void closed(); ///< Emitted when the server no longer is listening.

protected:
  virtual void incomingConnection(int socketDescriptor) = 0; ///< Connects the server to a specific session. Must be implemented by subclasses.

private:
  int     mPort; ///< The port the server listens to.
};

/**
 * \class OpenIGTLinkServer
 *
 * \brief Server that automatically starts listening to default port 18333 and
 * starts sending grabbed images (if available) using the IGTLink protocol.
 *
 * \date 26. nov. 2010
 * \\author Janne Beate Bakeng, SINTEF
 */
class OpenIGTLinkServer : public Server
{
  Q_OBJECT

public:
  OpenIGTLinkServer(QObject* parent = NULL);
  virtual ~OpenIGTLinkServer();

protected:
  virtual void incomingConnection(int socketDescriptor); ///< Connects to a OpenIGTLinkSession.

private:
};

typedef boost::shared_ptr<class Server> ServerPtr;
typedef boost::shared_ptr<class OpenIGTLinkServer> OpenIGTLinkServerPtr;
}//namespace cx
#endif /* CXSERVER_H_ */
