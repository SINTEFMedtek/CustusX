/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/


#include "cxTexture3DSlicerRep.h"

#include <vtkRenderer.h>
#include <vtkFloatArray.h>
#include <vtkPlaneSource.h>
#include <vtkPointData.h>
#include <vtkTriangleFilter.h>
#include <vtkStripper.h>
#include <vtkImageData.h>
//#include <vtkPainterPolyDataMapper.h>
#include <vtkLookupTable.h>

#include "cxImage.h"
#include "cxView.h"
#include "cxImageLUT2D.h"
#include "cxSliceProxy.h"
#include "cxTypeConversions.h"
#include "cxGPUImageBuffer.h"
#include "cxTexture3DSlicerProxy.h"

//---------------------------------------------------------
namespace cx
{
//---------------------------------------------------------

void Texture3DSlicerRep::setTargetSpaceToR()
{
	mProxy->setTargetSpaceToR();
}

Texture3DSlicerRep::Texture3DSlicerRep() :
	RepImpl()
{
	mProxy = Texture3DSlicerProxy::New();
}

Texture3DSlicerRep::~Texture3DSlicerRep()
{
}

Texture3DSlicerRepPtr Texture3DSlicerRep::New(const QString& uid)
{
	return wrap_new(new Texture3DSlicerRep(), uid);
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

//void Texture3DSlicerRep::update()
//{
//	mProxy->update();
//}

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

//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------
