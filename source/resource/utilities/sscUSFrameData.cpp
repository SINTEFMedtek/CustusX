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
#include "sscDataManagerImpl.h"
#include <QFileInfo>
#include "sscTimeKeeper.h"
#include "cxImageDataContainer.h"

#include "sscLogger.h"

typedef vtkSmartPointer<vtkImageAppend> vtkImageAppendPtr;

namespace ssc
{

USFrameDataPtr USFrameData::create(ImagePtr inputFrameData)
{
	USFrameDataPtr retval(new USFrameData());

	retval->mFilename = inputFrameData->getName();
	retval->mImageContainer.reset(new cx::SplitFramesContainer(inputFrameData->getBaseVtkImageData()));

//	vtkImageDataPtr input = inputFrameData->getBaseVtkImageData();
//	retval->mOptionalWholeBase = input;
//	retval->mBaseImage.resize(input->GetDimensions()[2]);

//	for (int i=0; i<retval->mBaseImage.size(); ++i)
//	{
//		vtkImageImportPtr import = vtkImageImportPtr::New();

//		import->SetImportVoidPointer(input->GetScalarPointer(0,0,i));
//		import->SetDataScalarType(input->GetScalarType());
//		import->SetDataSpacing(input->GetSpacing());
//		import->SetNumberOfScalarComponents(input->GetNumberOfScalarComponents());
//		int* extent = input->GetWholeExtent();
//		extent[4] = 0;
//		extent[5] = 0;
//		import->SetWholeExtent(extent);
//		import->SetDataExtentToWholeExtent();

//		import->Update();
//		retval->mBaseImage[i] = import->GetOutput();
//	}

	retval->initialize();

	return retval;
}

//USFrameDataPtr USFrameData::create(std::vector<vtkImageDataPtr> inputFrameData, QString filename)
//{
//	USFrameDataPtr retval(new USFrameData());
//	retval->mFilename = filename;
//	retval->mBaseImage = inputFrameData;

//	retval->initialize();

//	return retval;
//}

/** Create object from file.
  * If file or file+.mhd exists, use this,
  * Otherwise assume input is split over several
  * files and try to load all mhdFile + i + ".mhd".
  * forall i.
  */
USFrameDataPtr USFrameData::create(QString inputFilename)
{
	std::cout << "USFrameData::create() " << inputFilename << std::endl;
	QFileInfo info(inputFilename);

	ssc::TimeKeeper timer;
	QString mhdSingleFile = info.absolutePath()+"/"+info.completeBaseName()+".mhd";

	if (QFileInfo(mhdSingleFile).exists())
	{
//		std::cout << "USFrameData::create() single: " << mhdSingleFile << std::endl;
		vtkImageDataPtr image = ssc::MetaImageReader().load(mhdSingleFile);
		// load from single file
		USFrameDataPtr retval = USFrameData::create(ImagePtr(new Image(mhdSingleFile, image)));
		retval->mFilename = mhdSingleFile;
		timer.printElapsedms(QString("Loading single %1").arg(inputFilename));
		return retval;
	}
	else
	{
		USFrameDataPtr retval(new USFrameData());
		retval->mFilename = inputFilename;
		retval->mImageContainer.reset(new cx::CachedImageDataContainer(inputFilename, -1));
		retval->initialize();
		return retval;

//		(inputFilename);
//		std::vector<vtkImageDataPtr> frames;
//		for (unsigned i=0; true; ++i)
//		{
//			QString mhdFile = info.absolutePath()+"/"+info.completeBaseName()+QString("_%1.mhd").arg(i);
////			std::cout << "USFrameData::create() multi: " << mhdFile << std::endl;
//			if (!QFileInfo(mhdFile).exists())
//				break;
//			vtkImageDataPtr frame = ssc::MetaImageReader().load(mhdFile);
//			frames.push_back(frame);
//		}

//		timer.printElapsedms(QString("Loading multi %1").arg(inputFilename));
//		return USFrameData::create(frames, inputFilename);
	}


//    retval = ssc::USFrameData::create(info.path()+info.completeBaseName());
}


USFrameData::USFrameData() :
		mUseAngio(false), mCropbox(0,0,0,0,0,0)
{
}

void USFrameData::initialize()
{
	if (mImageContainer->empty())
		return;

	mReducedToFull.clear();
	for (unsigned i=0; i<mImageContainer->size(); ++i)
		mReducedToFull.push_back(i);
}

USFrameData::~USFrameData()
{
	mProcessedImage.clear();
	mImageContainer.reset();
//	mOptionalWholeBase = vtkImageDataPtr();
}


/**
 * Dimensions will be changed after this
 */
void USFrameData::removeFrame(unsigned int index)
{
	SSC_ASSERT(index < mReducedToFull.size());
	mReducedToFull.erase(mReducedToFull.begin()+index);

	this->clearCache();
}

Eigen::Array3i USFrameData::getDimensions() const
{
	Eigen::Array3i retval(mImageContainer->get(0)->GetDimensions());

	if (mCropbox.range()[0]!=0)
	{
		// no cache, generate one sample
		if (mProcessedImage.empty())
		{
			vtkImageDataPtr sample = this->cropImage(mImageContainer->get(0), mCropbox);
			retval[0] = sample->GetDimensions()[0];
			retval[1] = sample->GetDimensions()[1];
		}
		// cache is available, use that
		else
		{
			retval[0] = mProcessedImage[0]->GetDimensions()[0];
			retval[1] = mProcessedImage[0]->GetDimensions()[1];
		}
	}

	retval[2] = mReducedToFull.size();
	return retval;
}


Vector3D USFrameData::getSpacing() const
{
	if (mImageContainer->empty())
		return Vector3D(1,1,1);
	ssc::Vector3D retval = Vector3D(mImageContainer->get(0)->GetSpacing());
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
vtkImageDataPtr USFrameData::cropImage(vtkImageDataPtr input, IntBoundingBox3D cropbox) const
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

vtkImageDataPtr USFrameData::toGrayscale(vtkImageDataPtr input) const
{
	if (input->GetNumberOfScalarComponents() == 1) // already gray
		return input;

	vtkSmartPointer<vtkImageLuminance> luminance = vtkSmartPointer<vtkImageLuminance>::New();
	luminance->SetInput(input);
	vtkImageDataPtr outData = luminance->GetOutput();
	outData->Update();
	return outData;
}

vtkImageDataPtr USFrameData::useAngio(vtkImageDataPtr inData) const
{
	// Some of the code here is borrowed from the vtk examples:
	// http://public.kitware.com/cgi-bin/viewcvs.cgi/*checkout*/Examples/Build/vtkMy/Imaging/vtkImageFoo.cxx?root=VTK&content-type=text/plain

	if (inData->GetNumberOfScalarComponents() < 3)
	{
		ssc::messageManager()->sendWarning("Angio requested for grayscale ultrasound");
		return this->toGrayscale(inData);
	}

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
				}
				//Assume the outVolume is treated with the luminance filter first
				outPtr++;
				inPtr += 3;
			}
		}
	}

