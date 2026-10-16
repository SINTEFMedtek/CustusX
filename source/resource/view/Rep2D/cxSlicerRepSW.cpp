/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxSlicerRepSW.h"

#include <vtkRenderer.h>
#include <vtkImageData.h>
#include <vtkImageSliceMapper.h>
#include <vtkImageMapper3D.h>
#include <vtkImageAlgorithm.h>
#include <vtkImageSlice.h>

#include "cxView.h"
#include "cxImage.h"
#include "cxSlicedImageProxy.h"
#include "cxSliceProxy.h"
#include "cxImage2DRep3D.h"
#include "cxBoundingBox3D.h"

namespace cx
{

SliceRepSW::SliceRepSW() :
	RepImpl(),
	mDrawDirectly(false)
{
	mImageSlicer.reset(new SlicedImageProxy());

	vtkNew<vtkImageSliceMapper> imageSliceMapper;
	imageSliceMapper->SetInputConnection(mImageSlicer->getOutputPort()->GetOutputPort());

	mImageSlice = vtkImageSlicePtr::New();
	mImageSlice->SetMapper(imageSliceMapper);
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

/**This method set the image, that has all the information in it self.
 * color, brigthness, contrast, etc...
 */
void SliceRepSW::setImage( ImagePtr image )
{
	if (image == getImage())
		return;
	ImagePtr previous = this->getImage();
	if (previous)
	{
		disconnect(previous.get(), &Image::transformChanged, this, &SliceRepSW::updateDrawingMethod);
	}
	mImageSlicer->setImage(image);

	if (mRenderer && mDrawDirectly)
	{
		mRenderer->RemoveActor(mImage2D->getSlice());
		mRenderer->AddActor(mImageSlice);
	}
	mDrawDirectly = false;
	mImage2D.reset();
	if (image && image->is2D())
	{
		mImage2D = Image2DProxy::New();
		mImage2D->setImage(image);
		connect(image.get(), &Image::transformChanged, this, &SliceRepSW::updateDrawingMethod);
	}
	this->updateDrawingMethod();
}

QString SliceRepSW::getImageUid()const
{
	return mImageSlicer->getImage() ? mImageSlicer->getImage()->getUid() : "";  
}

void SliceRepSW::setSliceProxy(SliceProxyInterfacePtr slicer)
{
	if (mSlicer)
	{
		disconnect(mSlicer.get(), &SliceProxyInterface::transformChanged, this, &SliceRepSW::updateDrawingMethod);
	}
	mSlicer = slicer;
	mImageSlicer->setSliceProxy(slicer);
	if (mSlicer)
	{
		connect(mSlicer.get(), &SliceProxyInterface::transformChanged, this, &SliceRepSW::updateDrawingMethod);
	}
	this->updateDrawingMethod();
}

bool SliceRepSW::isInSlicePlane(Transform3D sMd, Vector3D center_d)
{
	Vector3D normal_s = sMd.vector(Vector3D(0, 0, 1)).normal();
	bool parallel = std::abs(normal_s[2]) > std::cos(kMaxAngleFromSlicePlane_rad);
	bool inPlane = std::abs(sMd.coord(center_d)[2]) < kMaxDistanceFromSlicePlane_mm;
	return parallel && inPlane;
}

bool SliceRepSW::isDrawnDirectly() const
{
	return mDrawDirectly;
}

/** Only the shown actor is in the renderer: the renderer updates the input of every actor, also hidden ones,
 *  so an image would still be sliced */
void SliceRepSW::updateDrawingMethod()
{
	ImagePtr image = this->getImage();
	bool drawDirectly = false;
	if (mImage2D && mSlicer && image)
	{
		Transform3D sMr = mSlicer->get_sMr();
		drawDirectly = isInSlicePlane(sMr * image->get_rMd(), image->boundingBox().center());
		mImage2D->setTransformOffset(sMr);
	}

	if (mRenderer && drawDirectly != mDrawDirectly)
	{
		mRenderer->RemoveActor(this->getShownSlice());
		mDrawDirectly = drawDirectly;
		mRenderer->AddActor(this->getShownSlice());
	}
	mDrawDirectly = drawDirectly;
}

vtkImageSlicePtr SliceRepSW::getShownSlice() const
{
	return mDrawDirectly ? mImage2D->getSlice() : mImageSlice;
}

void SliceRepSW::addRepActorsToViewRenderer(ViewPtr view)
{
	mRenderer = view->getRenderer();
	mRenderer->AddActor(this->getShownSlice());
}

void SliceRepSW::removeRepActorsFromViewRenderer(ViewPtr view)
{
	view->getRenderer()->RemoveActor(this->getShownSlice());
	mRenderer = vtkRendererPtr();
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
