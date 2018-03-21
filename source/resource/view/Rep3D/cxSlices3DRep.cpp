/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

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


//---------------------------------------------------------
namespace cx
{
//---------------------------------------------------------

Slices3DRep::Slices3DRep(SharedOpenGLContextPtr context) :
	RepImpl(),
	mSharedOpenGLContext(context)
{
}

Slices3DRep::~Slices3DRep()
{
}

Slices3DRepPtr Slices3DRep::New(SharedOpenGLContextPtr context, const QString& uid)
{
	return wrap_new(new Slices3DRep(context), uid);
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
	sliceProxy->initializeFromPlane(plane, false, true, 150, 0.25);
	sliceProxy->setAlwaysUseDefaultCenter(true);

	Texture3DSlicerProxyPtr current = Texture3DSlicerProxy::New(mSharedOpenGLContext);
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
