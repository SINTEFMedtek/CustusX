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
#include <vtkMatrix4x4.h>

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
	mDrawDirectly(false),
	mDirectDrawingEnabled(true)
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

void SliceRepSW::setImage( ImagePtr image )
{
	if (image == getImage())
		return;
	ImagePtr previous = this->getImage();
	if (previous)
	{
		disconnect(previous.get(), &Image::transformChanged, this, &SliceRepSW::drawingInputChanged);
		disconnect(previous.get(), &Image::vtkImageDataChanged, this, &SliceRepSW::drawingInputChanged);
	}
	mImageSlicer->setImage(image);

	this->showSlice(false);
	mImage2D.reset();
	if (image)
	{
		connect(image.get(), &Image::transformChanged, this, &SliceRepSW::drawingInputChanged);
		connect(image.get(), &Image::vtkImageDataChanged, this, &SliceRepSW::drawingInputChanged);
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
		disconnect(mSlicer.get(), &SliceProxyInterface::transformChanged, this, &SliceRepSW::drawingInputChanged);
	}
	mSlicer = slicer;
	mImageSlicer->setSliceProxy(slicer);
	if (mSlicer)
	{
		connect(mSlicer.get(), &SliceProxyInterface::transformChanged, this, &SliceRepSW::drawingInputChanged);
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

/** Draw the image exactly at z=0, like the sliced images, so its depth order against the other layers is fixed */
Transform3D SliceRepSW::moveIntoSlicePlane(Transform3D sMd, Vector3D center_d)
{
	Vector3D normal_s = sMd.vector(Vector3D(0, 0, 1)).normal();
	Vector3D target_s(0, 0, normal_s[2] < 0 ? -1 : 1);
	Vector3D center_s = sMd.coord(center_d);
	Transform3D rotation = Transform3D::Identity();
	rotation.linear() = Eigen::Quaterniond::FromTwoVectors(normal_s, target_s).toRotationMatrix();
	Transform3D toPlane = createTransformTranslate(Vector3D(center_s[0], center_s[1], 0)) * rotation * createTransformTranslate(-center_s);
	return toPlane * sMd;
}

bool SliceRepSW::isDrawnDirectly() const
{
	return mDrawDirectly;
}

void SliceRepSW::setDirectDrawingEnabled(bool enabled)
{
	mDirectDrawingEnabled = enabled;
	this->updateDrawingMethod();
	this->setModified();
}

void SliceRepSW::drawingInputChanged()
{
	this->setModified();
}

void SliceRepSW::onModifiedStartRender()
{
	this->updateDrawingMethod();
}

/** Decided before rendering: the slice plane and the image move in separate signals from the same tool update */
void SliceRepSW::updateDrawingMethod()
{
	ImagePtr image = this->getImage();
	bool drawDirectly = false;
	if (mDirectDrawingEnabled && mSlicer && image && image->is2D())
	{
		Transform3D sMd = mSlicer->get_sMr() * image->get_rMd();
		Vector3D center_d = image->boundingBox().center();
		drawDirectly = isInSlicePlane(sMd, center_d);
		if (drawDirectly && !mImage2D)
		{
			mImage2D = Image2DProxy::New();
			mImage2D->setImage(image);
		}
		if (drawDirectly)
		{
			mImageSlice->SetUserMatrix(moveIntoSlicePlane(sMd, center_d).getVtkMatrix());
		}
	}
	this->showSlice(drawDirectly);
}

/** The sliced image is disconnected while drawing directly, so the image is not sliced as well */
void SliceRepSW::showSlice(bool drawDirectly)
{
	if (drawDirectly != mDrawDirectly)
	{
		vtkAlgorithm* input = drawDirectly ? mImage2D->getOutputPort().Get() : mImageSlicer->getOutputPort().Get();
		mImageSlice->GetMapper()->SetInputConnection(input->GetOutputPort());
		if (!drawDirectly)
		{
			mImageSlice->SetUserMatrix(nullptr);
		}
	}
	mDrawDirectly = drawDirectly;
}

vtkImageSlicePtr SliceRepSW::getActor() const
{
	return mImageSlice;
}

void SliceRepSW::addRepActorsToViewRenderer(ViewPtr view)
{
	view->getRenderer()->AddActor(mImageSlice);
}

void SliceRepSW::removeRepActorsFromViewRenderer(ViewPtr view)
{
	view->getRenderer()->RemoveActor(mImageSlice);
}

bool SliceRepSW::hasImage(ImagePtr image) const
{
	return (mImageSlicer->getImage() != NULL);
}

void SliceRepSW::update()
{
	mImageSlicer->update();
	this->updateDrawingMethod();
}

void SliceRepSW::printSelf(std::ostream & os, Indent indent)
{
	mImageSlicer->printSelf(os, indent);
}

}// namespace cx
