#include "cxOpenIGTLinkSession.h"

#include <QHostAddress>
#include "sscMessageManager.h"
#include "sscTypeConversions.h"

namespace cx
{
//------------------------------------------------------------------------------

OpenIGTLinkSession::OpenIGTLinkSession(int socketDesctiptor, QObject* parent) :
    mSocketDescriptor(socketDesctiptor)
{}

OpenIGTLinkSession::~OpenIGTLinkSession()
{}

void OpenIGTLinkSession::run()
{
  mSocket = new QTcpSocket();
  mSocket->setSocketDescriptor(mSocketDescriptor);
  connect(mSocket, SIGNAL(disconnected()), this, SLOT(quit()), Qt::DirectConnection); // quit thread when disconnected

  QString clientName = mSocket->peerAddress().toString();
  ssc::messageManager()->sendInfo("Connected to "+qstring_cast(clientName)+". Session started.");

  OpenIGTLinkSender* sender = new OpenIGTLinkSender(mSocket);
  connect(this, SIGNAL(frame(Frame&)), sender, SLOT(receiveFrameSlot(Frame&)), Qt::DirectConnection);

  this->exec();

  ssc::messageManager()->sendInfo("Disconnected from "+qstring_cast(clientName)+". Session ended.");
  delete sender;
  delete mSocket;
}

//------------------------------------------------------------------------------

OpenIGTLinkSender::OpenIGTLinkSender(QTcpSocket* socket, QObject* parent) :
    QObject(parent),
    mSocket(socket)
{
  connect(this, SIGNAL(imageOnQueue()), this, SLOT(sendOpenIGTLinkImageSlot()), Qt::QueuedConnection);
  connect(mSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(errorSlot(QAbstractSocket::SocketError)), Qt::DirectConnection);
}

OpenIGTLinkSender::~OpenIGTLinkSender()
{}

void OpenIGTLinkSender::receiveFrameSlot(Frame& frame)
{
  igtl::ImageMessage::Pointer imgMsg = convertFrame(frame);
  this->addImageToQueue(imgMsg);
}

igtl::ImageMessage::Pointer OpenIGTLinkSender::convertFrame(Frame& frame)
{
  igtl::ImageMessage::Pointer retval = igtl::ImageMessage::New();

  //extract data needed variables from the frame
  int size[] = {frame.mWidth, frame.mHeight, 1};
  int offset[] = {0, 0, 0};

  // Create a new IMAGE type message
  retval->SetDimensions(size);
  retval->SetSpacing(1,1,1);
  retval->SetScalarType(igtl::ImageMessage::TYPE_UINT32); //TODO
  retval->SetDeviceName("GrabberServer"); // TODO write something useful here
  retval->SetSubVolume(size,offset);
  retval->AllocateScalars();

  igtl::TimeStamp::Pointer ts;
  ts = igtl::TimeStamp::New();
  double seconds = frame.mTimestamp*1000;
  ts->SetTime(seconds); //in seconds
  retval->SetTimeStamp(ts);

  igtl::Matrix4x4 matrix;
  matrix[0][0] = 1.0;  matrix[1][0] = 0.0;  matrix[2][0] = 0.0; matrix[3][0] = 0.0;
  matrix[0][1] = 0.0;  matrix[1][1] = 1.0;  matrix[2][1] = 0.0; matrix[3][1] = 0.0;
  matrix[0][2] = 0.0;  matrix[1][2] = 0.0;  matrix[2][2] = 1.0; matrix[3][2] = 0.0;
  matrix[0][3] = 0.0;  matrix[1][3] = 0.0;  matrix[2][3] = 0.0; matrix[3][3] = 1.0;
  retval->SetMatrix(matrix);

  // Set image data
  int fsize = retval->GetImageSize();
  memcpy(retval->GetScalarPointer(), frame.mFirstPixel, fsize); // not sure if we need to copy

  return retval;
}

void OpenIGTLinkSender::sendOpenIGTLinkImageSlot()
{
  igtl::ImageMessage::Pointer message = this->getLastImageMessageFromQueue();
  message->Pack();
  mSocket->write(reinterpret_cast<const char*>(message->GetPackPointer()), message->GetPackSize());
}

/** Add the message to a thread-safe queue
 */
void OpenIGTLinkSender::addImageToQueue(igtl::ImageMessage::Pointer imgMsg)
{
  QMutexLocker sentry(&mImageMutex);
  mMutexedImageMessageQueue.push_back(imgMsg);
  sentry.unlock();
  ssc::messageManager()->sendInfo("Images in queue: "+qstring_cast(mMutexedImageMessageQueue.size()));
  emit imageOnQueue(); // emit signal outside lock, catch possibly in another thread
}

/** Threadsafe retrieval of last image message.
 */
igtl::ImageMessage::Pointer OpenIGTLinkSender::getLastImageMessageFromQueue()
{
  QMutexLocker sentry(&mImageMutex);
  if (mMutexedImageMessageQueue.empty())
    return igtl::ImageMessage::Pointer();
  igtl::ImageMessage::Pointer retval = mMutexedImageMessageQueue.front();
  mMutexedImageMessageQueue.pop_front();
  return retval;
}

void OpenIGTLinkSender::errorSlot(QAbstractSocket::SocketError socketError)
{
  ssc::messageManager()->sendError("Socket error [Code="+qstring_cast(socketError)+"]\n"+mSocket->errorString());
}

//------------------------------------------------------------------------------

}//namespace cx
