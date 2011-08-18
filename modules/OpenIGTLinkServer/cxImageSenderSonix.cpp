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

namespace
{
//------------------------------------------------------------
// Function to generate random matrix.
//void GetRandomTestMatrix(igtl::Matrix4x4& matrix)
//{
//  //float position[3];
//  //float orientation[4];
//
//  matrix[0][0] = 1.0;  matrix[1][0] = 0.0;  matrix[2][0] = 0.0; matrix[3][0] = 0.0;
//  matrix[0][1] = 0.0;  matrix[1][1] = -1.0;  matrix[2][1] = 0.0; matrix[3][1] = 0.0;
//  matrix[0][2] = 0.0;  matrix[1][2] = 0.0;  matrix[2][2] = 1.0; matrix[3][2] = 0.0;
//  matrix[0][3] = 0.0;  matrix[1][3] = 0.0;  matrix[2][3] = 0.0; matrix[3][3] = 1.0;
//}

}

namespace cx
{

QString ImageSenderSonix::getType()
{
	return "Sonix";
}

QStringList ImageSenderSonix::getArgumentDescription()
{
	QStringList retval;
	retval << "--ipaddress: IP  address to connect to, default=127.0.0.1 (localhost)";
  retval << "--imagingmode:   Imaging mode,          default=0 (0 = B-mode, 12 = RF mode)";
  retval << "--datatype:      Video data type,       default=0x00000004 (4 = processed video, 2 = unprocessed)";
  retval << "--buffersize:    Grabber buffer size,   default=500";
  retval << "--properties:    dump image properties";
	return retval;
}


ImageSenderSonix::ImageSenderSonix(QTcpSocket* socket, StringMap arguments, QObject* parent) :
    QObject(parent),
    mSocket(socket),
    mArguments(arguments)
{
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
  connect(mSonixHelper, SIGNAL(frame(Frame&)), this, SIGNAL(receiveFrameSlot(Frame&)));

  mSonixGrabber->Record();
  std::cout << "Started streaming from sonix device" << std::endl;

//	QString videoSource = mArguments["videoport"];
//	int videoport = convertStringWithDefault(mArguments["videoport"], 0);
//	int height = convertStringWithDefault(mArguments["height"], 768);
//	int width = convertStringWithDefault(mArguments["width"], 1024);

//  mVideoCapture.open(videoSource.toStdString().c_str());
//  if (!mVideoCapture.isOpened()) //if this fails, try to open as a video camera, through the use of an integer param
//    mVideoCapture.open(videoport);
//  if (!mVideoCapture.isOpened())
//  {
//    cerr << "Failed to open a video device or video file!\n" << endl;
//    return;
//  }
//  else
//  {
//	  mVideoCapture.set(CV_CAP_PROP_FRAME_WIDTH, width);
//	  mVideoCapture.set(CV_CAP_PROP_FRAME_HEIGHT, height);
//
//	  if (mArguments.count("properties"))
//	  	this->dumpProperties();
//
//	  std::cout << "started streaming from openCV device " << videoSource.toStdString() << ", size=(" << width << "," << height << ")" << std::endl;
//  }

//  mImageData = loadImage(mImageFileDir);

//  mTimer = new QTimer(this);
//  connect(mTimer, SIGNAL(timeout()), this, SLOT(tick())); // this signal will be executed in the thread of THIS, i.e. the main thread.
//  mTimer->start(40);
//  mTimer->start(1200); // for test of the timeout feature
}

ImageSenderSoni:~ImageSenderSonix()
{
  mSonixGrabber->Stop();
  mSonixGrabber->ReleaseSystemResources();
  mSonixGrabber->Delete();
}

void ImageSenderSonix::dumpProperties()
{
  this->dumpProperty(CV_CAP_PROP_POS_MSEC, "CV_CAP_PROP_POS_MSEC");
  this->dumpProperty(CV_CAP_PROP_POS_FRAMES, "CV_CAP_PROP_POS_FRAMES");
  this->dumpProperty(CV_CAP_PROP_POS_AVI_RATIO, "CV_CAP_PROP_POS_AVI_RATIO");
  this->dumpProperty(CV_CAP_PROP_FRAME_WIDTH, "CV_CAP_PROP_FRAME_WIDTH");
  this->dumpProperty(CV_CAP_PROP_FRAME_HEIGHT, "CV_CAP_PROP_FRAME_HEIGHT");
  this->dumpProperty(CV_CAP_PROP_FPS, "CV_CAP_PROP_FPS");
  this->dumpProperty(CV_CAP_PROP_FOURCC, "CV_CAP_PROP_FOURCC");
  this->dumpProperty(CV_CAP_PROP_FRAME_COUNT, "CV_CAP_PROP_FRAME_COUNT");
  this->dumpProperty(CV_CAP_PROP_FORMAT, "CV_CAP_PROP_FORMAT");
  this->dumpProperty(CV_CAP_PROP_MODE, "CV_CAP_PROP_MODE");
  this->dumpProperty(CV_CAP_PROP_BRIGHTNESS, "CV_CAP_PROP_BRIGHTNESS");
  this->dumpProperty(CV_CAP_PROP_CONTRAST, "CV_CAP_PROP_CONTRAST");
  this->dumpProperty(CV_CAP_PROP_SATURATION, "CV_CAP_PROP_SATURATION");
  this->dumpProperty(CV_CAP_PROP_HUE, "CV_CAP_PROP_HUE");
  this->dumpProperty(CV_CAP_PROP_GAIN, "CV_CAP_PROP_GAIN");
  this->dumpProperty(CV_CAP_PROP_EXPOSURE, "CV_CAP_PROP_EXPOSURE");
  this->dumpProperty(CV_CAP_PROP_CONVERT_RGB, "CV_CAP_PROP_CONVERT_RGB");
//  this->dumpProperty(CV_CAP_PROP_WHITE_BALANCE, "CV_CAP_PROP_WHITE_BALANCE");
  this->dumpProperty(CV_CAP_PROP_RECTIFICATION, "CV_CAP_PROP_RECTIFICATION");
}

void ImageSenderSonix::dumpProperty(int val, QString name)
{
	double value = mVideoCapture.get(val);
	if (value!=-1)
		std::cout << "Property " << name.toStdString() << " : " << mVideoCapture.get(val) << std::endl;
}

/*void ImageSenderSonix::tick()
{
//  std::cout << "tick" << std::endl;
//  QTime start = QTime::currentTime();
  igtl::ImageMessage::Pointer imgMsg = this->getImageMessage();

  //------------------------------------------------------------
  // Pack (serialize) and send
  imgMsg->Pack();
  mSocket->write(reinterpret_cast<const char*>(imgMsg->GetPackPointer()), imgMsg->GetPackSize());
//  std::cout << "tick " << start.msecsTo(QTime::currentTime()) << " ms" << std::endl;
}*/

void ImageSenderSonix::receiveFrameSlot(Frame& frame)
{
  //TODO: Get info like origin from frame and create a IGTLinkSonixStatusMessage
  if (frame.mNewStatus)
  {
    IGTLinkSonixStatusMessage::Pointer statMsg = getFrameStatus(frame);
    // Pack (serialize) and send
    statMsg->Pack();
    mSocket->write(reinterpret_cast<const char*>(statMsg->GetPackPointer()), statMsg->GetPackSize());
  }

  IGTLinkImageMessage::Pointer imgMsg = convertFrame(frame);
//  std::cout << "Socket bytesToWrite: " << mSocket->bytesToWrite() << std::endl;
//  std::cout << "Socket readBufferSize: " << mSocket->readBufferSize() << std::endl;
//  this->addImageToQueue(imgMsg);


  //------------------------------------------------------------
  // Pack (serialize) and send
  imgMsg->Pack();
  mSocket->write(reinterpret_cast<const char*>(imgMsg->GetPackPointer()), imgMsg->GetPackSize());
}

IGTLinkSonixStatusMessage::Pointer ImageSenderSonix::getFrameStatus(Frame& frame)
{
  IGTLinkSonixStatusMessage::Pointer retval = IGTLinkSonixStatusMessage::New();
  //retval->SetOrigin(); //Origin is set in IGTLinkImageMessage
  retval->SetWidth();
  retval->SetType();
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

/*igtl::ImageMessage::Pointer ImageSenderOpenCV::getImageMessage()
{
  if (!mVideoCapture.isOpened())
    return igtl::ImageMessage::Pointer();

//  QTime start = QTime::currentTime();

  cv::Mat frame;
  mVideoCapture >> frame;

//  std::cout << "grab " << start.msecsTo(QTime::currentTime()) << " ms" << std::endl;
//  std::cout << "WH=("<< frame.cols << "," << frame.rows << ")" << ", Channels,Depth=("<< frame.channels() << "," << frame.depth() << ")" << std::endl;

  if (!frame.isContinuous())
  {
    std::cout << "Error: Non-continous frame data." << std::endl;
    return igtl::ImageMessage::Pointer();
  }

  int size[]  = {1.0, 1.0, 1.0};     // spacing (mm/pixel)
  size[0] = frame.cols;
  size[1] = frame.rows;

  float spacingF[]  = {1.0, 1.0, 1.0};     // spacing (mm/pixel)
  int*   svsize   = size;
  int   svoffset[] = {0, 0, 0};           // sub-volume offset
  int   scalarType = -1;

  if (frame.channels()==3 || frame.channels()==4)
  {
      scalarType = igtl::ImageMessage::TYPE_UINT32;// scalar type
  }
  else if (frame.channels()==1)
  {
    if (frame.depth()==16)
    {
      scalarType = igtl::ImageMessage::TYPE_UINT16;// scalar type
    }
    else if (frame.depth()==8)
    {
      scalarType = igtl::ImageMessage::TYPE_UINT8;// scalar type
    }
  }

  if (scalarType==-1)
  {
    std::cerr << "unknown image type" << std::endl;
    exit(0);
  }
  //------------------------------------------------------------
  // Create a new IMAGE type message
  igtl::ImageMessage::Pointer imgMsg = igtl::ImageMessage::New();
  imgMsg->SetDimensions(size);
  imgMsg->SetSpacing(spacingF);
  imgMsg->SetScalarType(scalarType);
  imgMsg->SetDeviceName("cxOpenCVGrabber");
  imgMsg->SetSubVolume(svsize, svoffset);
  imgMsg->AllocateScalars();

  unsigned char* destPtr = reinterpret_cast<unsigned char*>(imgMsg->GetScalarPointer());
  uchar* src = frame.data;
//  std::cout << "pre copy " << start.msecsTo(QTime::currentTime()) << " ms" << std::endl;
  int N = size[0]*size[1];

  if (frame.channels()>=3)
  {
    for (int i=0; i<N; ++i)
    {
        *destPtr++ = 255;
        *destPtr++ = src[2];
        *destPtr++ = src[1];
        *destPtr++ = src[0];
        src+=3;
    }
  }
  if (frame.channels()==1)
  { // BW camera
    for (int i=0; i<N; ++i)
    {
        *destPtr++ = *src++;
    }
  }

  //------------------------------------------------------------
  // Get randome orientation matrix and set it.
  igtl::Matrix4x4 matrix;
  GetRandomTestMatrix(matrix);
  imgMsg->SetMatrix(matrix);

//  std::cout << "grab+process " << start.msecsTo(QTime::currentTime()) << " ms" << std::endl;

  return imgMsg;
}*/

//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------

}

#endif // WIN32

