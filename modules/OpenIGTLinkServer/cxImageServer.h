/*
 * cxImageServer.h
 *
 *  \date Oct 30, 2010
 *      \author christiana
 */

#ifndef CXIMAGESERVER_H_
#define CXIMAGESERVER_H_

#include <QTcpServer>


namespace cx
{

class ImageServer : public QTcpServer
{
  Q_OBJECT

public:
  ImageServer(QObject* parent = NULL);
  virtual ~ImageServer();
  void startListen(int port);
protected:
  void incomingConnection(int socketDescriptor);
private slots:
private:
};

} // namespace cx

#endif /* CXIMAGESERVER_H_ */
