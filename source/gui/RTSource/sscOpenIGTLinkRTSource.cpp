/*
 * sscOpenIGTLinkRTSource.cpp
 *
 *  Created on: Oct 31, 2010
 *      Author: christiana
 */
#include "sscOpenIGTLinkRTSource.h"

#include <math.h>
#include <vtkImageData.h>
#include <vtkImageImport.h>
#include <vtkDataSetMapper.h>
#include <vtkTimerLog.h>
#include <QTimer>
#include "sscOpenIGTLinkClient.h"

namespace ssc
{

OpenIGTLinkRTSource::OpenIGTLinkRTSource() :
  mImageImport(vtkImageImportPtr::New())
{
  mImageImport->SetNumberOfScalarComponents(1);
  this->setEmptyImage();

  mTimeout = false;
  mTimeoutTimer = new QTimer(this);
  mTimeoutTimer->setInterval(1000);
  connect( mTimeoutTimer, SIGNAL(timeout()),this, SLOT(timeout()) );
}

OpenIGTLinkRTSource::~OpenIGTLinkRTSource()
{
  //disconnect();
  if (mClient)
  {
    mClient->terminate();
    mClient->wait(); // forever or until dead thread
  }
}

void OpenIGTLinkRTSource::timeout()
{
  std::cout << "timeout!" << std::endl;
  mTimeout = true;
  emit changed();
}


QString OpenIGTLinkRTSource::getInfoString() const
{
  if (!mClient)
    return "";
  return mClient->hostDescription();
}

QString OpenIGTLinkRTSource::getStatusString() const
{
  if (!mClient)
    return "Not connected";
  if (mTimeout)
    return "Timeout";
  return "Running";
}

void OpenIGTLinkRTSource::start()
{

}

void OpenIGTLinkRTSource::pause()
{

}

void OpenIGTLinkRTSource::stop()
{

}

bool OpenIGTLinkRTSource::validData() const
{
  return mClient && !mTimeout;
}

QDateTime OpenIGTLinkRTSource::getTimestamp()
{
  //TODO: get ts from messge
  return QDateTime();
}

bool OpenIGTLinkRTSource::connected() const
{
  return mClient;
}

void OpenIGTLinkRTSource::connectServer(QString address, int port)
{
  if (mClient)
    return;
  std::cout << "OpenIGTLinkRTSource::connect to server" << std::endl;
  mClient.reset(new IGTLinkClient(address, port, this));
  connect(mClient.get(), SIGNAL(finished()), this, SLOT(clientFinishedSlot()));
  connect(mClient.get(), SIGNAL(imageReceived()), this, SLOT(imageReceivedSlot())); // thread-bridging connection

  mClient->start();
  mTimeoutTimer->start();

  emit changed();
  emit serverStatusChanged();
}

void OpenIGTLinkRTSource::imageReceivedSlot()
{
  if (!mClient)
    return;
  this->updateImage(mClient->getLastImageMessage());
}

void OpenIGTLinkRTSource::disconnectServer()
{
  std::cout << "IGTLinkWidget::disconnect server" << std::endl;
  if (mClient)
  {
    mClient->stop();
    mClient->quit();
    mClient->wait(2000); // forever or until dead thread

    disconnect(mClient.get(), SIGNAL(finished()), this, SLOT(clientFinishedSlot()));
    disconnect(mClient.get(), SIGNAL(imageReceived()), this, SLOT(imageReceivedSlot())); // thread-bridging connection
    mClient.reset();
  }

  mTimeoutTimer->stop();

  emit changed();
  emit serverStatusChanged();
}

void OpenIGTLinkRTSource::clientFinishedSlot()
{
  if (!mClient)
    return;
  if (mClient->isRunning())
    return;
  this->disconnectServer();
}

/** chrash-avoiding measure -  for startup
 */
void OpenIGTLinkRTSource::setEmptyImage()
{
  mImageMessage = igtl::ImageMessage::Pointer();
  mImageImport->SetWholeExtent(0, 0, 0, 0, 0, 0);
  mImageImport->SetDataExtent(0,0,0,0,0,0);
  mImageImport->SetDataScalarTypeToUnsignedChar();
  mZero = 0;
  mImageImport->SetImportVoidPointer(&mZero);
  mImageImport->Modified();
}

void OpenIGTLinkRTSource::updateImage(igtl::ImageMessage::Pointer message)
{
  if (!message)
    this->setEmptyImage();

  mImageMessage = message;
  // Retrive the image data
  int size[3]; // image dimension
  float spacing[3]; // spacing (mm/pixel)
  int svsize[3]; // sub-volume size
  int svoffset[3]; // sub-volume offset
  int scalarType; // scalar type

  // Note: subvolumes is not supported. Implement when needed.

  scalarType = message->GetScalarType();
  message->GetDimensions(size);
  message->GetSpacing(spacing);
  message->GetSubVolume(svsize, svoffset);

  switch (scalarType)
  {
  case igtl::ImageMessage::TYPE_INT8:
    std::cout << "signed char is not supported. Falling back to unsigned char." << std::endl;
    mImageImport->SetDataScalarTypeToUnsignedChar();
    break;
  case igtl::ImageMessage::TYPE_UINT8:
    mImageImport->SetDataScalarTypeToUnsignedChar();
    break;
  case igtl::ImageMessage::TYPE_INT16:
    mImageImport->SetDataScalarTypeToShort();
    break;
  case igtl::ImageMessage::TYPE_UINT16:
    mImageImport->SetDataScalarTypeToUnsignedShort();
    break;
  case igtl::ImageMessage::TYPE_INT32:
    mImageImport->SetDataScalarTypeToInt();
    break;
  case igtl::ImageMessage::TYPE_UINT32:
    std::cout << "unsigned int is not supported. Falling back to int." << std::endl;
    mImageImport->SetDataScalarTypeToInt();
    break;
  case igtl::ImageMessage::TYPE_FLOAT32:
    mImageImport->SetDataScalarTypeToFloat();
    break;
  case igtl::ImageMessage::TYPE_FLOAT64:
    mImageImport->SetDataScalarTypeToDouble();
    break;
  default:
    std::cout << "unknown type. Falling back to unsigned char." << std::endl;
    mImageImport->SetDataScalarTypeToUnsignedChar();
  }

  mImageImport->SetDataOrigin(0,0,0);
  mImageImport->SetDataSpacing(spacing[0], spacing[1], spacing[2]);

  mImageImport->SetWholeExtent(0, size[0] - 1, 0, size[1] - 1, 0, size[2]-1);
  mImageImport->SetDataExtentToWholeExtent();
  mImageImport->SetImportVoidPointer(mImageMessage->GetPackBodyPointer());

  mImageImport->Modified();

  mTimeout = false;
  mTimeoutTimer->start();

  emit changed();
}

vtkImageDataPtr OpenIGTLinkRTSource::getVtkImageData()
{
  return mImageImport->GetOutput();
//  return mImageData;
}

///** increase input bounding box to a power of 2.
// */
//void OpenIGTLinkRTSource::padBox(int* x, int* y) const
//{
//  if (*x==0 || *y==0)
//    return;
//
//  int x_org = *x;
//  int y_org = *y;
//
//  *x = 1;
//  *y = 1;
//  while (*x < x_org)
//    (*x) *= 2;
//  while (*y < y_org)
//    (*y) *= 2;
//}


}
