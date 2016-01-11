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


/*
 * sscSlices3DRep.cpp
 *
 *  Created on: Oct 13, 2011
 *      Author: christiana
 */

#include "cxSlices3DRep.h"

#include <vtkRenderer.h>
#include <vtkFloatArray.h>
#include <vtkPlaneSource.h>
#include <vtkPointData.h>
#include <vtkTriangleFilter.h>
#include <vtkStripper.h>
#include <vtkImageData.h>
#include <vtkLookupTable.h>
#include "cxLogger.h"
#include "cxImage.h"
#include "cxView.h"
#include "cxImageLUT2D.h"
#include "cxSliceProxy.h"
#include "cxTypeConversions.h"
#include "cxGPUImageBuffer.h"
#include "cxTexture3DSlicerProxy.h"

#ifndef CX_VTK_OPENGL2
#include <vtkPainterPolyDataMapper.h>
#endif



//---------------------------------------------------------
namespace cx
{
//---------------------------------------------------------

Slices3DRep::Slices3DRep() :
	RepImpl()
{
}

Slices3DRep::~Slices3DRep()
{
}

Slices3DRepPtr Slices3DRep::New(const QString& uid)
{
	return wrap_new(new Slices3DRep(), uid);
}

void Slices3DRep::setShaderPath(QString path)
{
	for (unsigned i=0; i<mProxy.size(); ++i)
		mProxy[i]->setShaderPath(path);
}

void Slices3DRep::setImages(std::vector<ImagePtr> images)
{
	if (images.empty())
	{
		reportWarning("Slices3DRep::setImages: No input images (in ViewGroup)");
		return;
	}
	for (unsigned i=0; i<mProxy.size(); ++i)
	{
		mProxy[i]->setImages(images);
		mProxy[i]->getSliceProxy()->setDefaultCenter(images[0]->get_rMd().coord(images[0]->boundingBox().center()));
	}
}

void Slices3DRep::addPlane(PLANE_TYPE plane, PatientModelServicePtr dataManager)
{
	SliceProxyPtr sliceProxy = SliceProxy::create(dataManager);
	sliceProxy->initializeFromPlane(plane, false, Vector3D(0,0,1), true, 150, 0.25);
	sliceProxy->setAlwaysUseDefaultCenter(true);

	Texture3DSlicerProxyPtr current = Texture3DSlicerProxy::New();
	current->setSliceProxy(sliceProxy);
	current->setTargetSpaceToR();

	mProxy.push_back(current);
}

void Slices3DRep::setTool(ToolPtr tool)
{
	for (unsigned i=0; i<mProxy.size(); ++i)
		mProxy[i]->getSliceProxy()->setTool(tool);
}

void Slices3DRep::addRepActorsToViewRenderer(ViewPtr view)
{
	for (unsigned i=0; i<mProxy.size(); ++i)
		view->getRenderer()->AddActor(mProxy[i]->getActor());
}

void Slices3DRep::removeRepActorsFromViewRenderer(ViewPtr view)
{
	for (unsigned i=0; i<mProxy.size(); ++i)
		view->getRenderer()->RemoveActor(mProxy[i]->getActor());
}

//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------
