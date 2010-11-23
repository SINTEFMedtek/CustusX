#include "sscOpenIGTLinkClient.h"

#include "igtlOSUtil.h"
#include "igtlMessageHeader.h"
#include "igtlTransformMessage.h"
#include "igtlPositionMessage.h"
#include "igtlImageMessage.h"
#include "igtlClientSocket.h"
#include "igtlStatusMessage.h"

#include "sscTypeConversions.h"


int ReceiveTransform(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header)
{
  std::cerr << "Receiving TRANSFORM data type." << std::endl;

  // Create a message buffer to receive transform data
  igtl::TransformMessage::Pointer transMsg;
  transMsg = igtl::TransformMessage::New();
  transMsg->SetMessageHeader(header);
  transMsg->AllocatePack();

  // Receive transform data from the socket
  socket->Receive(transMsg->GetPackBodyPointer(), transMsg->GetPackBodySize());
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = transMsg->Unpack(1);

  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
  {
    // Retrive the transform data
    igtl::Matrix4x4 matrix;
    transMsg->GetMatrix(matrix);
    igtl::PrintMatrix(matrix);
    return 1;
  }

  return 0;
}

int ReceivePosition(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header)
{
  std::cerr << "Receiving POSITION data type." << std::endl;

  // Create a message buffer to receive transform data
  igtl::PositionMessage::Pointer positionMsg;
  positionMsg = igtl::PositionMessage::New();
  positionMsg->SetMessageHeader(header);
  positionMsg->AllocatePack();

  // Receive position position data from the socket
  socket->Receive(positionMsg->GetPackBodyPointer(), positionMsg->GetPackBodySize());
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = positionMsg->Unpack(1);

  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
  {
    // Retrive the transform data
    float position[3];
    float quaternion[4];

    positionMsg->GetPosition(position);
    positionMsg->GetQuaternion(quaternion);

    std::cerr << "position   = (" << position[0] << ", " << position[1] << ", " << position[2] << ")" << std::endl;
    std::cerr << "quaternion = (" << quaternion[0] << ", " << quaternion[1] << ", " << quaternion[2] << ", "
        << quaternion[3] << ")" << std::endl << std::endl;

    return 1;
  }

  return 0;
}



int ReceiveStatus(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header)
{

  std::cerr << "Receiving STATUS data type." << std::endl;

  // Create a message buffer to receive transform data
  igtl::StatusMessage::Pointer statusMsg;
  statusMsg = igtl::StatusMessage::New();
  statusMsg->SetMessageHeader(header);
  statusMsg->AllocatePack();

  // Receive transform data from the socket
  socket->Receive(statusMsg->GetPackBodyPointer(), statusMsg->GetPackBodySize());
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = statusMsg->Unpack(1);

  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
  {
    std::cerr << "========== STATUS ==========" << std::endl;
    std::cerr << " Code      : " << statusMsg->GetCode() << std::endl;
    std::cerr << " SubCode   : " << statusMsg->GetSubCode() << std::endl;
    std::cerr << " Error Name: " << statusMsg->GetErrorName() << std::endl;
    std::cerr << " Status    : " << statusMsg->GetStatusString() << std::endl;
    std::cerr << "============================" << std::endl;
  }

  return 0;

}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

