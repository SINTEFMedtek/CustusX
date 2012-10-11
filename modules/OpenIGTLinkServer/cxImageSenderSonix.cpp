/*
 * cxImageSenderSonix.cpp
 *
 *  \date Aug 15, 2011
 *      \author Ole Vegard Solberg
 */

#include "cxImageSenderSonix.h"
#include "sscVector3D.h"

#ifdef CX_WIN32

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
	retval << "--ipaddress:	IP address to connect to, default=127.0.0.1 (localhost)";
	retval << "--imagingmode:	default=2 (0 = B-mode, 2 = Color)";//, 6 = Dual, 12 = RF)";
	retval << "--datatype:	Video type, default=0x008 (4 = gray, 8 = color)";
	retval << "--buffersize:	Grabber buffer size,   default=500";
	retval << "--properties:	dump image properties";
	retval << "--debug:	Debug output";
	retval << "--delay:	Delay (sec) before connecting to Sonix (first time), default=80";
	return retval;
}


ImageSenderSonix::ImageSenderSonix(QObject* parent) :
    ImageSender(parent),
	mSocket(0),
	mEmitStatusMessage(false),
	mLastFrameTimestamp(0.0),
	mCurrentFrameTimestamp(0.0)
{
}

ImageSenderSonix::~ImageSenderSonix()
{
	mTimer->stop();
	if (mSonixGrabber)
		{
			mSonixGrabber->Stop();
			std::cout << "Releasing Ultrasonix resources" << std::endl;
			mSonixGrabber->ReleaseSystemResources();
		}
}


void ImageSenderSonix::initialize(StringMap arguments)
{
	std::cout << "Creating sender type Sonix" << std::endl;
		  
    mArguments = arguments;
	
	mMaxqueueInfo = 20;
//	mMaxBufferSize = 19200000; //800(width)*600(height)*4(bytes)*10(images)
	mDroppedImages = 0;

	typedef cx::Frame Frame;
	qRegisterMetaType<Frame>("Frame");
	
	connect(this, SIGNAL(imageOnQueue(int)), this, SLOT(sendOpenIGTLinkImageSlot(int)), Qt::QueuedConnection);
	connect(this, SIGNAL(statusOnQueue(int)), this, SLOT(sendOpenIGTLinkStatusSlot(int)), Qt::QueuedConnection);//Do not work yet

	this->mSonixHelper = new SonixHelper();

	mTimer = new QTimer;
	connect(mTimer, SIGNAL(timeout()), this, SLOT(initializeSonixSlot()));
	mTimer->setInterval(10000);
	mTimer->start();

	this->initializeSonixGrabber();
}


void ImageSenderSonix::initializeSonixSlot()
{
	if(!mSonixGrabber->IsInitialized())
	{
		//std::cout << "initializeSonixSlot() Initializing..." << std::endl;
		mSonixGrabber->Initialize();
		return;
	}

	//Don't reinitialize if in freeze state
	if(!mSonixGrabber->getFreezeState() && ssc::similar(mLastFrameTimestamp, mCurrentFrameTimestamp, 0.001))
	{
		std::cout << "initializeSonixSlot() Got no new frame. Reinitializing..." << std::endl;
		// If sonix exam is closed we need to create a new mSonixGrabber
		// Free resources. Do we need to delete?
		mSonixGrabber->Stop();
		std::cout << "Releasing Ultrasonix resources" << std::endl;
		mSonixGrabber->ReleaseSystemResources();
		disconnect(mSonixHelper, SIGNAL(frame(Frame&)), this, SLOT(receiveFrameSlot(Frame&)));
		//mSonixGrabber->Delete();

		this->initializeSonixGrabber();
	}
	else
	{
		mLastFrameTimestamp = mCurrentFrameTimestamp;
	}
}

