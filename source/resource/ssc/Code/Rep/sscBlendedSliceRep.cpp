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

#include "sscBlendedSliceRep.h"

#include <vtkImageActor.h>
#include <vtkImageReslice.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>
#include <vtkImageBlend.h>

#include "sscView.h"
#include "sscImageLUT2D.h"
#include "sscImage.h"
#include "sscSlicedImageProxy.h"
#include "sscTypeConversions.h"

//---------------------------------------------------------
namespace ssc
{
//---------------------------------------------------------

BlendedSliceRep::BlendedSliceRep(const QString& uid) :
	RepImpl(uid)
{
	mBlender = vtkImageBlendPtr::New();
	mImageActor = vtkImageActorPtr::New();

	// set up the slicer pipeline
	mBlender->SetBlendModeToNormal();
	firstImage = true;
	countImage = 0;
}

BlendedSliceRep::~BlendedSliceRep()
{
}

BlendedSliceRepPtr BlendedSliceRep::New(const QString& uid)
{
	BlendedSliceRepPtr retval(new BlendedSliceRep(uid));
	retval->mSelf = retval;
	return retval;
}

void BlendedSliceRep::setSliceProxy(SliceProxyPtr slicer)
{
	mSlicer = slicer;
}

void BlendedSliceRep::setImages(std::vector<ImagePtr> images)
{
	for (unsigned int i = 0; i< images.size(); ++i)
	{
		std::cout<<"BlendedSliceRep::setImages(): slice image: id" << images.at(i)->getUid()<<std::endl;
		ImagePtr image = images.at(i);
		connect(image.get(), SIGNAL(transferFunctionsChanged()), this, SLOT(updateAlphaSlot()));

		SlicedImageProxyPtr slicedImage(new ssc::SlicedImageProxy());
		slicedImage->setSliceProxy(mSlicer);
		slicedImage->setImage(image);
		slicedImage->update();
		mSlices.push_back(slicedImage);
		addInputImages(slicedImage->getOutput());
	}
}

void BlendedSliceRep::addInputImages(vtkImageDataPtr slicedImage)
{
//	std::cout<<".. in blender"<<std::endl;
	countImage++;
	if (firstImage)
	{
		mBlender->RemoveAllInputs();
		firstImage = false;
		mBaseImages = slicedImage;

	}
	//** hmmm en ny resample for hvergang ...?+?
	vtkImageReslicePtr resample = vtkImageReslicePtr::New();
	resample->SetInput(slicedImage);
	resample->SetInterpolationModeToLinear();
	resample->SetInformationInput(mBaseImages);

	mBlender->SetOpacity(countImage, getAlpha(countImage));
	mBlender->AddInputConnection(0, resample->GetOutputPort() );
//	mBlender->AddInput(0, slicedImage );
}

double BlendedSliceRep::getAlpha(int countImage)
{
	if (countImage<int(mSlices.size()))
	{
		return mSlices[countImage]->getImage()->getLookupTable2D()->getAlpha();
	}
	else
	{
		return 1.0;
	}
}

void BlendedSliceRep::addRepActorsToViewRenderer(View *view)
{
	mImageActor->SetInput(mBlender->GetOutput() );
	view->getRenderer()->AddActor(mImageActor);
}

void BlendedSliceRep::removeRepActorsFromViewRenderer(View *view)
{
	view->getRenderer()->RemoveActor(mImageActor);
}

void BlendedSliceRep::update()
{
	updateAlphaSlot();
}

//void BlendedSliceRep::updateThresholdSlot(double val)
//{
//	val = val/100.0;
//	std::cout<<"mBlender got threshold :"<<val<<std::endl;
//	mBlender->SetCompoundThreshold(val);
//}

/**SLOT
 *this get signal if alpha changes.. will excecute render pipeline
 **/
void BlendedSliceRep::updateAlphaSlot()
{
	for (int i=0; i<mBlender->GetNumberOfInputs(); i++)
	{
		//std::cout << "getAlpha(i) " << getAlpha(i) << std::endl;
		mBlender->SetOpacity(i, getAlpha(i));
		mBlender->Update();
	}
}

//---------------------------------------------------------
}// namespace ssc
//---------------------------------------------------------


