/*
 *  sscUSFrameData.cpp
 *  CustusX3
 *
 *  Created by Ole Vegard Solberg on 8/17/10.
 *  Copyright 2010 SINTEF. All rights reserved.
 *
 */

#include "sscUSFrameData.h"
#include <vtkImageData.h>

namespace ssc
{
USFrameData::USFrameData(ImagePtr inputFrameData)
{
  mImage = inputFrameData;
  vtkImageDataPtr input = mImage->getBaseVtkImageData();
  mDimensions = input->GetDimensions();
  mSpacing = Vector3D(input->GetSpacing());
  
  // Raw data pointer
  unsigned char *inputPointer = static_cast<unsigned char*>( input->GetScalarPointer() );
  
  //Create one pointer to each frame
  mFrames.resize(mDimensions[2]);
  unsigned int recordSize = mDimensions[0]*mDimensions[1];
  for (int record = 0; record < mDimensions[2]; record++)
  {
    mFrames[record] = inputPointer + record*recordSize;
  }
  
}
  
/**
 * Dimensions will be changed after this
 */
void USFrameData::removeFrame(unsigned int index)
{
  mFrames.erase(mFrames.begin() + index);
  mDimensions[2]--;
}
  
unsigned char* USFrameData::getFrame(unsigned int index)
{
  return mFrames[index];
}
  
int* USFrameData::getDimensions()
{
  return mDimensions;
}

Vector3D USFrameData::getSpacing()
{
  return mSpacing;
}
  
QString USFrameData::getName()
{
  return mImage->getName();
}
  
QString USFrameData::getUid()
{
  return mImage->getUid();
}
  
QString USFrameData::getFilePath()
{
  return mImage->getFilePath();
}
  
}//namespace ssc