/*
 * cxImageServer.h
 *
 *  Created on: Oct 30, 2010
 *      Author: christiana
 */

#ifndef CXIMAGESESSION_H_
#define CXIMAGESESSION_H_

#include "boost/shared_ptr.hpp"
#include <QTcpSocket>
#include <QThread>
class QTimer;

namespace cx
{

/**An object sending images out on an ip port.
 * In order to operate within a nongui thread,
 * it must be created within the run() method
 * of a qthread.
 */
class ImageSender : public QObject
{
  Q_OBJECT

public:
  ImageSender(QTcpSocket* socket, QString imageFileDir, QObject* parent = NULL);
  virtual ~ImageSender() {}
protected:
private:
  QTcpSocket* mSocket;
  QTimer* mTimer;
  int mCounter;
  QString mImageFileDir;
private slots:
  void tick();

};


class ImageSession : public QThread
{
  Q_OBJECT

public:
  ImageSession(int socketDescriptor, QString imageFileDir, QObject* parent = NULL);
  virtual ~ImageSession();
protected:
  void run();
private:
  QTcpSocket* mSocket;
  int mSocketDescriptor;
  QString mImageFileDir;
private slots:

};

} // namespace cx

#endif /* CXIMAGESESSION_H_ */
