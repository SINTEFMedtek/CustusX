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

ProcessedUSInputData::ProcessedUSInputData(std::vector<vtkImageDataPtr> frames, std::vector<ssc::TimedPosition> pos, ssc::ImagePtr mask, QString path, QString uid) :
	mProcessedImage(frames),
	mFrames(pos),
	mMask(mask),
	mPath(path),
	mUid(uid)
{

}

unsigned char* ProcessedUSInputData::getFrame(unsigned int index) const
{
	SSC_ASSERT(index < mProcessedImage.size());

	// Raw data pointer
	unsigned char *inputPointer = static_cast<unsigned char*> (mProcessedImage[index]->GetScalarPointer());
	return inputPointer;
}

Eigen::Array3i ProcessedUSInputData::getDimensions() const
{
	Eigen::Array3i retval;
	retval[0] = mProcessedImage[0]->GetDimensions()[0];
	retval[1] = mProcessedImage[0]->GetDimensions()[1];
	retval[2] = mProcessedImage.size();
	return retval;
}

Vector3D ProcessedUSInputData::getSpacing() const
{
	Vector3D retval = Vector3D(mProcessedImage[0]->GetSpacing());
	retval[2] = retval[0]; // set z-spacing to arbitrary value.
	return retval;
}

std::vector<ssc::TimedPosition> ProcessedUSInputData::getFrames()
{
	return mFrames;
}

ssc::ImagePtr ProcessedUSInputData::getMask()
{
	return mMask;
}

QString ProcessedUSInputData::getFilePath()
{
	return mPath;
}

QString ProcessedUSInputData::getUid()
{
	return mUid;
}


///----------------------------------------------------------------------------
///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

USFrameDataPtr USFrameData::create(ImagePtr inputFrameData)
{
	USFrameDataPtr retval(new USFrameData());

	retval->mName = QFileInfo(inputFrameData->getName()).completeBaseName();
	retval->mImageContainer.reset(new cx::SplitFramesContainer(inputFrameData->getBaseVtkImageData()));
	retval->initialize();

	return retval;
}

/** Create object from file.
  * If file or file+.mhd exists, use this,
  * Otherwise assume input is split over several
  * files and try to load all mhdFile + i + ".mhd".
  * forall i.
  */
USFrameDataPtr USFrameData::create(QString inputFilename)
{
	QFileInfo info(inputFilename);

	ssc::TimeKeeper timer;
	QString mhdSingleFile = info.absolutePath()+"/"+info.completeBaseName()+".mhd";

	if (QFileInfo(mhdSingleFile).exists())
	{
		vtkImageDataPtr image = ssc::MetaImageReader().load(mhdSingleFile);
		// load from single file
		USFrameDataPtr retval = USFrameData::create(ImagePtr(new Image(mhdSingleFile, image)));
		retval->mName = QFileInfo(mhdSingleFile).completeBaseName();
		timer.printElapsedms(QString("Loading single %1").arg(inputFilename));
		return retval;
	}
	else
	{
		USFrameDataPtr retval(new USFrameData());
		retval->mName = QFileInfo(inputFilename).completeBaseName();
		retval->mImageContainer.reset(new cx::CachedImageDataContainer(inputFilename, -1));
		retval->initialize();
		return retval;
	}
}

USFrameDataPtr USFrameData::create(QString name, cx::CachedImageDataContainerPtr images)
{
	USFrameDataPtr retval(new USFrameData());
	retval->mName = name;
	retval->mImageContainer = images;
	retval->initialize();

	return retval;
}

USFrameData::USFrameData() :
		mCropbox(0,0,0,0,0,0), mPurgeInput(true)
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
	mImageContainer.reset();
}

/**
 * Dimensions will be changed after this
 */
void USFrameData::removeFrame(unsigned int index)
{
	SSC_ASSERT(index < mReducedToFull.size());
	mReducedToFull.erase(mReducedToFull.begin()+index);
}

