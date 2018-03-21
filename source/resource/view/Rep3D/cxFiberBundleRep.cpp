/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