void ImageSenderSonix::initializeSonixGrabber()
{
	if (!mArguments.count("ipaddress"))
		mArguments["ipaddress"] = "127.0.0.1";
	if (!mArguments.count("imagingmode"))
		mArguments["imagingmode"] = "0";
	if (!mArguments.count("datatype"))
		mArguments["datatype"] = "0x00000008";
	if (!mArguments.count("buffersize"))
		mArguments["buffersize"] = "500";
	if (!mArguments.count("delay"))
		mArguments["buffersize"] = "80";

	QString ipaddress       = mArguments["ipaddress"];
	int imagingMode         = convertStringWithDefault(mArguments["imagingmode"], 0);
	int acquisitionDataType = convertStringWithDefault(mArguments["datatype"], 0x00000008);
	int bufferSize          = convertStringWithDefault(mArguments["buffersize"], 500);
	int delay          		= convertStringWithDefault(mArguments["delay"], 80);



	mSonixGrabber = vtkSonixVideoSource::New();
	if (mArguments.count("debug"))
		mSonixGrabber->setDebugOutput(true);
	else
		mSonixGrabber->setDebugOutput(false);
	mSonixGrabber->setSonixConnectionDelay(delay);
	mSonixGrabber->SetSonixIP(ipaddress.toStdString().c_str());
	mSonixGrabber->SetImagingMode(imagingMode);
	mSonixGrabber->SetAcquisitionDataType(acquisitionDataType);
	mSonixGrabber->SetFrameBufferSize(bufferSize);  // Number of image frames in buffer
	mSonixGrabber->Initialize(); // Run initialize to set spacing and offset

	//std::cout << "imagingMode: " << imagingMode << std::endl;
	//std::cout << "datatype: " << mArguments["datatype"].toStdString().c_str() << std::endl;
	//std::cout << "acquisitionDataType: " << acquisitionDataType << " ";
	//std::cout << "GetAcquisitionDataType: " << mSonixGrabber->GetAcquisitionDataType() << std::endl;

	mSonixGrabber->setSonixHelper(this->mSonixHelper);
	connect(mSonixHelper, SIGNAL(frame(Frame&)), this, SLOT(receiveFrameSlot(Frame&)), Qt::DirectConnection);
}

void ImageSenderSonix::startStreaming(QTcpSocket* socket)
{
	mSocket = socket;
	mSonixGrabber->Record();
	mEmitStatusMessage = true;
	std::cout << "Started streaming from sonix device" << std::endl;
}

void ImageSenderSonix::stopStreaming()
{
  mSonixGrabber->Stop();
  mSocket = NULL;
}