Eigen::Array3i USFrameData::getDimensions() const
{
	Eigen::Array3i retval(mImageContainer->get(0)->GetDimensions());

	if (mCropbox.range()[0]!=0)
	{
		// WARNING: cannot use dimensions from the cropImage function - it uses extent,
		// which may be larger. We need the real size, given by wholeextent/updateextent.
		vtkImageDataPtr sample = this->cropImage(mImageContainer->get(0), mCropbox);
//		retval[0] = sample->GetDimensions()[0];
//		retval[1] = sample->GetDimensions()[1];
		Eigen::Array<int, 6, 1> extent(sample->GetWholeExtent());
//		std::cout << "wholeextent " << extent << std::endl;
//		std::cout << "dim " << retval << std::endl;
		retval[0] = extent[1]-extent[0]+1;
		retval[1] = extent[3]-extent[2]+1;
//		std::cout << "extent dim: " << retval[0] << " " << retval[1] << std::endl;
	}

	retval[2] = mReducedToFull.size();
	return retval;
}


Vector3D USFrameData::getSpacing() const
{
	Vector3D retval(1,1,1);
	if (!mImageContainer->empty())
		retval = Vector3D(mImageContainer->get(0)->GetSpacing());
	retval[2] = retval[0]; // set z-spacing to arbitrary value.
	return retval;
}

void USFrameData::setCropBox(IntBoundingBox3D cropbox)
{
	// ensure clip never happens in z dir.
	cropbox[4] = -100000;
	cropbox[5] =  100000;
	mCropbox = cropbox;
}

/** Crop the image by setting the wholeExtent.
 *
 * WARNING: This means that the dimension differ from the extent - this
 * is a speed optimization during preprocessing.
 * Using ClipDataOn would create a copy with dim==extent
 */
vtkImageDataPtr USFrameData::cropImage(vtkImageDataPtr input, IntBoundingBox3D cropbox) const
{
  vtkImageClipPtr clip = vtkImageClipPtr::New();
  clip->SetInput(input);
  clip->SetOutputWholeExtent(cropbox.begin());
//  clip->ClipDataOn(); // this line sets wholeextent==extent, but uses lots of time (~6ms/frame)
  vtkImageDataPtr rawResult = clip->GetOutput();
  rawResult->Update();
  return rawResult;
}

/**Convert input to grayscale, and return a COPY of that volume ( in order to break the pipeline for memory purposes)
 *
 */
vtkImageDataPtr USFrameData::toGrayscale(vtkImageDataPtr input) const
{
	if (input->GetNumberOfScalarComponents() == 1) // already gray
		return input;

	vtkSmartPointer<vtkImageLuminance> luminance = vtkSmartPointer<vtkImageLuminance>::New();
	luminance->SetInput(input);
	vtkImageDataPtr outData = luminance->GetOutput();
	outData->Update();

	vtkImageDataPtr copy = vtkImageDataPtr::New();
	copy->DeepCopy(outData);
	return copy;

}

// for testing
//void printStuff(QString text, vtkImageDataPtr data)
//{
//	std::cout << text << std::endl;
//	Eigen::Array<int, 6, 1> wholeExtent(data->GetWholeExtent());
//	Eigen::Array<int, 6, 1> extent(data->GetExtent());
//	Eigen::Array<int, 6, 1> updateExtent(data->GetUpdateExtent());
//	std::cout << "\t whole ext " <<  wholeExtent << std::endl;
//	std::cout << "\tupdate ext " <<  updateExtent << std::endl;
//	std::cout << "\t       ext " <<  extent << std::endl;
////	data->UpdateInformation();

//	Eigen::Array<vtkIdType,3,1> hinc;
//	data->GetContinuousIncrements(data->GetWholeExtent(), hinc[0], hinc[1], hinc[2]);
//	std::cout << "\t whole inc " <<  hinc << std::endl;

//	Eigen::Array<vtkIdType,3,1> uinc;
//	data->GetContinuousIncrements(data->GetUpdateExtent(), uinc[0], uinc[1], uinc[2]);
//	std::cout << "\t      uinc " <<  uinc << std::endl;

//	Eigen::Array<vtkIdType,3,1> inc;
//	data->GetContinuousIncrements(data->GetExtent(), inc[0], inc[1], inc[2]);
//	std::cout << "\t       inc " <<  inc << std::endl;

