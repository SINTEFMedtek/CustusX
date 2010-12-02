#ifndef CXOPENIGTLINKSESSION_H_
#define CXOPENIGTLINKSESSION_H_

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QTcpSocket>
#include "igtlImageMessage.h"
#include "cxGrabber.h"

namespace cx
{
/**
 * \class OpenIGTLinkSession
 *
 * \brief
 *
 * \date 26. nov. 2010
 * \author: Janne Beate Bakeng, SINTEF
 * \author: Christian Askeland, SINTEF
 */
class OpenIGTLinkSession : public QThread
{
  Q_OBJECT

public:
  OpenIGTLinkSession(int socketDescriptor, QObject* parent = NULL);
  virtual ~OpenIGTLinkSession();

signals:
  void frame(Frame& frame);

protected:
  virtual void run();

private:
  int mSocketDescriptor;
  QTcpSocket* mSocket;
};

/**
 * \class OpenIGTLinkSender
 *
 * \brief An object sending images out on an ip port.
 * In order to operate within a nongui thread,
 * it must be created within the run() method
 * of a qthread.
 *
 * \date 26. nov. 2010
 * \author: Janne Beate Bakeng, SINTEF
 * \author: Christian Askeland, SINTEF
 */
class OpenIGTLinkSender : public QObject
{
  Q_OBJECT

public:
  OpenIGTLinkSender(QTcpSocket* socket, QObject* parent = NULL);
  virtual ~OpenIGTLinkSender();

public slots:
  void receiveFrameSlot(Frame& frame);
  void sendOpenIGTLinkImageSlot();
  void errorSlot(QAbstractSocket::SocketError);

signals:
  void imageOnQueue(); //means that a new igtl::ImageMessage is in the message queue

private:
  igtl::ImageMessage::Pointer convertFrame(Frame& frame);
  void addImageToQueue(igtl::ImageMessage::Pointer imgMsg);
  igtl::ImageMessage::Pointer getLastImageMessageFromQueue();

  QTcpSocket* mSocket;

  QMutex mImageMutex;
  std::list<igtl::ImageMessage::Pointer> mMutexedImageMessageQueue;

};

}//namespace cx
#endif /* CXOPENIGTLINKSESSION_H_ */
