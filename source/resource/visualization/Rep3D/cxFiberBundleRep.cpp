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


#include "cxFiberBundleRep.h"

#include "boost/bind.hpp"
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkMatrix4x4.h>
#include <vtkRenderer.h>
#include <vtkRibbonFilter.h>

#include "cxGraphicalPrimitives.h"
#include "cxView.h"
#include "cxTypeConversions.h"
#include "cxDataManager.h"
#include "cxMesh.h"

namespace cx
{
/** Constructor */
FiberBundleRep::FiberBundleRep()
	: RepImpl(), mFiberWidth(.5)
{
	mPolyDataMapper = vtkPolyDataMapperPtr::New();
	mProperty = vtkPropertyPtr::New();
	// mProperty->SetInterpolationToFlat();
	// mProperty->SetInterpolationToGouraud();

	mActor = vtkActorPtr::New();

	mActor->SetMapper(mPolyDataMapper);
	mActor->SetProperty(mProperty);
}

/**
 * Creates a new smart pointer object of this rep
 */
FiberBundleRepPtr FiberBundleRep::New(const QString& uid)
{
	return wrap_new(new FiberBundleRep(), uid);
}

/** Return type as string */
QString FiberBundleRep::getType() const
{
	return "FiberBundleRep";
}

/**
 * Assign new fiber bundle object
 */
void FiberBundleRep::setBundle(MeshPtr bundle)
{
	// Already has this bundle assigned
	if (hasBundle(bundle)) return;

	std::cout << "New fiber bundle assigned" << std::endl;

	if (mBundle)
	{
		disconnect(mBundle.get(), SIGNAL(transformChanged()), this, SLOT(bundleTransformChanged()));
		disconnect(mBundle.get(), SIGNAL(meshChanged()), this, SLOT(bundleChanged()));
	}

	mBundle = bundle;

	if (mBundle)
	{
		connect(mBundle.get(), SIGNAL(transformChanged()), this, SLOT(bundleTransformChanged()));
		connect(mBundle.get(), SIGNAL(meshChanged()), this, SLOT(bundleChanged()));
	}

	bundleChanged();
	this->bundleTransformChanged();
}

/**
 * Adds rep assigned actors to the active renderer
 */
void FiberBundleRep::addRepActorsToViewRenderer(ViewPtr view)
{
	view->getRenderer()->AddActor(mActor);

	if (mViewportListener)
		mViewportListener->startListen(view->getRenderer());
}

/**
 * Removes rep assigned actors from the rendering pipeline
 */
void FiberBundleRep::removeRepActorsFromViewRenderer(ViewPtr view)
{
	view->getRenderer()->RemoveActor(mActor);

	if (mViewportListener)
		mViewportListener->stopListen();
}

/** Called whenever bundle has changed */
void FiberBundleRep::bundleChanged()
{
	vtkPolyDataPtr model = mBundle->getVtkPolyData();

	if (model)
	{
		QColor color = mBundle->getColor();
		mActor->GetProperty()->SetColor(color.redF(), color.greenF(), color.blueF());
		mActor->GetProperty()->SetOpacity(color.alphaF());

		if (!mBundle->getIsWireframe())
		{
			/** Create a filter for the mesh.
			 * This filter enables shading and enhances the otherwise hard to differentiate lines.
			 */
			vtkSmartPointer<vtkRibbonFilter> ribbonFilter = vtkSmartPointer<vtkRibbonFilter>::New();
			ribbonFilter->SetInputData(model);
			ribbonFilter->SetWidth(mFiberWidth);
			ribbonFilter->SetWidthFactor(3);

			mPolyDataMapper->SetInputConnection(ribbonFilter->GetOutputPort());
		}
		else
		{
			{
				vtkPolyDataNormalsPtr normals = vtkPolyDataNormalsPtr::New();
				normals->SetInputData(model);
				normals->Update();
				model = normals->GetOutput();
			}
			mPolyDataMapper->SetInputData(model);
			mActor->SetMapper(mPolyDataMapper);
		}

		mPolyDataMapper->ScalarVisibilityOn();
		mPolyDataMapper->SetScalarModeToUsePointFieldData();
	}

}

/** Called when a new bundle transformation is available
 */
void FiberBundleRep::bundleTransformChanged()
{
	if (!mBundle) return;

	mActor->SetUserMatrix(mBundle->get_rMd().getVtkMatrix());
}

} // end namespace
