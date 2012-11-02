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
#include <vtkImageImport.h>
#include "sscTypeConversions.h"

typedef vtkSmartPointer<vtkImageAppend> vtkImageAppendPtr;

namespace ssc
{

USFrameDataPtr USFrameData::create(ImagePtr inputFrameData)
{
	USFrameDataPtr retval(new USFrameData());

	retval->mFilename = inputFrameData->getName();

	vtkImageDataPtr input = inputFrameData->getBaseVtkImageData();
	retval->mOptionalWholeBase = input;
	retval->mBaseImage.resize(input->GetDimensions()[2]);

	for (int i=0; i<retval->mBaseImage.size(); ++i)
	{
		vtkImageImportPtr import = vtkImageImportPtr::New();

		import->SetImportVoidPointer(input->GetScalarPointer(0,0,i));
		import->SetDataScalarType(input->GetScalarType());
		import->SetDataSpacing(input->GetSpacing());
		import->SetNumberOfScalarComponents(input->GetNumberOfScalarComponents());
		int* extent = input->GetWholeExtent();
		extent[4] = 0;
		extent[5] = 0;
		import->SetWholeExtent(extent);
		import->SetDataExtentToWholeExtent();

		import->Update();
		retval->mBaseImage[i] = import->GetOutput();
	}

	retval->initialize();

	return retval;
}

USFrameDataPtr USFrameData::create(std::vector<vtkImageDataPtr> inputFrameData, QString filename)
{
	USFrameDataPtr retval(new USFrameData());
	retval->mFilename = filename;
	retval->mBaseImage = inputFrameData;

	retval->initialize();

	return retval;
}

USFrameData::USFrameData() :
		mUseAngio(false), mCropbox(0,0,0,0,0,0)
{
}

void USFrameData::initialize()
{
	if (mBaseImage.empty())
		return;

	mReducedToFull.clear();
	for (unsigned i=0; i<mBaseImage.size(); ++i)
		mReducedToFull.push_back(i);

//	mDimensions = mBaseImage[0]->GetDimensions();
//	mDimensions[2] = mBaseImage.size();
//
//	mSpacing = Vector3D(mBaseImage[0]->GetSpacing());
//	mSpacing[2] = mSpacing[0]; // set z-spacing to arbitrary value.
}

/**
 * Dimensions will be changed after this
 */
void USFrameData::removeFrame(unsigned int index)
{
	if (index>=mReducedToFull.size())
	{
		ssc::messageManager()->sendError("index error");
		return;
	}
	mReducedToFull.erase(mReducedToFull.begin()+index);

//	std::cout << "USFrameData::removeFrame " << index << ", ptr=" << this << std::endl;
//	mDimensions[2]--;

	this->clearCache();
}

Eigen::Array3i USFrameData::getDimensions()
{
	Eigen::Array3i retval(mBaseImage[0]->GetDimensions());
	if (mCropbox[1]!=0)
	{
		retval[0] = mCropbox.range()[0];
		retval[1] = mCropbox.range()[1];
	}
	retval[2] = mReducedToFull.size();
	return retval;
////	return mDimensions;
//	mDimensions = mBaseImage[0]->GetDimensions();
//	mDimensions[2] = mBaseImage.size();
}

Vector3D USFrameData::getSpacing()
{
	if (mBaseImage.empty())
		return Vector3D(1,1,1);
	ssc::Vector3D retval = Vector3D(mBaseImage[0]->GetSpacing());
	retval[2] = retval[0]; // set z-spacing to arbitrary value.
	return retval;
}

void USFrameData::setAngio(bool angio)
{
	if (angio!=mUseAngio)
		this->clearCache();

	mUseAngio = angio;
}

void USFrameData::setCropBox(IntBoundingBox3D cropbox)
{
	// ensure clip never happens in z dir.
	cropbox[4] = -100000;
	cropbox[5] =  100000;

//	std::cout << "USFrameData::setCropBox " << cropbox << std::endl;

	if (cropbox!=mCropbox)
		this->clearCache();

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
	if (input->GetNumberOfScalarComponents() == 1) // already gray
		return input;

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
//	std::cout << "outExt: " << outExt[0] << " " << outExt[1] << " " << outExt[2] << " " << outExt[3] << " "
//		<< outExt[4] << " " << outExt[5] << endl;

	// Loop through output pixels
	int idxZ, idxY, idxR;

//	QDateTime start = QDateTime::currentDateTime();

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
//					(*(outPtr + 1)) = 0;
//					(*(outPtr + 2)) = 0;
				}
//				else
//				{
//				}//Assume the outVolume is treated with the luminance filter first
				outPtr++;
				inPtr += 3;
			}
		}
	}
