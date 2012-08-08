// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

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
USFrameData::USFrameData(ImagePtr inputFrameData, bool angio) :
	mUseAngio(angio)
{
	mImage = inputFrameData;

	this->reinitialize();
}

//mFileData.mUsRaw.reset(new ssc::USFrameData(mOriginalFileData.mUsRaw->getBase()));

/** reset the internal state of the oobject to that of the initialization,
 * i.e. no removed frames.
 */
void USFrameData::reinitialize()
{
	vtkImageDataPtr input;
	if (mUseAngio)
	{
		//input = mImage->getBaseVtkImageData();
		//TODO: Use only color information
		ssc::messageManager()->sendDebug("Extract angio data before reconstructing");
		input = this->useAngio(mImage);
	}
	else
	{
		input = mImage->getGrayScaleBaseVtkImageData(); // remove color, if any
	}
	mDimensions = input->GetDimensions();
	//  std::cout << "dims " << Eigen::Vector3i(mDimensions) << std::endl;
	mSpacing = Vector3D(input->GetSpacing());

	// Raw data pointer
	unsigned char *inputPointer = static_cast<unsigned char*> (input->GetScalarPointer());

	//Create one pointer to each frame
	mFrames.resize(mDimensions[2]);
	unsigned int recordSize = mDimensions[0] * mDimensions[1];
	for (int record = 0; record < mDimensions[2]; record++)
	{
		mFrames[record] = inputPointer + record * recordSize;
	}
}

vtkImageDataPtr USFrameData::useAngio(ImagePtr inputFrameData)
{
	// Some of the code here is borrowed from the vtk examples:
	// http://public.kitware.com/cgi-bin/viewcvs.cgi/*checkout*/Examples/Build/vtkMy/Imaging/vtkImageFoo.cxx?root=VTK&content-type=text/plain

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

	//  unsigned char *inPtr = static_cast<unsigned char*>(inData->GetScalarPointerForExtent(outExt));
	//  unsigned char *outPtr = static_cast<unsigned char*>(outData->GetScalarPointerForExtent(outExt));
	unsigned char *inPtr = static_cast<unsigned char*> (inData->GetScalarPointer());
	unsigned char *outPtr = static_cast<unsigned char*> (outData->GetScalarPointer());

	//  int rowLength = (outExt[1] - outExt[0]+1)*inData->GetNumberOfScalarComponents();
	int maxX = outExt[1] - outExt[0];
	int maxY = outExt[3] - outExt[2];
	int maxZ = outExt[5] - outExt[4];
	//  target = (unsigned long)((maxZ+1)*(maxY+1)/50.0);
	//  target++;

	// Get increments to march through data
	vtkIdType inIncX, inIncY, inIncZ;
	vtkIdType outIncX, outIncY, outIncZ;
	//The following may give some values if in and out have different extent???
	inData->GetContinuousIncrements(outExt, inIncX, inIncY, inIncZ); //Don't work?
	outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ); //Don't work?
	std::cout << "outExt: " << outExt[0] << " " << outExt[1] << " " << outExt[2] << " " << outExt[3] << " "
		<< outExt[4] << " " << outExt[5] << endl;
	//  std::cout << "outIncX: " << outIncX << " outIncY: " << outIncY << " outIncZ: " << outIncZ << std::endl;
	//  std::cout << "inIncX: " << inIncX << " inIncY: " << inIncY << " inIncZ: " << inIncZ << std::endl;

	// Loop through output pixels

	int idxZ, idxY, idxR;

	for (idxZ = 0; idxZ <= maxZ; idxZ++)
	{
		for (idxY = 0; /*!self->AbortExecute &&*/idxY <= maxY; idxY++)
		{
			//      if (!id)
			//        {
			//        if (!(count%target))
			//          {
			//          self->UpdateProgress(count/(50.0*target));
			//          }
			//        count++;
			//        }
			//      for (int idxR = 0; idxR < rowLength; idxR++)
			for (idxR = 0; idxR < maxX; idxR++)//Look at 3 scalar components at the same time (RGB)
			{
				// Pixel operation. Add foo. Dumber would be impossible.
				//        *outPtr = (OT)((float)(*inPtr) + foo);
				if (((*inPtr) == (*(inPtr + 1))) && ((*inPtr) == (*(inPtr + 2))))
				{
					//std::cout << "idxZ: " << idxZ << " idxY: " << idxY << " idxR: " << idxR << std::endl;
					(*outPtr) = 0;
					(*(outPtr + 1)) = 0;
					(*(outPtr + 2)) = 0;
				}
				else
				{
				}//Assume the outVolume is treated with the luminance filter first
				//        outPtr++;
				//        inPtr++;
				outPtr++;
				inPtr += 3;
			}
			//      outPtr += outIncY;
			//      inPtr += inIncY;
		}
		//    outPtr += outIncZ;
		//    inPtr += inIncZ;
	}

	return outData;
}

/**
 * Dimensions will be changed after this
 */
void USFrameData::removeFrame(unsigned int index)
{
	//	std::cout << "USFrameData::removeFrame " << index << std::endl;
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
void USFrameData::setAngio(bool angio)
{
	mUseAngio = angio;
}

}//namespace ssc
