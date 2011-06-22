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
#include "vtkMetaImageWriter.h"

typedef vtkSmartPointer<vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<vtkImageMapToColors> vtkImageMapToColorsPtr;
typedef vtkSmartPointer<vtkLookupTable> vtkLookupTablePtr;

#include "cxImageSenderFile.h"
#include "cxImageSenderOpenCV.h"

namespace cx
{




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
#ifdef USE_OpenCV
  ImageSenderOpenCV* sender = new ImageSenderOpenCV(mSocket, mImageFileDir);
#else
  ImageSender* sender = new ImageSender(mSocket, mImageFileDir);
#endif
  // socket should now be connected....?

  this->exec();

  std::cout << "Disconnected from " << clientName.toStdString() << ". Session ended." << std::endl;
  delete sender;
  delete mSocket;
}


} // cx