//	Eigen::Array3i dim(data->GetDimensions());
//	std::cout << "\t       dim " <<  dim << std::endl;

//	Eigen::Array3i max(wholeExtent[1] - wholeExtent[0], wholeExtent[3] - wholeExtent[2], wholeExtent[5] - wholeExtent[4]);
//	std::cout << "\t       max " <<  max << std::endl;
//}

vtkImageDataPtr USFrameData::useAngio(vtkImageDataPtr inData, vtkImageDataPtr grayFrame) const
{
	// Some of the code here is borrowed from the vtk examples:
	// http://public.kitware.com/cgi-bin/viewcvs.cgi/*checkout*/Examples/Build/vtkMy/Imaging/vtkImageFoo.cxx?root=VTK&content-type=text/plain

	if (inData->GetNumberOfScalarComponents() != 3)
	{
		ssc::messageManager()->sendWarning("Angio requested for grayscale ultrasound");
		return grayFrame;
	}

	vtkImageDataPtr outData = vtkImageDataPtr::New();
	outData->DeepCopy(grayFrame);
	outData->Update(); // updates whole extent.

//	printStuff("Clipped color in", inData);
//	printStuff("Grayscale in", outData);

	int* inExt = inData->GetWholeExtent();
	int* outExt = outData->GetWholeExtent();

	// Remember that the input might (and do due to vtkImageClip) contain leaps.
	// This means that the wholeextent might be larger than the extent, thus
	// we must use a startoffset and leaps between lines.

	unsigned char *inPtr = static_cast<unsigned char*> (inData->GetScalarPointerForExtent(inData->GetWholeExtent()));
	unsigned char *outPtr = static_cast<unsigned char*> (outData->GetScalarPointerForExtent(outData->GetWholeExtent()));

	int maxX = outExt[1] - outExt[0];
	int maxY = outExt[3] - outExt[2];
	int maxZ = outExt[5] - outExt[4];

	Eigen::Array<vtkIdType,3,1> inInc;
	inData->GetContinuousIncrements(inData->GetWholeExtent(), inInc[0], inInc[1], inInc[2]);
	SSC_ASSERT(inInc[0]==0);
	// we assume (wholeextent == extent) for the outData in the algorithm below. assert here.
	Eigen::Array<vtkIdType,3,1> outInc;
	outData->GetContinuousIncrements(outData->GetWholeExtent(), outInc[0], outInc[1], outInc[2]);
	SSC_ASSERT(outInc[0]==0);
	SSC_ASSERT(outInc[1]==0);
	SSC_ASSERT(outInc[2]==0);

	for (int z=0; z<=maxZ; z++)
	{
		for (int y=0; y<=maxY; y++)
		{
			for (int x=0; x<=maxX; x++)
			{
				//Look at 3 scalar components at the same time (RGB),
				//if color is grayscale or close to grayscale: set to zero.

				if (((*inPtr) == (*(inPtr + 1))) && ((*inPtr) == (*(inPtr + 2))))
				{
					(*outPtr) = 0;
				}
				else
				{
					// strong check: look for near-gray values and remove them.
					double r = inPtr[0];
					double g = inPtr[1];
					double b = inPtr[2];
					int metric = (fabs(r-g) + fabs(r-b) + fabs(g-b)) / 3; // average absolute diff must be less than or equal to this
//					std::cout << QString("  %1,%2,%3 \t %4, %5 -- %6").arg(int(inPtr[0])).arg(int(inPtr[1])).arg(int(inPtr[2])).arg(idxR).arg(idxY).arg(metric) << std::endl;
					if (metric <= 3)
						(*outPtr) = 0;

				}
				//Assume the outVolume is treated with the luminance filter first
				outPtr++;
				inPtr += 3;
			}
			inPtr += inInc[1];
		}
		inPtr += inInc[2];
	}

	return outData;
}

void USFrameData::setPurgeInputDataAfterInitialize(bool value)
{
	mPurgeInput = value;
}

