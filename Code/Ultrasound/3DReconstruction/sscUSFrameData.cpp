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
#include <vtkImageLuminance.h>
#include <vtkImageClip.h>
#include <vtkImageAppend.h>
#include <vtkMetaImageWriter.h>
#include "sscTypeConversions.h"

typedef vtkSmartPointer<vtkImageAppend> vtkImageAppendPtr;

namespace ssc
{
USFrameData::USFrameData() :
	mUseAngio(false), mCropbox(0,0,0,0,0,0), mDirty(true)
{
//	mBaseImage = inputFrameData;

	//this->reinitialize();
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
//
//QString USFrameData::getName()
//{
//	return mBaseImage->getName();
//}
//
//QString USFrameData::getUid()
//{
//	return mBaseImage->getUid();
//}
//
//QString USFrameData::getFilePath()
//{
//	return mBaseImage->getFilePath();
//}

//ImagePtr USFrameData::getBase()
//{
//	return mBaseImage;
//}
void USFrameData::setAngio(bool angio)
{
	if (angio==mUseAngio)
		mDirty = true;

	mUseAngio = angio;
}

void USFrameData::setCropBox(IntBoundingBox3D cropbox)
{
	// ensure clip never happens in z dir.
	cropbox[4] = -100000;
	cropbox[5] =  100000;

	if (cropbox==mCropbox)
		mDirty = true;

	mCropbox = cropbox;
}

/** Return an image that is cropped using its own croppingBox.
 *  The image is not added to the data manager nor saved.
 */
vtkImageDataPtr USFrameData::cropImage(vtkImageDataPtr input, IntBoundingBox3D cropbox)
{
  vtkImageClipPtr clip = vtkImageClipPtr::New();
  clip->SetInput(input);
  clip->SetOutputWholeExtent(cropbox.begin());
  clip->ClipDataOn();
  vtkImageDataPtr rawResult = clip->GetOutput();

  rawResult->Update();
  rawResult->UpdateInformation();
  rawResult->ComputeBounds();
  return rawResult;
}

vtkImageDataPtr USFrameData::toGrayscale(vtkImageDataPtr input)
{
	vtkSmartPointer<vtkImageLuminance> luminance = vtkSmartPointer<vtkImageLuminance>::New();
	luminance->SetInput(input);
	vtkImageDataPtr outData = luminance->GetOutput();
	outData->Update();
	return outData;
}

vtkImageDataPtr USFrameData::useAngio(vtkImageDataPtr inData)
{
	// Some of the code here is borrowed from the vtk examples:
	// http://public.kitware.com/cgi-bin/viewcvs.cgi/*checkout*/Examples/Build/vtkMy/Imaging/vtkImageFoo.cxx?root=VTK&content-type=text/plain

//	vtkImageDataPtr inData = inputFrameData->getBaseVtkImageData();

	if (inData->GetNumberOfScalarComponents() < 3)
	{
		ssc::messageManager()->sendWarning("Angio requested for grayscale ultrasound");
		return this->toGrayscale(inData);
	}

//	vtkSmartPointer<vtkImageLuminance> luminance = vtkSmartPointer<vtkImageLuminance>::New();
//	luminance->SetInput(inputFrameData->getBaseVtkImageData());
//	vtkImageDataPtr outData = luminance->GetOutput();
//	outData->Update();
	vtkImageDataPtr outData = this->toGrayscale(inData);

	int* outExt = outData->GetExtent();

	unsigned char *inPtr = static_cast<unsigned char*> (inData->GetScalarPointer());
	unsigned char *outPtr = static_cast<unsigned char*> (outData->GetScalarPointer());

	int maxX = outExt[1] - outExt[0];
	int maxY = outExt[3] - outExt[2];
	int maxZ = outExt[5] - outExt[4];

	// Get increments to march through data
	vtkIdType inIncX, inIncY, inIncZ;
	vtkIdType outIncX, outIncY, outIncZ;
	//The following may give some values if in and out have different extent???
	inData->GetContinuousIncrements(outExt, inIncX, inIncY, inIncZ); //Don't work?
	outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ); //Don't work?
	std::cout << "outExt: " << outExt[0] << " " << outExt[1] << " " << outExt[2] << " " << outExt[3] << " "
		<< outExt[4] << " " << outExt[5] << endl;

	// Loop through output pixels
	int idxZ, idxY, idxR;

	for (idxZ = 0; idxZ <= maxZ; idxZ++)
	{
		for (idxY = 0; /*!self->AbortExecute &&*/idxY <= maxY; idxY++)
		{
			for (idxR = 0; idxR < maxX; idxR++)//Look at 3 scalar components at the same time (RGB)
			{
				// Pixel operation. Add foo. Dumber would be impossible.
				//        *outPtr = (OT)((float)(*inPtr) + foo);
				if (((*inPtr) == (*(inPtr + 1))) && ((*inPtr) == (*(inPtr + 2))))
				{
					(*outPtr) = 0;
					(*(outPtr + 1)) = 0;
					(*(outPtr + 2)) = 0;
				}
				else
				{
				}//Assume the outVolume is treated with the luminance filter first
				outPtr++;
				inPtr += 3;
			}
		}
	}
	return outData;
}

/**write us images to disk.
 *
 * The images are handled as an array of 2D frames, but written into
 * one 3D image mhd file. Due to memory limitations (one large mem block
 * causes bit trouble), this is done by writing a single frame, and then
 * appending the other frames manually, and then hacking the mhd file to
 * incorporate the correct dimensions.
 *
 */
bool USFrameData::save(QString filename, bool compressed)
{
	ssc::messageManager()->sendInfo(QString("USFrameData prepare write frames"));
	vtkImageDataPtr image = this->getSingleBaseImage();

//	ssc::messageManager()->sendInfo(QString("USFrameData start write %1 frames").arg(image->GetDimensions()[2]));

	vtkMetaImageWriterPtr writer = vtkMetaImageWriterPtr::New();
	writer->SetInput(image);
	writer->SetFileName(cstring_cast(filename));
	writer->SetCompression(compressed);
	writer->Write();

	ssc::messageManager()->sendInfo(QString("USFrameData completed write of %1 frames").arg(image->GetDimensions()[2]));
	return true;;
}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


USFrameDataMonolithic::USFrameDataMonolithic(ImagePtr inputFrameData) :
	USFrameData()
{
	mBaseImage = inputFrameData;
	this->reinitialize();
}

/** reset the internal state of the oobject to that of the initialization,
 * i.e. no removed frames.
 */
void USFrameDataMonolithic::reinitialize()
{
	if (!mDirty)
		return;
	mDirty = false;

	vtkImageDataPtr current = mBaseImage->getBaseVtkImageData();

	if (mCropbox.range()[0]!=0)
		current = this->cropImage(current, mCropbox);

	if (mUseAngio)
		current = this->useAngio(current);
	else
		current = this->toGrayscale(current); // remove color, if any
	mProcessedImage = current;

	mDimensions = mProcessedImage->GetDimensions();
	mSpacing = Vector3D(mProcessedImage->GetSpacing());

	// Raw data pointer
	unsigned char *inputPointer = static_cast<unsigned char*> (mProcessedImage->GetScalarPointer());

	//Create one pointer to each frame
	mFrames.resize(mDimensions[2]);
	unsigned int recordSize = mDimensions[0] * mDimensions[1];
	for (int record = 0; record < mDimensions[2]; record++)
		mFrames[record] = inputPointer + record * recordSize;
}

vtkImageDataPtr USFrameDataMonolithic::getSingleBaseImage()
{
	return mBaseImage->getBaseVtkImageData();
}

QString USFrameDataMonolithic::getName()
{
	return mBaseImage->getName();
}

QString USFrameDataMonolithic::getUid()
{
	return mBaseImage->getUid();
}

QString USFrameDataMonolithic::getFilePath()
{
	return mBaseImage->getFilePath();
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


USFrameDataSplitFrames::USFrameDataSplitFrames(std::vector<vtkImageDataPtr> inputFrameData, QString filename) :
	USFrameData()
{
	mFilename = filename;
	mBaseImage = inputFrameData;
//	this->reinitialize();
}

/** Merge all us frames into one vtkImageData
 *
 */
vtkImageDataPtr USFrameDataSplitFrames::mergeFrames(std::vector<vtkImageDataPtr> input)
{
  vtkImageAppendPtr filter = vtkImageAppendPtr::New();
  filter->SetAppendAxis(2); // append along z-axis

  for (unsigned i=0; i<input.size(); ++i)
    filter->SetInput(i, input[i]);

  filter->Update();
  return filter->GetOutput();
}

//void USFrameDataSplitFrames::crop()
//{
//	vtkImageDataPtr out = cropImage(in, mCropBox);
//}

vtkImageDataPtr USFrameDataSplitFrames::getSingleBaseImage()
{
	return this->mergeFrames(mBaseImage);
}


/** reset the internal state of the oobject to that of the initialization,
 * i.e. no removed frames.
 */
void USFrameDataSplitFrames::reinitialize()
{
	if (!mDirty)
		return;
	mDirty = false;

	mProcessedImage.clear();

	mProcessedImage.resize(mBaseImage.size());

	// apply cropping and angio
	for (unsigned i=0; i<mProcessedImage.size(); ++i)
	{
		vtkImageDataPtr current = mBaseImage[i];

		if (mCropbox.range()[0]!=0)
			current = this->cropImage(current, mCropbox);

		if (mUseAngio)
			current = this->useAngio(current);
		else
			current = this->toGrayscale(current);

		mProcessedImage[i] = current;
	}

	mDimensions = mProcessedImage[0]->GetDimensions();
	mDimensions[2] = mProcessedImage.size();

	mSpacing = Vector3D(mProcessedImage[0]->GetSpacing());
	mSpacing[2] = mSpacing[0]; // set z-spacing to arbitrary value.

	//Create one pointer to each frame
	mFrames.resize(mDimensions[2]);
	for (int record = 0; record < mDimensions[2]; record++)
	{
		// Raw data pointer
		unsigned char *inputPointer = static_cast<unsigned char*> (mProcessedImage[record]->GetScalarPointer());
		mFrames[record] = inputPointer;
	}
}

QString USFrameDataSplitFrames::getName()
{
	return mFilename;
}

QString USFrameDataSplitFrames::getUid()
{
	return mFilename;
}

QString USFrameDataSplitFrames::getFilePath()
{
	return mFilename;
}



}//namespace ssc
