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
  connect(sender, SIGNAL(queueInfo(int, int)), this, SIGNAL(queueInfo(int, int)));

  this->exec();

  ssc::messageManager()->sendInfo("Disconnected from "+qstring_cast(clientName)+". Session ended.");
  delete sender;
  delete mSocket;
}

//------------------------------------------------------------------------------

OpenIGTLinkSender::OpenIGTLinkSender(QTcpSocket* socket, QObject* parent) :
    QObject(parent),
    mSocket(socket),
    mMaxqueueInfo(20),
    mMaxBufferSize(19200000), //800(width)*600(height)*4(bytes)*10(images)
    mDroppedImages(0)
{
  connect(this, SIGNAL(imageOnQueue(int)), this, SLOT(sendOpenIGTLinkImageSlot(int)), Qt::QueuedConnection);
  connect(mSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(errorSlot(QAbstractSocket::SocketError)), Qt::DirectConnection);
}

OpenIGTLinkSender::~OpenIGTLinkSender()
{}

void OpenIGTLinkSender::receiveFrameSlot(Frame& frame)
{
  //TODO: Get info like origin from frame and create a IGTLinkUSStatusMessage
  //IGTLinkUSStatusMessage::Pointer statMsg = getStatus(frame);

  IGTLinkImageMessage::Pointer imgMsg = convertFrame(frame);
//  std::cout << "Socket bytesToWrite: " << mSocket->bytesToWrite() << std::endl;
//  std::cout << "Socket readBufferSize: " << mSocket->readBufferSize() << std::endl;
  this->addImageToQueue(imgMsg);
}

IGTLinkImageMessage::Pointer OpenIGTLinkSender::convertFrame(Frame& frame)
{
  IGTLinkImageMessage::Pointer retval = IGTLinkImageMessage::New();

  //extract data needed variables from the frame
  int size[] = {frame.mWidth, frame.mHeight, 1};
  int offset[] = {0, 0, 0};

  // Create a new IMAGE type message
  retval->SetDimensions(size);
  //retval->SetSpacing(1,1,1); // TODO
  retval->SetSpacing(frame.mSpacing[0], frame.mSpacing[1],1);
  //std::cout << "Frame spacing: " << frame.mSpacing[0] << " " << frame.mSpacing[1] << std::endl;
  //retval->SetScalarType(igtl::ImageMessage::TYPE_UINT32);
  retval->SetScalarType(frame.mPixelFormat); //Use frame.mPixelFormat directly
  retval->SetDeviceName("GrabberServer"); // TODO write something useful here
  retval->SetSubVolume(size,offset);
  retval->AllocateScalars();

  igtl::TimeStamp::Pointer ts;
  ts = igtl::TimeStamp::New();
  double seconds = frame.mTimestamp;
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

void OpenIGTLinkSender::sendOpenIGTLinkImageSlot(int sendNumberOfImages)
{
  if(mSocket->bytesToWrite() > mMaxBufferSize)
    return;

  for(int i=0; i<sendNumberOfImages; ++i)
  {
    IGTLinkImageMessage::Pointer message = this->getLastImageMessageFromQueue();
    if(!message)
      break;
    message->Pack();
    mSocket->write(reinterpret_cast<const char*>(message->GetPackPointer()), message->GetPackSize());
  }
}

/** Add the message to a thread-safe queue
 */
void OpenIGTLinkSender::addImageToQueue(IGTLinkImageMessage::Pointer imgMsg)
{
  QMutexLocker sentry(&mImageMutex);
  if(mMutexedImageMessageQueue.size() > mMaxqueueInfo)
  {
    mMutexedImageMessageQueue.pop_front();
    mDroppedImages++;
  }

  mMutexedImageMessageQueue.push_back(imgMsg);
  int size = mMutexedImageMessageQueue.size();
  sentry.unlock();

  emit queueInfo(size, mDroppedImages);
  emit imageOnQueue(size); // emit signal outside lock, catch possibly in another thread
}

/** Threadsafe retrieval of last image message.
 */
IGTLinkImageMessage::Pointer OpenIGTLinkSender::getLastImageMessageFromQueue()
{
  QMutexLocker sentry(&mImageMutex);
  if (mMutexedImageMessageQueue.empty())
    return IGTLinkImageMessage::Pointer();
  IGTLinkImageMessage::Pointer retval = mMutexedImageMessageQueue.front();
  mMutexedImageMessageQueue.pop_front();
  return retval;
}

void OpenIGTLinkSender::errorSlot(QAbstractSocket::SocketError socketError)
{
  ssc::messageManager()->sendError("Socket error [Code="+qstring_cast(socketError)+"]\n"+mSocket->errorString());
}

//------------------------------------------------------------------------------

}//namespace cx