//	std::cout << "loop: " << start.msecsTo(QDateTime::currentDateTime()) << "ms" << std::endl;
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
 * Update: Because we want to compress data, the standard vtk filter is used.
 * This is not a big problem anymore because this is done in a working thread.
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

unsigned char* USFrameData::getFrame(unsigned int index)
{
	this->generateCache();

	if (mProcessedImage.size()<=index)
		ssc::messageManager()->sendError("index error");
	// Raw data pointer
	unsigned char *inputPointer = static_cast<unsigned char*> (mProcessedImage[index]->GetScalarPointer());
	return inputPointer;
}

void USFrameData::generateCache()
{
	if (!mProcessedImage.empty())
		return;

	mProcessedImage.clear();

	mProcessedImage.resize(mReducedToFull.size());

	// apply cropping and angio
	for (unsigned i=0; i<mReducedToFull.size(); ++i)
	{
		vtkImageDataPtr current = mBaseImage[mReducedToFull[i] ];

		if (mCropbox.range()[0]!=0)
			current = this->cropImage(current, mCropbox);

		if (mUseAngio)
			current = this->useAngio(current);
		else
			current = this->toGrayscale(current);

		mProcessedImage[i] = current;
	}
//	std::cout << "USFrameData::generateCache " << std::endl;

}

/** Merge all us frames into one vtkImageData
 *
 */
vtkImageDataPtr USFrameData::mergeFrames(std::vector<vtkImageDataPtr> input)
{
  vtkImageAppendPtr filter = vtkImageAppendPtr::New();
  filter->SetAppendAxis(2); // append along z-axis

  for (unsigned i=0; i<input.size(); ++i)
    filter->SetInput(i, input[i]);

  filter->Update();
  return filter->GetOutput();
}

void USFrameData::clearCache()
{
	mProcessedImage.clear();
}

USFrameDataPtr USFrameData::copy()
{
	USFrameDataPtr retval(new USFrameData(*this));
	retval->clearCache();
	this->initialize();
	return retval;
}

//void USFrameDataSplitFrames::crop()
//{
//	vtkImageDataPtr out = cropImage(in, mCropBox);
//}

vtkImageDataPtr USFrameData::getSingleBaseImage()
{
	if (mOptionalWholeBase)
		return mOptionalWholeBase;
	return this->mergeFrames(mBaseImage);
}

QString USFrameData::getName()
{
	return mFilename;
}

QString USFrameData::getUid()
{
	return mFilename;
}

QString USFrameData::getFilePath()
{
	return mFilename;
}

void USFrameData::fillImageImport(vtkImageImportPtr import, int index)
{
	import->SetImportVoidPointer(mBaseImage[index]->GetScalarPointer());
	import->SetDataScalarType(mBaseImage[index]->GetScalarType());
	import->SetDataSpacing(mBaseImage[index]->GetSpacing());
	import->SetNumberOfScalarComponents(mBaseImage[index]->GetNumberOfScalarComponents());
	import->SetWholeExtent(mBaseImage[index]->GetWholeExtent());
	import->SetDataExtentToWholeExtent();
}




}//namespace ssc
