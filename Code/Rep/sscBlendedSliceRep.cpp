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

//---------------------------------------------------------
namespace ssc
{
//---------------------------------------------------------

BlendedSliceRep::BlendedSliceRep(const std::string& uid) :
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

BlendedSliceRepPtr BlendedSliceRep::New(const std::string& uid)
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

void BlendedSliceRep::addRepActorsToViewRenderer(View* view)
{
	mImageActor->SetInput(mBlender->GetOutput() );
	view->getRenderer()->AddActor(mImageActor);
}

void BlendedSliceRep::removeRepActorsFromViewRenderer(View* view)
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