void ImageSenderSonix::receiveFrameSlot(Frame& frame)
{
	mCurrentFrameTimestamp = frame.mTimestamp;

	if(!mSocket)
		{
			return;
		}

  //TODO: Get info like origin from frame and create a IGTLinkUSStatusMessage
  if (frame.mNewStatus || mEmitStatusMessage)
  {
    IGTLinkUSStatusMessage::Pointer statMsg = getFrameStatus(frame);
	//double spacing[3];
	//statMsg->GetSpacing(spacing);
	//std::cout << "Spacing3: " << spacing[0] << ", " << spacing[1] << ", " << spacing[2] << std::endl;
    this->addStatusMessageToQueue(statMsg);
    // Pack (serialize) and send
//    statMsg->Pack();
//    mSocket->write(reinterpret_cast<const char*>(statMsg->GetPackPointer()), statMsg->GetPackSize());
    mEmitStatusMessage = false;
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

IGTLinkUSStatusMessage::Pointer ImageSenderSonix::getFrameStatus(Frame& frame)
{
  IGTLinkUSStatusMessage::Pointer retval = IGTLinkUSStatusMessage::New();
  //retval->SetOrigin(); //Origin is set in IGTLinkImageMessage
//  retval->SetWidth();
//  retval->SetType();

//  retval->SetROI(frame.ulx, frame.uly, frame.urx, frame.ury, frame.brx, frame.bry, frame.blx, frame.bly);

  //TODO: Only dummy values. Calculate real values
  retval->SetOrigin(frame.mOrigin[0], frame.mOrigin[1], 0);
  retval->SetProbeType(2); 		// 1 = sector, 2 = linear
//  retval->SetDepthStart(10.0);// Start of sector in mm from origin
//  retval->SetDepthEnd(40.0);	// End of sector in mm from origin
//  retval->SetWidth(30.0);			// Width of sector in mm for LINEAR, Width of sector in radians for SECTOR.
  retval->SetDepthStart((frame.uly-frame.mOrigin[1]) * frame.mSpacing[1]);// Start of sector in mm from origin
  retval->SetDepthEnd((frame.bly-frame.mOrigin[1]) * frame.mSpacing[1]);	// End of sector in mm from origin
  //As ROI is a bit wide we subtract the width by 0.3 mm
  retval->SetWidth((frame.urx -  frame.ulx ) * frame.mSpacing[0] - 0.3);			// Width of sector in mm for LINEAR, Width of sector in radians for SECTOR.

  //std::cout << "uly: " << frame.uly << " bly: " << frame.bly << std::endl;
  //std::cout << "spacing: " << frame.mSpacing[0] << "  " << frame.mSpacing[1] << std::endl;
//  std::cout << "Origin: " << frame.mOrigin[0] << " " << frame.mOrigin[1] << " " << std::endl;
//  std::cout << "Probetype: " << retval->GetProbeType() << std::endl;
//  std::cout << "Depth start: " << retval->GetDepthStart();
//  std::cout << " end: " << retval->GetDepthEnd();
//  std::cout << " width: " << retval->GetWidth() << std::endl;
  //  retval->SetSpacing(frame.mSpacing[0], frame.mSpacing[1],1);
  //std::cout << "Spacing: " << frame.mSpacing[0] << ", " << frame.mSpacing[1] << std::endl;

  //double spacing[3];
  //retval->GetSpacing(spacing);
  //std::cout << "Spacing2: " << spacing[0] << ", " << spacing[1] << ", " << spacing[2] << std::endl;

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
  
  retval->SetOrigin(frame.mOrigin[0], frame.mOrigin[1], 0);//Set origin after frame is set
  // Set image data
  int fsize = retval->GetImageSize();
  memcpy(retval->GetScalarPointer(), frame.mFirstPixel, fsize); // not sure if we need to copy
  
  //float origin[3];
  //retval->GetOrigin(origin);
  //std::cout << "origin3: " << origin[0] << " " << origin[1] << " " << origin[2] << " " << std::endl;
  //float spacing[3];
  //retval->GetSpacing(spacing);
  //std::cout << "spacing: " << spacing[0] << " " << spacing[1] << " " << spacing[2] << " " << std::endl;

  //int dimensions[3];
  //retval->GetDimensions(dimensions);
  //std::cout << "dimensions: " << dimensions[0] << " " << dimensions[1] << " " << dimensions[2] << " " << std::endl;
  return retval;
}
void ImageSenderSonix::sendOpenIGTLinkImageSlot(int sendNumberOfMessages)
{
	if(!mSocket)
		return;
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
	if(!mSocket)
		return;
  //std::cout << "ImageSenderSonix::sendOpenIGTLinkStatusSlot" << std::endl;
  if(mSocket->bytesToWrite() > mMaxBufferSize)
    return;

  for(int i=0; i<sendNumberOfMessage; ++i)
  {
    IGTLinkUSStatusMessage::Pointer message = this->getLastStatusMessageFromQueue();
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
void ImageSenderSonix::addStatusMessageToQueue(IGTLinkUSStatusMessage::Pointer msg)
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
IGTLinkUSStatusMessage::Pointer ImageSenderSonix::getLastStatusMessageFromQueue()
{
  QMutexLocker sentry(&mStatusMutex);
  if (mMutexedStatusMessageQueue.empty())
    return IGTLinkUSStatusMessage::Pointer();
  IGTLinkUSStatusMessage::Pointer retval = mMutexedStatusMessageQueue.front();
  mMutexedStatusMessageQueue.pop_front();
  return retval;
}
//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------

}

#endif // CX_WIN32

