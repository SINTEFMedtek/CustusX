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

namespace ssc
{

OpenIGTLinkRTSource::OpenIGTLinkRTSource() :
  mImageWidth( 0),
  mImageHeight( 0),
  mImageImport(vtkImageImportPtr::New())
{
  //mUSSession = -1;
  mUSTextBuf = NULL;

  mImageImport->SetNumberOfScalarComponents(1);
//  mImageImport->SetNumberOfScalarComponents(4);
  mImageImport->SetDataScalarTypeToUnsignedChar();

  mImageData = mImageImport->GetOutput();
}

OpenIGTLinkRTSource::~OpenIGTLinkRTSource()
{
  //disconnect();
}

QDateTime OpenIGTLinkRTSource::getTimestamp()
{
  return QDateTime();
}


bool OpenIGTLinkRTSource::connected() const
{
  //return (mUSSession != -1);
  return true;
}

//void OpenIGTLinkRTSource::reconnect()
//{
//  if (!connected())
//    return;
//  int temp = m_shmtKey;
//  disconnect();
//  connect(temp);
//}

///**connect to shared memory
// */
//void OpenIGTLinkRTSource::connect(int shmtKey)
//{
//  if (mUSSession != -1)
//  {
//    disconnect();
//  }
//
//  m_shmtKey = shmtKey;
//  int err = SHM_Connect(shmtKey, &mUSSession);
//
//  if (err != kNoError)
//  {
//    mUSSession = -1;
//    SW_LOG( "Failed connecting to shared buffer with key %d, status: %d", shmtKey, err );
//    return;
//  }
//  SW_LOG( "Connected to shared buffer with key %d", shmtKey );
//
//  update();
//}
//
///**disconnect from shared memory
// */
//void OpenIGTLinkRTSource::disconnect()
//{
//  if (mUSSession != -1)
//  {
//    SHM_Done(mUSSession);
//    mUSSession = -1;
//  }
//
//  initializeBuffer(0,0);
//}

void OpenIGTLinkRTSource::updateImage(igtl::ImageMessage::Pointer message)
{
  mImageMessage = message;
  // Retrive the image data
  int size[3]; // image dimension
  float spacing[3]; // spacing (mm/pixel)
  int svsize[3]; // sub-volume size
  int svoffset[3]; // sub-volume offset
  int scalarType; // scalar type

  scalarType = message->GetScalarType();
  message->GetDimensions(size);
  message->GetSpacing(spacing);
  message->GetSubVolume(svsize, svoffset);

//  snw_us::UltrasoundImage *us_image= NULL;

//  int32_t dummy_index;
//  us_image = ( snw_us::UltrasoundImage* ) SHM_GetReadBuffer(mUSSession, &dummy_index);
//  if (!us_image)
//  {
//    SW_LOG("US Image couldn't be loaded.");
//    return;
//  }

  mImageWidth = size[0];
  mImageHeight = size[1];

  mImageImport->SetWholeExtent(0, mImageWidth - 1, 0, mImageHeight - 1, 0, 0);
  mImageImport->SetDataExtentToWholeExtent();
  mImageImport->SetImportVoidPointer(mImageMessage->GetPackBodyPointer());


  //this->initializeBuffer(size[0], size[1]);

  //mUSTextBuf->

//  // This is where the image is decoded
//  if (snw_us::DecodeUltrasoundImage(mUSTextBuf, mImageWidth, mImageHeight, *us_image) != kNoError)
//  {
//    SW_LOG( "US Image couldn't be decoded." );
//    return;
//  }

//  std::stringstream ss;
//  ss << "connected(): " << connected() << std::endl;
//  ss << "mImageWidth: " << mImageWidth << std::endl;
//  ss << "mImageHeight: " << mImageHeight << std::endl;
//  ss << "us_image->width: " << us_image->width << std::endl;
//  ss << "us_image->height: " << us_image->height << std::endl;
//  Logger::state("vm.us.state", ss.str());
//
// //debug code: speckle the image with pink in order to see better
//  static int COUNT = 0;
//  for (int i=0; i<mImageWidth*mImageHeight; ++i)
//  {
//    if (i%4==0)
//    {
//      //printf("mUSTextBuf[i] = %d\n" , mUSTextBuf[i] );
//      mUSTextBuf[i] = 0xFFFF00FF;
//      mUSTextBuf[i]+= (10*COUNT++)*0xFF;
//    }
//    else
//      mUSTextBuf[i] = 0x00FF00FF;
//  }


//  Logger::log("vm.log", "us update "+ string_cast(mImageWidth) + ", " + string_cast(mImageHeight) );
  mImageImport->Modified();
}

/**Change size of buffers if changed (or new)
 * Connect to mImageImport.
 */
void OpenIGTLinkRTSource::initializeBuffer(int newWidth, int newHeight)
{
 // padBox(&newWidth, &newHeight); // remove to use input bb

  // already ok: ignore
  if (mImageWidth==newWidth && mImageHeight==newHeight)
  {
    return;
  }

  mImageWidth = newWidth;
  mImageHeight = newHeight;


  mImageImport->SetWholeExtent(0, mImageWidth - 1, 0, mImageHeight - 1, 0, 0);
  mImageImport->SetDataExtentToWholeExtent();
  int size = mImageWidth * mImageHeight;
  delete[] mUSTextBuf;
  mUSTextBuf = NULL;
  if (size>0)
  {
    mUSTextBuf = new uint32_t[size];
    memset(mUSTextBuf, 0, sizeof(int32_t) * size);
    mImageImport->SetImportVoidPointer(mUSTextBuf);
    //Logger::log("vm.log", "connected us buffer to vtkImageImport, size=("+string_cast(mImageWidth) + "," + string_cast(mImageHeight)+")");
  }
  else
  {
    mImageImport->SetImportVoidPointer(NULL);
    //Logger::log("vm.log", "disconnected us buffer from vtkImageImport");
  }
}

vtkImageDataPtr OpenIGTLinkRTSource::getVtkImageData()
{
  return mImageData;
}

/** increase input bounding box to a power of 2.
 */
void OpenIGTLinkRTSource::padBox(int* x, int* y) const
{
  if (*x==0 || *y==0)
    return;

  int x_org = *x;
  int y_org = *y;

  *x = 1;
  *y = 1;
  while (*x < x_org)
    (*x) *= 2;
  while (*y < y_org)
    (*y) *= 2;
}


}
