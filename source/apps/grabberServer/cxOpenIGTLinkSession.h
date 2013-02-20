#ifndef CXOPENIGTLINKSESSION_H_
#define CXOPENIGTLINKSESSION_H_

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QTcpSocket>
//#include "igtlImageMessage.h"
#include "cxIGTLinkImageMessage.h"
#include "cxGrabber.h"

namespace cx
{
/**
 * \class OpenIGTLinkSession
 *
 * \brief A thread for running the server.
 *
 * \date 26. nov. 2010
 * \\author Janne Beate Bakeng, SINTEF
 * \\author Christian Askeland, SINTEF
 */
class OpenIGTLinkSession : public QThread
{
  Q_OBJECT

public:
  OpenIGTLinkSession(int socketDescriptor, QObject* parent = NULL);
  virtual ~OpenIGTLinkSession();

signals:
  void frame(Frame& frame); ///< Emitted whenever the session receives a new frame
  void queueInfo(int size, int dropped); ///< Emitted whenever the queue size changes

protected:
  virtual void run(); ///< Creates and connects to a socket and the OpenIGTLinkSender that sends frames to the socket.

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
 * \\author Janne Beate Bakeng, SINTEF
 * \\author Christian Askeland, SINTEF
 */
class OpenIGTLinkSender : public QObject
{
  Q_OBJECT

public:
  OpenIGTLinkSender(QTcpSocket* socket, QObject* parent = NULL);
  virtual ~OpenIGTLinkSender();

public slots:
  void receiveFrameSlot(Frame& frame); ///< The slot that receives the incoming frame. It is converted and added to internal threadsafe queue.
  void sendOpenIGTLinkImageSlot(int sendNumberOfImages); ///< Gets the oldest frame from the internal queue and sends it to the socket.
  void errorSlot(QAbstractSocket::SocketError); ///< Slot for receiving error messages from the socket.

signals:
  void imageOnQueue(int); ///< Emitted when there is a new igtl::ImageMessage is in the message queue
  void queueInfo(int size, int dropped); ///< Emitted whenever the queue size changes

private:
  //igtl::ImageMessage::Pointer convertFrame(Frame& frame); ///< Converst the frame into a OpenIGTLink ImageMessage
  IGTLinkImageMessage::Pointer convertFrame(Frame& frame); ///< Converst the frame into a OpenIGTLink ImageMessage
  //void addImageToQueue(igtl::ImageMessage::Pointer imgMsg); ///< Adds a OpenIGTLink ImageMessage to the queue
  void addImageToQueue(IGTLinkImageMessage::Pointer imgMsg); ///< Adds a OpenIGTLink ImageMessage to the queue
  //igtl::ImageMessage::Pointer getLastImageMessageFromQueue(); ///< Gets the oldes message from the queue-
  IGTLinkImageMessage::Pointer getLastImageMessageFromQueue(); ///< Gets the oldes message from the queue-

  QTcpSocket* mSocket; ///< The socket to send messages to.

  QMutex mImageMutex; ///< A lock for making the class threadsafe
  unsigned int mMaxqueueInfo;
  int mMaxBufferSize;
  std::list<IGTLinkImageMessage::Pointer> mMutexedImageMessageQueue; ///< A threasafe internal queue

  int mDroppedImages;

};

}//namespace cx
#endif /* CXOPENIGTLINKSESSION_H_ */