namespace ssc
{

IGTLinkClient::IGTLinkClient(QString address, int port, QObject* parent) :
  QThread(parent), mHeadingReceived(false), mAddress(address), mPort(port)
{
//  std::cout << "client::create thread: " << QThread::currentThread() << std::endl;
}

void IGTLinkClient::run()
{
 // std::cout << "client::run thread: " << QThread::currentThread() << std::endl;
  //std::cout << "run client thread, connecting to " << mAddress << " " << mPort << std::endl;

  //------------------------------------------------------------
  // Establish Connection
  mSocket = new QTcpSocket();
  connect(mSocket, SIGNAL(readyRead()), this, SLOT(readyReadSlot()), Qt::DirectConnection);
  connect(mSocket, SIGNAL(hostFound()), this, SLOT(hostFoundSlot()), Qt::DirectConnection);
  connect(mSocket, SIGNAL(connected()), this, SLOT(connectedSlot()), Qt::DirectConnection);
  connect(mSocket, SIGNAL(disconnected()), this, SLOT(disconnectedSlot()), Qt::DirectConnection);
  connect(mSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(errorSlot(QAbstractSocket::SocketError)), Qt::DirectConnection);

  std::cout << "Looking for host: " << this->hostDescription() << std::endl;
  mSocket->connectToHost(mAddress, mPort);

  int timeout = 5000;
  if (!mSocket->waitForConnected(timeout))
  {
    std::cout << "Timeout looking for host " << this->hostDescription() << std::endl;
    mSocket->disconnectFromHost();
    return;
  }

  // Create a message buffer to receive header
  mHeaderMsg = igtl::MessageHeader::New();

  // run event loop
  this->exec();

  mSocket->disconnectFromHost();
  //std::cout << "finished openIGTLink client thread" << std::endl;
  delete mSocket;
}

QString IGTLinkClient::hostDescription() const
{
  return mAddress + ":" + qstring_cast(mPort);
}

void IGTLinkClient::hostFoundSlot()
{
  std::cout << "Host found: " << this->hostDescription() << std::endl;
}
void IGTLinkClient::connectedSlot()
{
  std::cout << "Connected to host " << this->hostDescription() << std::endl;
}
void IGTLinkClient::disconnectedSlot()
{
  std::cout << "Disconnected from host " << this->hostDescription() << std::endl;
}
void IGTLinkClient::errorSlot(QAbstractSocket::SocketError socketError)
{
  std::cout << "Socket error [Host=" << this->hostDescription() <<", Code=" << socketError << "]\n" << mSocket->errorString() << std::endl;
}

/** add the message to a thread-safe queue
 */
void IGTLinkClient::addImageToQueue(igtl::ImageMessage::Pointer imgMsg)
{
  QMutexLocker sentry(&mImageMutex);
  mMutexedImageMessageQueue.push_back(imgMsg);
  sentry.unlock();
  emit imageReceived(); // emit signal outside lock, catch possibly in another thread
}

/** Threadsafe retrieval of last image message.
 *
 */
igtl::ImageMessage::Pointer IGTLinkClient::getLastImageMessage()
{
  QMutexLocker sentry(&mImageMutex);
  if (mMutexedImageMessageQueue.empty())
    return igtl::ImageMessage::Pointer();
  igtl::ImageMessage::Pointer retval = mMutexedImageMessageQueue.front();
  mMutexedImageMessageQueue.pop_front();
  return retval;
}


void IGTLinkClient::readyReadSlot()
{
 // std::cout << "client::tick thread: " << QThread::currentThread() << std::endl;

  //std::cout << "tick " << std::endl;

  if (!mHeadingReceived)
  {
    // Initialize receive buffer
    mHeaderMsg->InitPack();

    // ignore if not enough data (yet)
    if (mSocket->bytesAvailable() < mHeaderMsg->GetPackSize())
    {
      //std::cout << "Incomplete heading received, ignoring. " << std::endl;
      //std::cout << "available: " << mSocket->bytesAvailable() << ", needed " << mHeaderMsg->GetPackSize() << std::endl;
      return;
    }

    // after peek: read to increase pos
    mSocket->read(reinterpret_cast<char*>(mHeaderMsg->GetPackPointer()), mHeaderMsg->GetPackSize());
    mHeadingReceived = true;

    // Deserialize the header
    mHeaderMsg->Unpack();
  }

  if (mHeadingReceived)
  {
    bool success = false;
    // Check data type and receive data body
//    if (strcmp(mHeaderMsg->GetDeviceType(), "TRANSFORM") == 0)
//    {
//      ReceiveTransform(mSocket, mHeaderMsg);
//    }
//    else if (strcmp(mHeaderMsg->GetDeviceType(), "POSITION") == 0)
//    {
//      ReceivePosition(mSocket, mHeaderMsg);
//    }
    if (strcmp(mHeaderMsg->GetDeviceType(), "IMAGE") == 0)
    {
      success = this->ReceiveImage(mSocket, mHeaderMsg);
    }
//    else if (strcmp(mHeaderMsg->GetDeviceType(), "STATUS") == 0)
//    {
//      ReceiveStatus(mSocket, mHeaderMsg);
//    }
    else
    {
      std::cerr << "Receiving : " << mHeaderMsg->GetDeviceType() << std::endl;
      mSocket->read(mHeaderMsg->GetBodySizeToRead());
    }

    if (success)
      mHeadingReceived = false; // restart
  }
}

bool IGTLinkClient::ReceiveImage(QTcpSocket* socket, igtl::MessageHeader::Pointer& header)
{

  // Create a message buffer to receive transform data
  igtl::ImageMessage::Pointer imgMsg;
  imgMsg = igtl::ImageMessage::New();
  imgMsg->SetMessageHeader(header);
  imgMsg->AllocatePack();

  // Receive transform data from the socket
  // ignore if not enough data (yet)
  if (socket->bytesAvailable()<imgMsg->GetPackBodySize())
  {
    //std::cout << "Incomplete body received, ignoring. " << std::endl;
    return false;
  }
  //std::cout << "Receiving IMAGE data type." << std::endl;

  socket->read(reinterpret_cast<char*>(imgMsg->GetPackBodyPointer()), imgMsg->GetPackBodySize());
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = imgMsg->Unpack(1);

  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
  {
    // Retrive the image data
    int size[3]; // image dimension
    float spacing[3]; // spacing (mm/pixel)
    int svsize[3]; // sub-volume size
    int svoffset[3]; // sub-volume offset
    int scalarType; // scalar type

    scalarType = imgMsg->GetScalarType();
    imgMsg->GetDimensions(size);
    imgMsg->GetSpacing(spacing);
    imgMsg->GetSubVolume(svsize, svoffset);

//    std::cerr << "Device Name           : " << imgMsg->GetDeviceName() << std::endl;
//    std::cerr << "Scalar Type           : " << scalarType << std::endl;
//    std::cerr << "Dimensions            : (" << size[0] << ", " << size[1] << ", " << size[2] << ")" << std::endl;
//    std::cerr << "Spacing               : (" << spacing[0] << ", " << spacing[1] << ", " << spacing[2] << ")"
//        << std::endl;
//    std::cerr << "Sub-Volume dimensions : (" << svsize[0] << ", " << svsize[1] << ", " << svsize[2] << ")" << std::endl;
//    std::cerr << "Sub-Volume offset     : (" << svoffset[0] << ", " << svoffset[1] << ", " << svoffset[2] << ")"
//        << std::endl;

    this->addImageToQueue(imgMsg);

    return true;
  }

  std::cout << "body crc failed!" << std::endl;
  return true;

}

}//end namespace ssc