//	outData = AlgorithmHelpers::execute_itk_GrayscaleFillholeImageFilter(outData);

	return outData;
}

///**write us images to disk.
// *
// * The images are handled as an array of 2D frames, but written into
// * one 3D image mhd file. Due to memory limitations (one large mem block
// * causes bit trouble), this is done by writing a single frame, and then
// * appending the other frames manually, and then hacking the mhd file to
// * incorporate the correct dimensions.
// *
// * Update: Because we want to compress data, the standard vtk filter is used.
// * This is not a big problem anymore because this is done in a working thread.
// *
// */
//bool USFrameData::save(QString filename, bool compressed)
//{
//	ssc::messageManager()->sendInfo(QString("USFrameData prepare write frames"));
//	vtkImageDataPtr image = this->getSingleBaseImage();

////	ssc::messageManager()->sendInfo(QString("USFrameData start write %1 frames").arg(image->GetDimensions()[2]));

//	vtkMetaImageWriterPtr writer = vtkMetaImageWriterPtr::New();
//	writer->SetInput(image);
//	writer->SetFileName(cstring_cast(filename));
//	writer->SetCompression(compressed);
//	writer->Write();

//	ssc::messageManager()->sendInfo(QString("USFrameData completed write of %1 frames").arg(image->GetDimensions()[2]));
//	return true;;
//}

