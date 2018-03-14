/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxTexture3DSlicerRep.h"

#include <vtkRenderer.h>
#include <vtkFloatArray.h>
#include <vtkPlaneSource.h>
#include <vtkPointData.h>
#include <vtkTriangleFilter.h>
#include <vtkStripper.h>
#include <vtkImageData.h>
#include <vtkLookupTable.h>

#include "cxImage.h"
#include "cxView.h"
#include "cxImageLUT2D.h"
#include "cxSliceProxy.h"
#include "cxTypeConversions.h"
#include "cxGPUImageBuffer.h"
#include "cxTexture3DSlicerProxy.h"

#include "cxSharedOpenGLContext.h"

//---------------------------------------------------------
namespace cx
{
//---------------------------------------------------------

void Texture3DSlicerRep::setTargetSpaceToR()
{
	mProxy->setTargetSpaceToR();
}

Texture3DSlicerRep::Texture3DSlicerRep(SharedOpenGLContextPtr context) :
	RepImpl()
{
	mProxy = Texture3DSlicerProxy::New(context);
}

Texture3DSlicerRep::~Texture3DSlicerRep()
{
}

QString Texture3DSlicerRep::getType() const
{
	return "Texture3DSlicerRep";
}

Texture3DSlicerRepPtr Texture3DSlicerRep::New(SharedOpenGLContextPtr context, const QString& uid)
{
	return wrap_new(new Texture3DSlicerRep(context), uid);
}

void Texture3DSlicerRep::setShaderPath(QString path)
{
	mProxy->setShaderPath(path);
}

void Texture3DSlicerRep::viewChanged()
{
	if (!this->getView())
		return;
	if (this->getView()->getZoomFactor() < 0)
		return; // ignore if zoom is invalid
	mProxy->setViewportData(this->getView()->get_vpMs(), this->getView()->getViewport());
}

void Texture3DSlicerRep::setImages(std::vector<ImagePtr> images)
{
	mProxy->setImages(images);
}

std::vector<ImagePtr> Texture3DSlicerRep::getImages()
{
	return mProxy->getImages();
}

void Texture3DSlicerRep::setSliceProxy(SliceProxyPtr slicer)
{
	mProxy->setSliceProxy(slicer);
}

void Texture3DSlicerRep::addRepActorsToViewRenderer(ViewPtr view)
{
	view->getRenderer()->AddActor(mProxy->getActor());
	connect(view.get(), SIGNAL(resized(QSize)), this, SLOT(viewChanged()));
	this->viewChanged();
}

void Texture3DSlicerRep::removeRepActorsFromViewRenderer(ViewPtr view)
{
	view->getRenderer()->RemoveActor(mProxy->getActor());
	disconnect(view.get(), SIGNAL(resized(QSize)), this, SLOT(viewChanged()));
}

void Texture3DSlicerRep::printSelf(std::ostream & os, Indent indent)
{

}

void Texture3DSlicerRep::setViewportData(const Transform3D& vpMs, const DoubleBoundingBox3D& vp)
{
	mProxy->setViewportData(vpMs, vp);
}

bool Texture3DSlicerRep::isSupported(vtkRenderWindowPtr window)
{
	return Texture3DSlicerProxy::isSupported(window);
}

void Texture3DSlicerRep::setRenderWindow(vtkRenderWindowPtr window)
{
	mProxy->setRenderWindow(window);
}

//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------
