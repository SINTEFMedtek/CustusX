/*
 * cxImageServer.h
 *
 *  Created on: Oct 30, 2010
 *      Author: christiana
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
  ImageServer(QString imageFileDir, QObject* parent = NULL);
  virtual ~ImageServer();
  void startListen(int port);
protected:
  void incomingConnection(int socketDescriptor);
private slots:
//  void preSessionDelete();
private:
  QString mImageFileDir;

};

} // namespace cx

#endif /* CXIMAGESERVER_H_ */
