#include "cxIGTLinkClient.h"

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

int ReceiveImage(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header)
{
  std::cerr << "Receiving IMAGE data type." << std::endl;

  // Create a message buffer to receive transform data
  igtl::ImageMessage::Pointer imgMsg;
  imgMsg = igtl::ImageMessage::New();
  imgMsg->SetMessageHeader(header);
  imgMsg->AllocatePack();

  // Receive transform data from the socket
  socket->Receive(imgMsg->GetPackBodyPointer(), imgMsg->GetPackBodySize());
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

namespace cx
{

IGTLinkClient::IGTLinkClient(QString address, int port, QObject* parent) :
  QThread(parent), mStopped(false), mAddress(address), mPort(port)
{
  std::cout << "client::create thread: " << QThread::currentThread() << std::endl;
}

void IGTLinkClient::stop()
{
  mStopped = true;
  std::cout << "stop " << mStopped << std::endl;
}

void IGTLinkClient::run()
{
  std::cout << "client::run thread: " << QThread::currentThread() << std::endl;
  std::cout << "run client thread, connecting to " << mAddress << " " << mPort << std::endl;

  //------------------------------------------------------------
  // Establish Connection
  mSocket = igtl::ClientSocket::New();
  int r = mSocket->ConnectToServer(cstring_cast(mAddress), mPort);

  if (r != 0)
  {
    std::cerr << "Cannot connect to the server." << std::endl;
    return;
  }

  //------------------------------------------------------------
  // Create a message buffer to receive header
  mHeaderMsg = igtl::MessageHeader::New();


  while (!mStopped)
  {
    tick();
    QThread::msleep(20);
  }
//
//  // start a timer for ticks
//  QTimer* timer = new QTimer;
//  std::cout << "client::timer thread: " << timer->thread() << std::endl;
//  connect(timer, SIGNAL(timeout()), this, SLOT(tick()));
//  timer->start(200); // ms
//
//  // run event loop
//  this->exec();
//
//  timer->stop();
//  delete timer;
  //------------------------------------------------------------
  // Close connection
  mSocket->CloseSocket();
  std::cout << "finished openIGTLink client thread" << std::endl;
}

void IGTLinkClient::tick()
{
  std::cout << "client::tick thread: " << QThread::currentThread() << std::endl;

  if (mStopped)
  {
    this->quit();
    return;
  }

  std::cout << "tick " << std::endl;

  //  while (1)
  //    {
  //------------------------------------------------------------
  // loop
  //for (int i = 0; i < 100; i ++)
  //  {

  // Initialize receive buffer
  mHeaderMsg->InitPack();
  // Receive generic header from the socket
  int r = mSocket->Receive(mHeaderMsg->GetPackPointer(), mHeaderMsg->GetPackSize()); // this blocks until something appears on the socket
  if (r == 0)
  {
    mSocket->CloseSocket();
    std::cout << "Socket failure: terminating " << std::endl;
    this->quit();
    return;
  }
  if (r != mHeaderMsg->GetPackSize())
  {
    std::cout << "Packet size inconsistency: ignoring. " << std::endl;
    return;
  }

  // Deserialize the header
  mHeaderMsg->Unpack();

  // Check data type and receive data body
  if (strcmp(mHeaderMsg->GetDeviceType(), "TRANSFORM") == 0)
  {
    ReceiveTransform(mSocket, mHeaderMsg);
  }
  else if (strcmp(mHeaderMsg->GetDeviceType(), "POSITION") == 0)
  {
    ReceivePosition(mSocket, mHeaderMsg);
  }
  else if (strcmp(mHeaderMsg->GetDeviceType(), "IMAGE") == 0)
  {
    ReceiveImage(mSocket, mHeaderMsg);
  }
  else if (strcmp(mHeaderMsg->GetDeviceType(), "STATUS") == 0)
  {
    ReceiveStatus(mSocket, mHeaderMsg);
  }
  else
  {
    std::cerr << "Receiving : " << mHeaderMsg->GetDeviceType() << std::endl;
    mSocket->Skip(mHeaderMsg->GetBodySizeToRead(), 0);
  }
  //      }
  //    }
}


}//end namespace cx
