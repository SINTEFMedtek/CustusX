/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxSlicerRepSW.h"

#include <vtkImageActor.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>

#include "cxView.h"
#include "cxImage.h"
#include "cxSlicedImageProxy.h"
#include "vtkImageMapper3D.h"
#include "vtkImageAlgorithm.h"

namespace cx
{

SliceRepSW::SliceRepSW() :
	RepImpl()
{
	mImageSlicer.reset(new SlicedImageProxy());
	mImageActor = vtkImageActorPtr::New();
	mImageActor->GetMapper()->SetInputConnection(mImageSlicer->getOutputPort()->GetOutputPort());
//	mImageActor->SetInput(mImageSlicer->getOutputPort());
}

SliceRepSW::~SliceRepSW()
{
}

SliceRepSWPtr SliceRepSW::New(const QString& uid)
{
	return wrap_new(new SliceRepSW(), uid);
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
	mImageSlicer->setImage(image);
}

QString SliceRepSW::getImageUid()const
{
	return mImageSlicer->getImage() ? mImageSlicer->getImage()->getUid() : "";  
}

void SliceRepSW::setSliceProxy(SliceProxyInterfacePtr slicer)
{
	mImageSlicer->setSliceProxy(slicer);
}

void SliceRepSW::addRepActorsToViewRenderer(ViewPtr view)
{
	view->getRenderer()->AddActor(mImageActor);
}

void SliceRepSW::removeRepActorsFromViewRenderer(ViewPtr view)
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

}// namespace cx
