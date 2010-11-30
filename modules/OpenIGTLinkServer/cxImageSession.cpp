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
#include "vtkLookupTable.h"
#include "vtkImageMapToColors.h"

typedef vtkSmartPointer<vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<vtkImageMapToColors> vtkImageMapToColorsPtr;
typedef vtkSmartPointer<vtkLookupTable> vtkLookupTablePtr;

namespace cx
{

void GetRandomTestMatrix(igtl::Matrix4x4& matrix);

vtkImageDataPtr loadImage(QString filename)
{
  std::cout << "reading image " << filename.toStdString() << std::endl;
  //load the image from file
  vtkMetaImageReader* reader = vtkMetaImageReader::New();
  reader->SetFileName(filename.toStdString().c_str());
  reader->ReleaseDataFlagOn();
  reader->Update();

  return reader->GetOutput();
}

vtkImageDataPtr convertToTestColorImage(vtkImageDataPtr input)
{
    int N = 1400;
    //make a default system set lookuptable, grayscale...
    vtkLookupTablePtr lut = vtkLookupTablePtr::New();
    lut->SetNumberOfTableValues(N);
    //lut->SetTableRange (0, 1024); // the window of the input
    lut->SetTableRange (0, N-1); // the window of the input
    lut->SetSaturationRange (0, 0.5);
    lut->SetHueRange (0, 1);
    lut->SetValueRange (0, 1);
    lut->Build();

//    vtkDataSetMapperPtr mapper = vtkDataSetMapper::New();
//    mapper->SetInput(input);
//    mapper->SetLookupTable(lut);
//    mapper->GetOutputPort()->Print(std::cout);

    vtkImageMapToColorsPtr mapper = vtkImageMapToColorsPtr::New();
    mapper->SetInput(input);
    mapper->SetLookupTable(lut);
    mapper->Update();
    return mapper->GetOutput();
//    return mapper->GetOutputPort();
}

igtl::ImageMessage::Pointer getVtkImageMessage(vtkImageData* image)
{
  static int staticCounter = 0;
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

  if (image->GetNumberOfScalarComponents()==4)
  {
    if (image->GetScalarType()==VTK_UNSIGNED_CHAR)
    {
      scalarType = igtl::ImageMessage::TYPE_UINT32;// scalar type
    }
  }

  if (image->GetNumberOfScalarComponents()==1)
  {
    if (image->GetScalarType()==VTK_UNSIGNED_SHORT)
    {
      scalarType = igtl::ImageMessage::TYPE_UINT16;// scalar type
    }
    else if (image->GetScalarType()==VTK_UNSIGNED_CHAR)
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
  imgMsg->SetDeviceName("cxTestImage");
  imgMsg->SetSubVolume(svsize, svoffset);
  imgMsg->AllocateScalars();

  //------------------------------------------------------------
  // Set image data (See GetTestImage() bellow for the details)
//  GetTestImage(imgMsg, filedir, index);

  int fsize = imgMsg->GetImageSize();
  int frame = (staticCounter++) % image->GetDimensions()[2];
  memcpy(imgMsg->GetScalarPointer(), image->GetScalarPointer(0,0,frame), fsize); // not sure if we need to copy

  //------------------------------------------------------------
  // Get randome orientation matrix and set it.
  igtl::Matrix4x4 matrix;
  GetRandomTestMatrix(matrix);
  imgMsg->SetMatrix(matrix);

  return imgMsg;
}


//------------------------------------------------------------
// Function to generate random matrix.
void GetRandomTestMatrix(igtl::Matrix4x4& matrix)
{
  float position[3];
  float orientation[4];

  matrix[0][0] = 1.0;  matrix[1][0] = 0.0;  matrix[2][0] = 0.0; matrix[3][0] = 0.0;
  matrix[0][1] = 0.0;  matrix[1][1] = -1.0;  matrix[2][1] = 0.0; matrix[3][1] = 0.0;
  matrix[0][2] = 0.0;  matrix[1][2] = 0.0;  matrix[2][2] = 1.0; matrix[3][2] = 0.0;
  matrix[0][3] = 0.0;  matrix[1][3] = 0.0;  matrix[2][3] = 0.0; matrix[3][3] = 1.0;
}

//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------


ImageSender::ImageSender(QTcpSocket* socket, QString imageFileDir, QObject* parent) :
    QObject(parent),
    mSocket(socket),
    mCounter(0),
    mImageFileDir(imageFileDir)
{
  mImageData = loadImage(mImageFileDir);
  mImageData = convertToTestColorImage(mImageData);

  mTimer = new QTimer(this);
  connect(mTimer, SIGNAL(timeout()), this, SLOT(tick())); // this signal will be executed in the thread of THIS, i.e. the main thread.
  mTimer->start(40);
}

void ImageSender::tick()
{
  std::cout << "tick" << std::endl;
  igtl::ImageMessage::Pointer imgMsg = getVtkImageMessage(mImageData);

  //------------------------------------------------------------
  // Pack (serialize) and send
  imgMsg->Pack();
  mSocket->write(reinterpret_cast<const char*>(imgMsg->GetPackPointer()), imgMsg->GetPackSize());
}


//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------


ImageSession::ImageSession(int socketDescriptor, QString imageFileDir, QObject* parent) :
    QThread(parent),
    mSocketDescriptor(socketDescriptor),
    mImageFileDir(imageFileDir)
{
}

ImageSession::~ImageSession()
{
}

void ImageSession::run()
{
  mSocket = new QTcpSocket();
  connect(mSocket, SIGNAL(disconnected()), this, SLOT(quit()), Qt::DirectConnection); // quit thread when disconnected
  mSocket->setSocketDescriptor(mSocketDescriptor);
  QString clientName = mSocket->localAddress().toString();
  std::cout << "Connected to " << clientName.toStdString() << ". Session started." << std::endl;
  ImageSender* sender = new ImageSender(mSocket, mImageFileDir);
  // socket should now be connected....?

  this->exec();

  std::cout << "Disconnected from " << clientName.toStdString() << ". Session ended." << std::endl;
  delete sender;
  delete mSocket;
}


} // cx