std::vector<std::vector<vtkImageDataPtr> > USFrameData::initializeFrames(std::vector<bool> angio)
{
//	ssc::TimeKeeper timer;
//	std::cout << "USFrameData::initializeFrames start " << mReducedToFull.size() << std::endl;
	std::vector<std::vector<vtkImageDataPtr> > raw(angio.size());

	for (unsigned i=0; i<raw.size(); ++i)
	{
		raw[i].resize(mReducedToFull.size());
	}

	// apply cropping and angio
	for (unsigned i=0; i<mReducedToFull.size(); ++i)
	{
//		timer.printElapsedms("inbetween");
//		timer.reset();
		SSC_ASSERT(mImageContainer->size()>mReducedToFull[i]);
		vtkImageDataPtr current = mImageContainer->get(mReducedToFull[i]);
//		timer.printElapsedms("\tload image\t");
//		timer.reset();

//		std::cout << "i="<<i<< ", " << Eigen::Array3i(current->GetDimensions()) << std::endl;

		if (mCropbox.range()[0]!=0)
			current = this->cropImage(current, mCropbox);

//		std::cout << "i="<<i<< ", " << Eigen::Array3i(current->GetDimensions()) << std::endl;

//		timer.printElapsedms("\t\t\tcrop\t");
//		timer.reset();

		// optimization: grayFrame is used in both calculations: compute once
		vtkImageDataPtr grayFrame = this->toGrayscale(current);
//		timer.printElapsedms("\t\t\tgray\t");
//		timer.reset();

		for (unsigned j=0; j<angio.size(); ++j)
		{

			if (angio[j])
			{
				vtkImageDataPtr angioFrame = this->useAngio(current, grayFrame);
				raw[j][i] = angioFrame;
//				timer.printElapsedms("\t\t\tangio\t");
//				timer.reset();
//				std::cout << "i="<<i<< ", " << Eigen::Array3i(angioFrame->GetDimensions()) << std::endl;
			}
			else
			{
//				vtkImageDataPtr grayFrame = this->toGrayscale(current);
				raw[j][i] = grayFrame;
//				timer.printElapsedms("\t\t\tgray\t");
//				timer.reset();
//				std::cout << "i="<<i<< ", " << Eigen::Array3i(grayFrame->GetDimensions()) << std::endl;
			}
		}

//		timer.printElapsedms("\t\t\tprocess\t");
//		timer.reset();

		if (mPurgeInput)
			mImageContainer->purge(mReducedToFull[i]);

//		timer.printElapsedms("\t\t\tinit\t");
//		timer.reset();
	}

	if (mPurgeInput)
		mImageContainer->purgeAll();

	return raw;
}

void USFrameData::purgeAll()
{
	mImageContainer->purgeAll();
}

USFrameDataPtr USFrameData::copy()
{
	USFrameDataPtr retval(new USFrameData(*this));
	this->initialize();
	return retval;
}

QString USFrameData::getName() const
{
	return QFileInfo(mName).completeBaseName();
}

void USFrameData::fillImageImport(vtkImageImportPtr import, int index)
{
	ssc::TimeKeeper timer;
	vtkImageDataPtr source = mImageContainer->get(index);

//	 use this test code to display angio output:
//	vtkImageDataPtr current = mImageContainer->get(index);
//	current = this->cropImage(current, ssc::IntBoundingBox3D(157, 747, 68, 680, 0, 0));
//	vtkImageDataPtr grayFrame = this->toGrayscale(current);
//	static vtkImageDataPtr source;
//	source = this->useAngio(current, grayFrame);
	source->Update();

	import->SetImportVoidPointer(source->GetScalarPointer());
	import->SetDataScalarType(source->GetScalarType());
	import->SetDataSpacing(source->GetSpacing());
	import->SetNumberOfScalarComponents(source->GetNumberOfScalarComponents());
	import->SetWholeExtent(source->GetWholeExtent());
	import->SetDataExtentToWholeExtent();
}




bool USFrameData::is4D()
{
	int numberOfFrames = mReducedToFull.size();

	if (numberOfFrames > 0)
		if(mImageContainer->get(0)->GetDataDimension() == 3)
			return true;
	return false;
}


}//namespace ssc
