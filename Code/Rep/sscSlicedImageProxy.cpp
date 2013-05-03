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

#include "sscSlicedImageProxy.h"

#include <vtkImageReslice.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkWindowLevelLookupTable.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkImageAlgorithm.h>
#include <vtkImageChangeInformation.h>
#include <vtkImageExtractComponents.h>
#include <vtkImageAppendComponents.h>

#include "sscImage.h"
#include "sscSliceProxy.h"
#include "sscImageLUT2D.h"
#include "sscTypeConversions.h"

namespace ssc
{


ApplyLUTToImage2DProxy::ApplyLUTToImage2DProxy()
{
	mDummyImage = createDummyImageData();

	mRedirecter = vtkSmartPointer<vtkImageChangeInformation>::New(); // used for forwarding only.
	mRedirecter->SetInput(mDummyImage);
}

ApplyLUTToImage2DProxy::~ApplyLUTToImage2DProxy()
{

}

void ApplyLUTToImage2DProxy::setInput(vtkImageDataPtr image, vtkLookupTablePtr lut)
{
	if (image)
	{
		if (image->GetNumberOfScalarComponents() == 3) // color
		{
			// split the image into the components, apply the lut, then merge.

			vtkImageAppendComponentsPtr merger = vtkImageAppendComponentsPtr::New();

			for (int i = 0; i < 3; ++i)
			{
				vtkImageMapToColorsPtr compWindowLevel = vtkImageMapToColorsPtr::New();
				compWindowLevel->SetInput(image);
				compWindowLevel->SetActiveComponent(i);
				compWindowLevel->SetLookupTable(lut);
				if (i==2) //TODO the only thing missing here is the alpha channel. Should be able to pass that on from the last pipe.
				{
					compWindowLevel->SetOutputFormatToLuminanceAlpha();
				}
				else
				{
					compWindowLevel->SetOutputFormatToLuminance();
				}

				merger->SetInput(i, compWindowLevel->GetOutput());
			}

			mRedirecter->SetInput(merger->GetOutput());
			//mRedirecter->SetInput(mReslicer->GetOutput());
		}
		else // grayscale
		{
			vtkImageMapToColorsPtr windowLevel = vtkImageMapToColorsPtr::New();
			windowLevel->SetOutputFormatToRGBA();
			windowLevel->SetInput(image);
			windowLevel->SetLookupTable(lut);
			windowLevel->Update();
			mRedirecter->SetInput(windowLevel->GetOutput());
		}
	}
	else // no image
	{
		mRedirecter->SetInput(mDummyImage);
	}
}

vtkImageDataPtr ApplyLUTToImage2DProxy::getOutput()
{
	return mRedirecter->GetOutput();
}

/** Test: create small dummy data set with one voxel
 *
 */
vtkImageDataPtr ApplyLUTToImage2DProxy::createDummyImageData()
{
	vtkImageDataPtr dummyImageData = vtkImageDataPtr::New();
	dummyImageData->SetExtent(0, 0, 0, 0, 0, 0);
	dummyImageData->SetSpacing(1, 1, 1);
	//dummyImageData->SetScalarTypeToUnsignedShort();
	dummyImageData->SetScalarTypeToUnsignedChar();
	dummyImageData->SetNumberOfScalarComponents(1);
	dummyImageData->AllocateScalars();
	unsigned char* dataPtr = static_cast<unsigned char*> (dummyImageData->GetScalarPointer());
	*dataPtr = 0; // Set voxel to black
	return dummyImageData;
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------


SlicedImageProxy::SlicedImageProxy()
{
	mMatrixAxes = vtkMatrix4x4Ptr::New();

	mReslicer = vtkImageReslicePtr::New();
	mReslicer->SetInterpolationModeToLinear();
	mReslicer->SetOutputDimensionality(2);
	mReslicer->SetResliceAxes(mMatrixAxes);
	//mReslicer->SetAutoCropOutput(false); //faster update rate
	mReslicer->AutoCropOutputOn(); // fix used in 2.0.9, but slower update rate

	mImageWithLUTProxy.reset(new ApplyLUTToImage2DProxy());
}

SlicedImageProxy::~SlicedImageProxy()
{
}

void SlicedImageProxy::setSliceProxy(SliceProxyPtr slicer)
{
	if (mSlicer)
	{
		disconnect(mSlicer.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(transformChangedSlot()));
	}
	mSlicer = slicer;
	if (mSlicer)
	{
		connect(mSlicer.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(transformChangedSlot()));
		update();
	}
}

void SlicedImageProxy::transferFunctionsChangedSlot()
{
	mImageWithLUTProxy->setInput(mImage->getBaseVtkImageData(), mImage->getLookupTable2D()->getOutputLookupTable());
}

void SlicedImageProxy::setImage(ImagePtr image)
{
	if (mImage)
	{
		disconnect(mImage.get(), SIGNAL(transferFunctionsChanged()), this, SLOT(transferFunctionsChangedSlot()));
		disconnect(mImage.get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
		disconnect(mImage.get(), SIGNAL(vtkImageDataChanged()), this, SLOT(transformChangedSlot()));
	}

	mImage = image;

	if (mImage)
	{
		connect(mImage.get(), SIGNAL(transferFunctionsChanged()), this, SLOT(transferFunctionsChangedSlot()));
		connect(mImage.get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
		connect(mImage.get(), SIGNAL(vtkImageDataChanged()), this, SLOT(transformChangedSlot()));
	}

	if (mImage)
	{
		mReslicer->SetInput(mImage->getBaseVtkImageData());
		mReslicer->SetBackgroundLevel(mImage->getMin());

		vtkImageDataPtr input = mReslicer->GetOutput();
		// if input is 2D - use directly
		if (mImage->getBaseVtkImageData()->GetDimensions()[2]==1)
			input = mImage->getBaseVtkImageData();

		mImageWithLUTProxy->setInput(input, mImage->getLookupTable2D()->getOutputLookupTable());
	}
	else // no image
	{
		mImageWithLUTProxy->setInput(vtkImageDataPtr(), vtkLookupTablePtr());
	}

	this->update();
}

ImagePtr SlicedImageProxy::getImage() const
{
	return mImage;
}

vtkImageDataPtr SlicedImageProxy::getOutput()
{
	return mImageWithLUTProxy->getOutput();
}

void SlicedImageProxy::update()
{
	if (!mImage)
		return;

	Transform3D rMs = Transform3D::Identity();
	if (mSlicer)
		rMs = mSlicer->get_sMr().inv();
	Transform3D iMr = mImage->get_rMd().inv();
	Transform3D M = iMr * rMs;
	//	std::cout << "iMs, "<< mSlicer->getName() <<"\n" << M << std::endl;

	mMatrixAxes->DeepCopy(M.getVtkMatrix());
}

void SlicedImageProxy::transformChangedSlot()
{
	update();
}

void SlicedImageProxy::printSelf(std::ostream & os, Indent indent)
{
	//os << indent << "PlaneType: " << mType << std::endl;
	os << indent << "mImage: " << (mImage ? mImage->getUid() : "NULL") << std::endl;
	os << indent << "mSlicer: " << (mSlicer ? mSlicer.get() : 0) << std::endl;
	if (mSlicer)
	{
		mSlicer->printSelf(os, indent.stepDown());
	}
	os << indent << "mReslicer->GetOutput(): " << mReslicer->GetOutput() << std::endl;
	os << indent << "mReslicer->GetInput() : " << mReslicer->GetInput() << std::endl;
	Transform3D test(mReslicer->GetResliceAxes());
	os << indent << "resliceaxes: " << std::endl;
	test.put(os, indent.getIndent() + 3);
	os << std::endl;
	//os << indent << "rMs_debug: " << std::endl;
	//rMs_debug.put(os, indent.getIndent()+3);

}

//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------
