/*
 * cxImageServer.cpp
 *
 *  Created on: Oct 30, 2010
 *      Author: christiana
 */
#include "cxImageSession.h"

#include <QTimer>
#include <QHostAddress>
#include "igtlOSUtil.h"
#include "igtlImageMessage.h"
#include "igtlServerSocket.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vtkMetaImageReader.h"
#include "vtkImageImport.h"

typedef vtkSmartPointer<vtkImageData> vtkImageDataPtr;

namespace cx
{


int GetTestImage(igtl::ImageMessage::Pointer& msg, const char* dir, int i);
void GetRandomTestMatrix(igtl::Matrix4x4& matrix);
igtl::ImageMessage::Pointer getImageMessage(int index);

vtkImageDataPtr loadImage(QString filename)
{
  //load the image from file
  vtkMetaImageReader* reader = vtkMetaImageReader::New();
  reader->SetFileName(filename.toStdString().c_str());
  reader->ReleaseDataFlagOn();
  reader->Update();

  return reader->GetOutput();
}



igtl::ImageMessage::Pointer getVtkImageMessage(vtkImageData* image)
{
  static int counter = 0;
  //------------------------------------------------------------
  // size parameters
  int   size[]     = {256, 256, 1};       // image dimension
  image->GetDimensions(size);
  size[2] = 1; // grab only one frame

  double spacingD[]  = {1.0, 1.0, 5.0};     // spacing (mm/pixel)
  float spacingF[]  = {1.0, 1.0, 5.0};     // spacing (mm/pixel)
  image->GetSpacing(spacingD);
  spacingF[0] = spacingD[0];
  spacingF[1] = spacingD[1];
  spacingF[2] = spacingD[2];
  int*   svsize   = size;
  int   svoffset[] = {0, 0, 0};           // sub-volume offset
  int   scalarType = -1;

  if (image->GetScalarType()==VTK_UNSIGNED_SHORT)
  {
    scalarType = igtl::ImageMessage::TYPE_UINT16;// scalar type
  }
  else if (image->GetScalarType()==VTK_UNSIGNED_CHAR)
  {
    scalarType = igtl::ImageMessage::TYPE_UINT8;// scalar type
  }
  else
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
  imgMsg->SetDeviceName("cxTestImage");
  imgMsg->SetSubVolume(svsize, svoffset);
  imgMsg->AllocateScalars();

  //------------------------------------------------------------
  // Set image data (See GetTestImage() bellow for the details)
//  GetTestImage(imgMsg, filedir, index);

  int fsize = imgMsg->GetImageSize();
  int frame = (counter++) % image->GetDimensions()[2];
  //int frame = 0;
  memcpy(imgMsg->GetScalarPointer(), image->GetScalarPointer(0,0,frame), fsize);
//  std::cout << "first voxel: ";
//  for (int i=0; i<1000; ++i)
//  {
//    //    std::cout << (int)(*reinterpret_cast<unsigned char*>(image->GetScalarPointer(0+i,0,frame))) << " ";
//    unsigned char* p = reinterpret_cast<unsigned char*>(image->GetScalarPointer());
//    std::cout << (int)(p[i]) << " ";
//  }
//  std::cout << std::endl;


//  size_t b = fread(msg->GetScalarPointer(), 1, fsize, fp);


  //------------------------------------------------------------
  // Get randome orientation matrix and set it.
  igtl::Matrix4x4 matrix;
  GetRandomTestMatrix(matrix);
  imgMsg->SetMatrix(matrix);

//  //------------------------------------------------------------
//  // Pack (serialize) and send
//  imgMsg->Pack();
//  socket->Send(imgMsg->GetPackPointer(), imgMsg->GetPackSize());
//
//  igtl::Sleep(interval); // wait

  return imgMsg;
}


igtl::ImageMessage::Pointer getImageMessage(const char* filedir, int index)
{
  //------------------------------------------------------------
  // size parameters
  int   size[]     = {256, 256, 1};       // image dimension
  float spacing[]  = {1.0, 1.0, 5.0};     // spacing (mm/pixel)
  int   svsize[]   = {256, 256, 1};       // sub-volume size
  int   svoffset[] = {0, 0, 0};           // sub-volume offset
  int   scalarType = igtl::ImageMessage::TYPE_UINT8;// scalar type

  //------------------------------------------------------------
  // Create a new IMAGE type message
  igtl::ImageMessage::Pointer imgMsg = igtl::ImageMessage::New();
  imgMsg->SetDimensions(size);
  imgMsg->SetSpacing(spacing);
  imgMsg->SetScalarType(scalarType);
  imgMsg->SetDeviceName("cxTestImage");
  imgMsg->SetSubVolume(svsize, svoffset);
  imgMsg->AllocateScalars();

  //------------------------------------------------------------
  // Set image data (See GetTestImage() bellow for the details)
  GetTestImage(imgMsg, filedir, index);

  //------------------------------------------------------------
  // Get randome orientation matrix and set it.
  igtl::Matrix4x4 matrix;
  GetRandomTestMatrix(matrix);
  imgMsg->SetMatrix(matrix);

//  //------------------------------------------------------------
//  // Pack (serialize) and send
//  imgMsg->Pack();
//  socket->Send(imgMsg->GetPackPointer(), imgMsg->GetPackSize());
//
//  igtl::Sleep(interval); // wait

  return imgMsg;
}

//------------------------------------------------------------
// Function to read test image data
int GetTestImage(igtl::ImageMessage::Pointer& msg, const char* dir, int i)
{

  //------------------------------------------------------------
  // Check if image index is in the range
  if (i < 0 || i >= 5)
    {
    std::cerr << "Image index is invalid." << std::endl;
    return 0;
    }

  //------------------------------------------------------------
  // Generate path to the raw image file
  char filename[128];
  sprintf(filename, "%s/igtlTestImage%d.raw", dir, i+1);
  std::cerr << "Reading " << filename << "...";

  //------------------------------------------------------------
  // Load raw data from the file
  FILE *fp = fopen(filename, "rb");
  if (fp == NULL)
    {
    std::cerr << "File opeining error: " << filename << std::endl;
    return 0;
    }
  int fsize = msg->GetImageSize();
  size_t b = fread(msg->GetScalarPointer(), 1, fsize, fp);

  fclose(fp);

  std::cerr << "done." << std::endl;

  return 1;
}


//------------------------------------------------------------
// Function to generate random matrix.
void GetRandomTestMatrix(igtl::Matrix4x4& matrix)
{
  float position[3];
  float orientation[4];

  /*
  // random position
  static float phi = 0.0;
  position[0] = 50.0 * cos(phi);
  position[1] = 50.0 * sin(phi);
  position[2] = 0;
  phi = phi + 0.2;

  // random orientation
  static float theta = 0.0;
  orientation[0]=0.0;
  orientation[1]=0.6666666666*cos(theta);
  orientation[2]=0.577350269189626;
  orientation[3]=0.6666666666*sin(theta);
  theta = theta + 0.1;

  igtl::Matrix4x4 matrix;
  igtl::QuaternionToMatrix(orientation, matrix);

  matrix[0][3] = position[0];
  matrix[1][3] = position[1];
  matrix[2][3] = position[2];
  */

  matrix[0][0] = 1.0;  matrix[1][0] = 0.0;  matrix[2][0] = 0.0; matrix[3][0] = 0.0;
  matrix[0][1] = 0.0;  matrix[1][1] = -1.0;  matrix[2][1] = 0.0; matrix[3][1] = 0.0;
  matrix[0][2] = 0.0;  matrix[1][2] = 0.0;  matrix[2][2] = 1.0; matrix[3][2] = 0.0;
  matrix[0][3] = 0.0;  matrix[1][3] = 0.0;  matrix[2][3] = 0.0; matrix[3][3] = 1.0;

  //igtl::PrintMatrix(matrix);
}




ImageSession::ImageSession(int socketDescriptor, QString imageFileDir, QObject* parent) :
    QThread(parent),
    mSocketDescriptor(socketDescriptor),
    mImageFileDir(imageFileDir)
{
}

ImageSession::~ImageSession()
{
//  std::cout << "ImageSession::~ImageSession()" << std::endl;
}

void ImageSession::run()
{
  mSocket = new QTcpSocket();
//  connect(mSocket, SIGNAL(disconnected()), this, SLOT(quit()));
  connect(mSocket, SIGNAL(disconnected()), this, SLOT(quit())); // quit thread when disconnected
  mSocket->setSocketDescriptor(mSocketDescriptor);
  QString clientName = mSocket->localAddress().toString();
  std::cout << "Connected to " << clientName.toStdString() << ". Session started." << std::endl;
  ImageSender* sender = new ImageSender(mSocket, mImageFileDir);
  // socket should now be connected....?

  //std::cout << "executing" << std::endl;
  this->exec();
  //std::cout << "finished executing" << std::endl;

  std::cout << "Disconnected from " << clientName.toStdString() << ". Session ended." << std::endl;
  delete sender;
  delete mSocket;
  //mSocket->deleteLater();
}

void ImageSender::setTestImage()
{
  int W = 512;
  int H = 512;

  mImageImport = vtkImageImportPtr::New();
  mImageImport->SetNumberOfScalarComponents(1);

  mImageImport->SetWholeExtent(0, W-1, 0, H-1, 0, 0);
  mImageImport->SetDataExtent(0,W-1,0,H-1,0,0);
  mImageImport->SetDataScalarTypeToUnsignedChar();
  mTestData.resize(W*H);
  std::fill(mTestData.begin(), mTestData.end(), 50);

  for (unsigned y=0; y<H; ++y)
    for (unsigned x=0; x<W; ++x)
    {
      mTestData[x+W*y] = x/2;
    }

  mImageImport->SetImportVoidPointer(&(*mTestData.begin()));
  mImageImport->Modified();
  mImageImport->GetOutput()->Update();
//  return mImageImport-GetOutput();
}

ImageSender::ImageSender(QTcpSocket* socket, QString imageFileDir, QObject* parent) :
    QObject(parent),
    mSocket(socket),
    mCounter(0),
    mImageFileDir(imageFileDir)
{
  this->setTestImage();
  mImageData = loadImage(mImageFileDir);
//  mImageData = mImageImport->GetOutput();

  mTimer = new QTimer(this);
  connect(mTimer, SIGNAL(timeout()), this, SLOT(tick())); // this signal will be executed in the thread of THIS, i.e. the main thread.
//  mTimer->start(500);
  mTimer->start(40);

}

void ImageSender::tick()
{
  std::cout << "tick" << std::endl;
  //igtl::ImageMessage::Pointer imgMsg = getImageMessage(mImageFileDir.toStdString().c_str(), (mCounter++) % 5);
  igtl::ImageMessage::Pointer imgMsg = getVtkImageMessage(mImageData);

  //------------------------------------------------------------
  // Pack (serialize) and send
  imgMsg->Pack();
  mSocket->write(reinterpret_cast<const char*>(imgMsg->GetPackPointer()), imgMsg->GetPackSize());
  //std::cout << "tock" << std::endl;
}
//{
//  //------------------------------------------------------------
//  // Prepare server socket
//  igtl::ServerSocket::Pointer serverSocket;
//  serverSocket = igtl::ServerSocket::New();
//  int r = serverSocket->CreateServer(port);
//
//  if (r < 0)
//    {
//    std::cerr << "Cannot create a server socket." << std::endl;
//    exit(0);
//    }
//
//
//  igtl::Socket::Pointer socket;
//
//  while (1)
//    {
//    //------------------------------------------------------------
//    // Waiting for Connection
//    socket = serverSocket->WaitForConnection(1000);
//
//    if (socket.IsNotNull()) // if client connected
//      {
//      //------------------------------------------------------------
//      // loop
//      for (int i = 0; i < 100; i ++)
//        {
//        igtl::ImageMessage::Pointer imgMsg = getImageMessage(i % 5);
//        //------------------------------------------------------------
//        // Pack (serialize) and send
//        imgMsg->Pack();
//        socket->Send(imgMsg->GetPackPointer(), imgMsg->GetPackSize());
//
//        igtl::Sleep(interval); // wait
//        }
//      }
//    }
//
//  //------------------------------------------------------------
//  // Close connection (The example code never reachs to this section ...)
//
//  socket->CloseSocket();
//
//}


}
