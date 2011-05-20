/*
 *  sscUSFrameData.cpp
 *  CustusX3
 *
 *  Created by Ole Vegard Solberg on 8/17/10.
 *  Copyright 2010 SINTEF. All rights reserved.
 *
 */

#include "sscUSFrameData.h"
#include "sscMessageManager.h"
#include <vtkImageData.h>

namespace ssc
{
USFrameData::USFrameData(ImagePtr inputFrameData, bool angio)
{
  mImage = inputFrameData;
  vtkImageDataPtr input;
  if (angio)
  {
    //input = mImage->getBaseVtkImageData();
    //TODO: Use only color information
    input = this->useAngio(inputFrameData);
  }
  else
  {
    input = mImage->getGrayScaleBaseVtkImageData(); // remove color, if any
  }
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

vtkImageDataPtr USFrameData::useAngio(ImagePtr inputFrameData)
{
  vtkImageDataPtr inData = inputFrameData->getBaseVtkImageData();
  vtkImageDataPtr outData = inputFrameData->getGrayScaleBaseVtkImageData();
  int numComp = inData->GetNumberOfScalarComponents();
  if (numComp < 3)
  {
    ssc::messageManager()->sendWarning("Angio requested for grayscale ultrasound");
    return outData;
  }
//  int scalarType = inData->GetScalarType();

  int* outExt = outData->GetExtent();

  unsigned char *inPtr = static_cast<unsigned char*>(inData->GetScalarPointerForExtent(outExt));
  unsigned char *outPtr = static_cast<unsigned char*>(outData->GetScalarPointerForExtent(outExt));

  int rowLength = (outExt[1] - outExt[0]+1)*inData->GetNumberOfScalarComponents();
  int maxY = outExt[3] - outExt[2];
  int maxZ = outExt[5] - outExt[4];
//  target = (unsigned long)((maxZ+1)*(maxY+1)/50.0);
//  target++;

  // Get increments to march through data
  vtkIdType inIncX, inIncY, inIncZ;
  vtkIdType outIncX, outIncY, outIncZ;
  inData->GetContinuousIncrements(outExt, inIncX, inIncY, inIncZ);
  outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);

  // Loop through ouput pixels

  for (int idxZ = 0; idxZ <= maxZ; idxZ++)
    {
    for (int idxY = 0; /*!self->AbortExecute &&*/ idxY <= maxY; idxY++)
      {
//      if (!id)
//        {
//        if (!(count%target))
//          {
//          self->UpdateProgress(count/(50.0*target));
//          }
//        count++;
//        }
      for (int idxR = 0; idxR < rowLength; idxR++)
        {
        // Pixel operation. Add foo. Dumber would be impossible.
//        *outPtr = (OT)((float)(*inPtr) + foo);
        if ((*inPtr) == (*(inPtr+1)) == (*(inPtr+2)))
          *outPtr = 0;
        else
        {}//Assume the outVolume is treated with the luminance filter first
        outPtr++;
        inPtr++;
        }
      outPtr += outIncY;
      inPtr += inIncY;
      }
    outPtr += outIncZ;
    inPtr += inIncZ;
    }

  return outData;
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

ImagePtr USFrameData::getBase()
{
  return mImage;
}

  
}//namespace ssc