unsigned char* USFrameData::getFrame(unsigned int index) const
{
	if (mProcessedImage.empty())
	{
		ssc::messageManager()->sendError(QString("USFrameData %1 not properly initialized prior to calling getFrame()").arg(this->getName()));
		return NULL;
	}

	SSC_ASSERT(index < mProcessedImage.size());

	// Raw data pointer
	unsigned char *inputPointer = static_cast<unsigned char*> (mProcessedImage[index]->GetScalarPointer());
	return inputPointer;
}

/** Fill cache, enabling getFrames()
  * NOT thread-safe.
  *
  */
void USFrameData::generateCache()
{
	if (!mProcessedImage.empty())
		return;

	mProcessedImage.clear();

	mProcessedImage.resize(mReducedToFull.size());

	// apply cropping and angio
	for (unsigned i=0; i<mReducedToFull.size(); ++i)
	{
		SSC_ASSERT(mImageContainer->size()>mReducedToFull[i]);
		vtkImageDataPtr current = mImageContainer->get(mReducedToFull[i]);

		if (mCropbox.range()[0]!=0)
			current = this->cropImage(current, mCropbox);

		if (mUseAngio)
			current = this->useAngio(current);
		else
			current = this->toGrayscale(current);

		mProcessedImage[i] = current;
	}
}

void USFrameData::initializeFrames()
{
	this->generateCache();
}


///** Merge all us frames into one vtkImageData
// *
// */
//vtkImageDataPtr USFrameData::mergeFrames(std::vector<vtkImageDataPtr> input) const
//{
//  vtkImageAppendPtr filter = vtkImageAppendPtr::New();
//  filter->SetAppendAxis(2); // append along z-axis

//  for (unsigned i=0; i<input.size(); ++i)
//    filter->SetInput(i, input[i]);

//  filter->Update();
//  return filter->GetOutput();
//}

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

//vtkImageDataPtr USFrameData::getSingleBaseImage()
//{
//	if (mOptionalWholeBase)
//		return mOptionalWholeBase;
//	return this->mergeFrames(mBaseImage);
//}

QString USFrameData::getName() const
{
	return QFileInfo(mFilename).completeBaseName();
}

QString USFrameData::getUid() const
{
	return QFileInfo(mFilename).completeBaseName();
//	return mFilename;
}

QString USFrameData::getFilePath() const
{
	return mFilename;
}

void USFrameData::fillImageImport(vtkImageImportPtr import, int index)
{
	ssc::TimeKeeper timer;
	vtkImageDataPtr source = mImageContainer->get(index);

//	if (mProcessedImage.size() > index)
//	{
//		source = mProcessedImage[index];
//	}

//	std::cout << "USFrameData::fillImageImport" << source->GetNumberOfScalarComponents() << std::endl;

	import->SetImportVoidPointer(source->GetScalarPointer());
	import->SetDataScalarType(source->GetScalarType());
	import->SetDataSpacing(source->GetSpacing());
	import->SetNumberOfScalarComponents(source->GetNumberOfScalarComponents());
	import->SetWholeExtent(source->GetWholeExtent());
	import->SetDataExtentToWholeExtent();

	timer.printElapsedms(QString("import index %1").arg(index));
}




}//namespace ssc
