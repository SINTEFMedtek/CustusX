#include "sscSlicerRepSW.h"

#include <boost/lexical_cast.hpp>
#include <vtkImageActor.h>
#include <vtkImageReslice.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkWindowLevelLookupTable.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkImageBlend.h>
#include <vtkLookupTable.h>

#include <vtkColorTransferFunction.h>
#include "sscView.h"
#include "sscDataManager.h"
#include "sscSliceProxy.h"

#include "sscBoundingBox3D.h"

namespace ssc
{

SliceRepSW::SliceRepSW(const std::string& uid) :
	RepImpl(uid)
{
	mImageSlicer.reset(new SlicedImageProxy());
	mImageActor = vtkImageActorPtr::New();
	mImageActor->SetInput( mImageSlicer->getOutput());
}

SliceRepSW::~SliceRepSW()
{
}

SliceRepSWPtr SliceRepSW::New(const std::string& uid)
{
	SliceRepSWPtr retval(new SliceRepSW(uid));
	retval->mSelf = retval;
	return retval;
}

ImagePtr SliceRepSW::getImage() 
{
	return mImageSlicer->getImage(); 
}

vtkImageActorPtr SliceRepSW::getActor()
{
	return mImageActor; 
}

/**This method set the image, that has all the information in it self.
 * color, brigthness, contrast, etc...
 */
void SliceRepSW::setImage( ImagePtr image )
{
	if (getImage())
	{
		getImage()->disconnectRep(mSelf);
	}
	mImageSlicer->setImage(image);
	if (getImage())
	{
		getImage()->connectRep(mSelf);
	}
}

std::string SliceRepSW::getImageUid()const
{
	return mImageSlicer->getImage() ? mImageSlicer->getImage()->getUid() : "";  
}

void SliceRepSW::setSliceProxy(ssc::SliceProxyPtr slicer)
{
	mImageSlicer->setSliceProxy(slicer);
}

void SliceRepSW::addRepActorsToViewRenderer(View* view)
{
	view->getRenderer()->AddActor(mImageActor);
}

void SliceRepSW::removeRepActorsFromViewRenderer(View* view)
{
	view->getRenderer()->RemoveActor(mImageActor);
}

bool SliceRepSW::hasImage(ImagePtr image) const
{
	return (mImageSlicer->getImage() != NULL);
}

void SliceRepSW::update()
{
	mImageSlicer->update();
}

void SliceRepSW::printSelf(std::ostream & os, Indent indent)
{
	mImageSlicer->printSelf(os, indent);
}

}// namespace ssc
