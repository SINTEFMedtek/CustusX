/*
 * cxImageSenderSonix.cpp
 *
 *  Created on: Aug 15, 2011
 *      Author: Ole Vegard Solberg
 */

#include "cxImageSenderSonix.h"

#ifdef WIN32

#include <QTimer>
#include <QTime>
#include <QHostAddress>
#include "igtlOSUtil.h"
#include "igtlImageMessage.h"
#include "igtlServerSocket.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vtkMetaImageReader.h"
#include "vtkImageImport.h"
#include "vtkLookupTable.h"
#include "vtkImageMapToColors.h"
#include "vtkMetaImageWriter.h"


#include "vtkSonixVideoSource.h"

namespace cx
{

QString ImageSenderSonix::getType()
{
	return "Sonix";
}

QStringList ImageSenderSonix::getArgumentDescription()
{
	QStringList retval;
	retval << "--ipaddress:   IP address to connect to, default=127.0.0.1 (localhost)";
	retval << "--imagingmode: default=0 (0 = B-mode, 12 = RF mode)";
	retval << "--datatype: Video type, default=0x00000004 (4 = processed, 2 = unprocessed)";
	retval << "--buffersize:  Grabber buffer size,   default=500";
	retval << "--properties:  dump image properties";
	return retval;
}


ImageSenderSonix::ImageSenderSonix(QTcpSocket* socket, StringMap arguments, QObject* parent) :
    QObject(parent),
    mSocket(socket),
    mArguments(arguments),
	mMaxqueueInfo(20),
	mMaxBufferSize(19200000), //800(width)*600(height)*4(bytes)*10(images)
	mDroppedImages(0)
{
  typedef cx::Frame Frame;
  qRegisterMetaType<Frame>("Frame");

  connect(this, SIGNAL(imageOnQueue(int)), this, SLOT(sendOpenIGTLinkImageSlot(int)), Qt::QueuedConnection);
  connect(this, SIGNAL(statusOnQueue(int)), this, SLOT(sendOpenIGTLinkStatusSlot(int)), Qt::QueuedConnection);

	if (!mArguments.count("ipaddress"))
		mArguments["ipaddress"] = "127.0.0.1";
	if (!mArguments.count("imagingmode"))
		mArguments["imagingmode"] = "0";
  if (!mArguments.count("datatype"))
    mArguments["datatype"] = "0x00000004";
  if (!mArguments.count("buffersize"))
    mArguments["buffersize"] = "500";

	QString ipaddress       = mArguments["ipaddress"];
  int imagingMode         = convertStringWithDefault(mArguments["imagingmode"], 0);
	int acquisitionDataType = convertStringWithDefault(mArguments["datatype"], 0x00000004);
  int bufferSize          = convertStringWithDefault(mArguments["buffersize"], 500);



  mSonixGrabber = vtkSonixVideoSource::New();
  mSonixGrabber->SetSonixIP(ipaddress.toStdString().c_str());
  mSonixGrabber->SetImagingMode(imagingMode);
  mSonixGrabber->SetAcquisitionDataType(acquisitionDataType);
  mSonixGrabber->SetFrameBufferSize(bufferSize);  // Number of image frames in buffer
  mSonixGrabber->Initialize(); // Run initialize to set spacing and offset

  this->mSonixHelper = new SonixHelper;
  mSonixGrabber->setSonixHelper(this->mSonixHelper);
  connect(mSonixHelper, SIGNAL(frame(Frame&)), this, SLOT(receiveFrameSlot(Frame&)), Qt::DirectConnection);

  mSonixGrabber->Record();
  std::cout << "Started streaming from sonix device" << std::endl;
}

ImageSenderSonix::~ImageSenderSonix()
{
  mSonixGrabber->Stop();
  mSonixGrabber->ReleaseSystemResources();
  mSonixGrabber->Delete();
}

void ImageSenderSonix::receiveFrameSlot(Frame& frame)
{
  //TODO: Get info like origin from frame and create a IGTLinkSonixStatusMessage
  if (frame.mNewStatus)
  {
    IGTLinkSonixStatusMessage::Pointer statMsg = getFrameStatus(frame);
    this->addStatusMessageToQueue(statMsg);
    // Pack (serialize) and send
//    statMsg->Pack();
//    mSocket->write(reinterpret_cast<const char*>(statMsg->GetPackPointer()), statMsg->GetPackSize());
  }

  IGTLinkImageMessage::Pointer imgMsg = convertFrame(frame);
//  std::cout << "Socket bytesToWrite: " << mSocket->bytesToWrite() << std::endl;
//  std::cout << "Socket readBufferSize: " << mSocket->readBufferSize() << std::endl;
  this->addImageToQueue(imgMsg);


  //------------------------------------------------------------
  // Pack (serialize) and send
//  imgMsg->Pack();
//  mSocket->write(reinterpret_cast<const char*>(imgMsg->GetPackPointer()), imgMsg->GetPackSize());
}

IGTLinkSonixStatusMessage::Pointer ImageSenderSonix::getFrameStatus(Frame& frame)
{
  IGTLinkSonixStatusMessage::Pointer retval = IGTLinkSonixStatusMessage::New();
  //retval->SetOrigin(); //Origin is set in IGTLinkImageMessage
//  retval->SetWidth();
//  retval->SetType();
  return retval;
}

IGTLinkImageMessage::Pointer ImageSenderSonix::convertFrame(Frame& frame)
{
  IGTLinkImageMessage::Pointer retval = IGTLinkImageMessage::New();

  //extract data needed variables from the frame
  int size[] = {frame.mWidth, frame.mHeight, 1};
  int offset[] = {0, 0, 0};

  // Create a new IMAGE type message
  retval->SetDimensions(size);
  retval->SetSpacing(frame.mSpacing[0], frame.mSpacing[1],1);
  retval->SetOrigin(frame.mOrigin[0], frame.mOrigin[1], 0);
  //std::cout << "Frame spacing: " << frame.mSpacing[0] << " " << frame.mSpacing[1] << std::endl;
  retval->SetScalarType(frame.mPixelFormat); //Use frame.mPixelFormat directly
  retval->SetDeviceName("ImageSenderSonix"); // TODO write something useful here
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
void ImageSenderSonix::sendOpenIGTLinkImageSlot(int sendNumberOfMessages)
{
  if(mSocket->bytesToWrite() > mMaxBufferSize)
    return;

  for(int i=0; i<sendNumberOfMessages; ++i)
  {
    IGTLinkImageMessage::Pointer message = this->getLastImageMessageFromQueue();
    if(!message)
      break;
    message->Pack();
    mSocket->write(reinterpret_cast<const char*>(message->GetPackPointer()), message->GetPackSize());
  }
}
void ImageSenderSonix::sendOpenIGTLinkStatusSlot(int sendNumberOfMessage)
{
  std::cout << "ImageSenderSonix::sendOpenIGTLinkStatusSlot" << std::endl;
  if(mSocket->bytesToWrite() > mMaxBufferSize)
    return;

  for(int i=0; i<sendNumberOfMessage; ++i)
  {
    IGTLinkSonixStatusMessage::Pointer message = this->getLastStatusMessageFromQueue();
    if(!message)
      break;
    message->Pack();
    mSocket->write(reinterpret_cast<const char*>(message->GetPackPointer()), message->GetPackSize());
  }
}
/** Add the image message to a thread-safe queue
 */
void ImageSenderSonix::addImageToQueue(IGTLinkImageMessage::Pointer msg)
{
  QMutexLocker sentry(&mImageMutex);
  if(mMutexedImageMessageQueue.size() > mMaxqueueInfo)
  {
    mMutexedImageMessageQueue.pop_front();
    mDroppedImages++;
  }

  mMutexedImageMessageQueue.push_back(msg);
  int size = mMutexedImageMessageQueue.size();
  sentry.unlock();

  emit queueInfo(size, mDroppedImages);
  emit imageOnQueue(size); // emit signal outside lock, catch possibly in another thread
}

/** Threadsafe retrieval of last image message.
 */
IGTLinkImageMessage::Pointer ImageSenderSonix::getLastImageMessageFromQueue()
{
  QMutexLocker sentry(&mImageMutex);
  if (mMutexedImageMessageQueue.empty())
    return IGTLinkImageMessage::Pointer();
  IGTLinkImageMessage::Pointer retval = mMutexedImageMessageQueue.front();
  mMutexedImageMessageQueue.pop_front();
  return retval;
}

/** Add the status message to a thread-safe queue
 */
void ImageSenderSonix::addStatusMessageToQueue(IGTLinkSonixStatusMessage::Pointer msg)
{
  QMutexLocker sentry(&mStatusMutex);
  if(mMutexedStatusMessageQueue.size() > mMaxqueueInfo)
  {
    mMutexedStatusMessageQueue.pop_front();
  }

  mMutexedStatusMessageQueue.push_back(msg);
  int size = mMutexedStatusMessageQueue.size();
  sentry.unlock();
  emit statusOnQueue(size); // emit signal outside lock, catch possibly in another thread
}

/** Threadsafe retrieval of last image message.
 */
IGTLinkSonixStatusMessage::Pointer ImageSenderSonix::getLastStatusMessageFromQueue()
{
  QMutexLocker sentry(&mStatusMutex);
  if (mMutexedStatusMessageQueue.empty())
    return IGTLinkSonixStatusMessage::Pointer();
  IGTLinkSonixStatusMessage::Pointer retval = mMutexedStatusMessageQueue.front();
  mMutexedStatusMessageQueue.pop_front();
  return retval;
}
//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------

}

#endif // WIN32

