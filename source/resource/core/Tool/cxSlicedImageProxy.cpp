/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxSlicedImageProxy.h"

#include <vtkImageReslice.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkWindowLevelLookupTable.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkImageAlgorithm.h>
#include <vtkImageChangeInformation.h>
#include <vtkImageExtractComponents.h>
#include <vtkImageAppendComponents.h>

#include "cxImage.h"
#include "cxSliceProxy.h"
#include "cxImageLUT2D.h"
#include "cxTypeConversions.h"


namespace cx
{


ApplyLUTToImage2DProxy::ApplyLUTToImage2DProxy()
{
	mDummyImage = Image::createDummyImageData(1, 0);

	mRedirecter = vtkSmartPointer<vtkImageChangeInformation>::New(); // used for forwarding only.
	mRedirecter->SetInputData(mDummyImage);
}

ApplyLUTToImage2DProxy::~ApplyLUTToImage2DProxy()
{

}

void ApplyLUTToImage2DProxy::setInputData(vtkImageDataPtr image, vtkLookupTablePtr lut)
{
	vtkImageChangeInformationPtr redirecter = vtkImageChangeInformationPtr::New();
	redirecter->SetInputData(image);
	redirecter->Update();
	this->setInput(redirecter, lut);
}

void ApplyLUTToImage2DProxy::setInput(vtkImageAlgorithmPtr input, vtkLookupTablePtr lut)
{
	input->Update();

	if (input->GetOutput())
	{
		int numScalarComponents = input->GetOutput()->GetNumberOfScalarComponents();
		if ( numScalarComponents >= 3) // color
		{
			// split the image into the components, apply the lut, then merge.

			vtkImageAppendComponentsPtr merger = vtkImageAppendComponentsPtr::New();

			for (int i = 0; i < numScalarComponents; ++i)
			{
				vtkImageMapToColorsPtr compWindowLevel = vtkImageMapToColorsPtr::New();
				compWindowLevel->SetInputConnection(input->GetOutputPort());
				compWindowLevel->SetActiveComponent(i);
				compWindowLevel->SetLookupTable(lut);

				if (i==2 && numScalarComponents==3)
				{
					compWindowLevel->SetOutputFormatToLuminanceAlpha();
				}
				else
				{
					compWindowLevel->SetOutputFormatToLuminance();
				}

				merger->AddInputConnection(compWindowLevel->GetOutputPort());
			}

			mRedirecter->SetInputConnection(merger->GetOutputPort());
		}
		else // grayscale
		{
			vtkImageMapToColorsPtr windowLevel = vtkImageMapToColorsPtr::New();
			windowLevel->SetOutputFormatToRGBA();
			windowLevel->SetInputConnection(input->GetOutputPort());
			windowLevel->SetLookupTable(lut);
			mRedirecter->SetInputConnection(windowLevel->GetOutputPort());
		}
	}
	else // no image
	{
		mRedirecter->SetInputData(mDummyImage);
	}

	mRedirecter->Update();
}

vtkImageAlgorithmPtr ApplyLUTToImage2DProxy::getOutputPort()
{
	return mRedirecter;
}

vtkImageDataPtr ApplyLUTToImage2DProxy::getOutput()
{
	return mRedirecter->GetOutput();
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

	mRedirecter = vtkImageChangeInformationPtr::New();
}

SlicedImageProxy::~SlicedImageProxy()
{
}

void SlicedImageProxy::setOutputFormat(Vector3D origin, Eigen::Array3i dim, Vector3D spacing)
{
	mReslicer->SetOutputOrigin(origin.data());
	mReslicer->SetOutputExtent(0, dim[0], 0, dim[1], 0, 0);
	// this looks like the correct way, but gives incorrect output (the way it is used)
	// TODO investigate
//	mReslicer->SetOutputExtent(0, dim[0]-1, 0, dim[1]-1, 0, 0);
	mReslicer->SetOutputSpacing(spacing.data());
}

void SlicedImageProxy::setSliceProxy(SliceProxyInterfacePtr slicer)
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
	mReslicer->SetInputData(mImage->getBaseVtkImageData());
	mReslicer->SetBackgroundLevel(mImage->getMin());
	mImageWithLUTProxy->setInput(mRedirecter, mImage->getLookupTable2D()->getOutputLookupTable());
}

void SlicedImageProxy::updateRedirecterSlot()
{
	mRedirecter->SetInputConnection(mReslicer->GetOutputPort());
	update();
}

void SlicedImageProxy::setImage(ImagePtr image)
{
	if (mImage)
	{
		disconnect(mImage.get(), SIGNAL(transferFunctionsChanged()), this, SLOT(transferFunctionsChangedSlot()));
		disconnect(mImage.get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
		disconnect(mImage.get(), SIGNAL(vtkImageDataChanged()), this, SLOT(updateRedirecterSlot()));
	}

	mImage = image;

	if (mImage)
	{
		connect(mImage.get(), SIGNAL(transferFunctionsChanged()), this, SLOT(transferFunctionsChangedSlot()));
		connect(mImage.get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
		connect(mImage.get(), SIGNAL(vtkImageDataChanged()), this, SLOT(updateRedirecterSlot()));
	}

	if (mImage)
	{
		this->updateRedirecterSlot();
		this->transferFunctionsChangedSlot();
	}
	else // no image
	{
		mImageWithLUTProxy->setInput(vtkImageAlgorithmPtr(), vtkLookupTablePtr());
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

vtkImageAlgorithmPtr SlicedImageProxy::getOutputPort()
{
	return mImageWithLUTProxy->getOutputPort();
}

vtkImageDataPtr SlicedImageProxy::getOutputWithoutLUT()
{
	return mRedirecter->GetOutput();
}

vtkImageAlgorithmPtr SlicedImageProxy::getOutputPortWithoutLUT()
{
	return mRedirecter;
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
