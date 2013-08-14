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

#include "sscSlicerRepSW.h"

#include <vtkImageActor.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>

#include "sscView.h"
#include "sscImage.h"
#include "sscSlicedImageProxy.h"

namespace ssc
{

SliceRepSW::SliceRepSW(const QString& uid) :
	RepImpl(uid)
{
	mImageSlicer.reset(new SlicedImageProxy());
	mImageActor = vtkImageActorPtr::New();
	mImageActor->SetInput( mImageSlicer->getOutput());
}

SliceRepSW::~SliceRepSW()
{
}

SliceRepSWPtr SliceRepSW::New(const QString& uid)
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
	if (image == getImage())
		return;
	if (getImage())
	{
		getImage()->disconnectFromRep(mSelf);
	}
	mImageSlicer->setImage(image);
	if (getImage())
	{
		getImage()->connectToRep(mSelf);
	}
}

QString SliceRepSW::getImageUid()const
{
	return mImageSlicer->getImage() ? mImageSlicer->getImage()->getUid() : "";  
}

void SliceRepSW::setSliceProxy(ssc::SliceProxyPtr slicer)
{
	mImageSlicer->setSliceProxy(slicer);
}

void SliceRepSW::addRepActorsToViewRenderer(View *view)
{
	view->getRenderer()->AddActor(mImageActor);
}

void SliceRepSW::removeRepActorsFromViewRenderer(View *view)
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
