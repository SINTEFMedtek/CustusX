/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxSlices3DRep.h"

#include <vtkRenderer.h>
#include <vtkMatrix4x4.h>
#include <vtkImageAlgorithm.h>
#include <vtkImageSliceMapper.h>
#include <vtkImageSlice.h>

#include "cxLogger.h"
#include "cxSliceProxy.h"
#include "cxSlicedImageProxy.h"
#include "cxImage.h"
#include "cxView.h"
#include "cxPatientModelService.h"

namespace cx
{

Slice3DProxy::Slice3DProxy()
{
	mSlicer.reset(new SlicedImageProxy());

	vtkNew<vtkImageSliceMapper> mapper;
	mapper->SetInputConnection(mSlicer->getOutputPort()->GetOutputPort());

	mActor = vtkImageSlicePtr::New();
	mActor->SetMapper(mapper);
}

Slice3DProxy::~Slice3DProxy()
{
}

boost::shared_ptr<Slice3DProxy> Slice3DProxy::New()
{
	return boost::shared_ptr<Slice3DProxy>(new Slice3DProxy());
}

vtkImageSlicePtr Slice3DProxy::getActor()
{
	return mActor;
}

void Slice3DProxy::setSliceProxy(SliceProxyPtr sliceProxy)
{
	if (mSliceProxy)
		disconnect(mSliceProxy.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(transformChangedSlot(Transform3D)));

	mSliceProxy = sliceProxy;
	mSlicer->setSliceProxy(sliceProxy);

	if (mSliceProxy)
	{
		connect(mSliceProxy.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(transformChangedSlot(Transform3D)));
		this->transformChangedSlot(mSliceProxy->get_sMr());
	}
}

SliceProxyPtr Slice3DProxy::getSliceProxy() const
{
	return mSliceProxy;
}

void Slice3DProxy::setImage(ImagePtr image)
{
	mSlicer->setImage(image);
}

void Slice3DProxy::transformChangedSlot(Transform3D sMr)
{
	Transform3D rMs = sMr.inv();
	mActor->SetUserMatrix(rMs.getVtkMatrix());
}


// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------


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

void Slices3DRep::setPatientModelService(PatientModelServicePtr dataManager)
{
	mDataManager = dataManager;
}

void Slices3DRep::addPlane(PLANE_TYPE plane)
{
	if (!mDataManager)
	{
		reportError("Slices3DRep::addPlane: call setPatientModelService() first.");
		return;
	}
	SliceProxyPtr sliceProxy = SliceProxy::create(mDataManager);
	sliceProxy->initializeFromPlane(plane, false, true, 150, 0.25);
	sliceProxy->setAlwaysUseDefaultCenter(true);

	Slice3DProxyPtr proxy = Slice3DProxy::New();
	proxy->setSliceProxy(sliceProxy);
	mProxies.push_back(proxy);
}

void Slices3DRep::setImage(ImagePtr image)
{
	if (!image)
	{
		reportWarning("Slices3DRep::setImage: No image");
		return;
	}
	if (mProxies.empty())
	{
		reportWarning("Slices3DRep::setImage: No planes added, call addPlane() first.");
		return;
	}

	Vector3D center_r = image->get_rMd().coord(image->boundingBox().center());
	for (unsigned i = 0; i < mProxies.size(); ++i)
	{
		mProxies[i]->getSliceProxy()->setDefaultCenter(center_r);
		mProxies[i]->setImage(image);
	}
}

void Slices3DRep::setTool(ToolPtr tool)
{
	for (unsigned i = 0; i < mProxies.size(); ++i)
		mProxies[i]->getSliceProxy()->setTool(tool);
}

void Slices3DRep::addRepActorsToViewRenderer(ViewPtr view)
{
	for (unsigned i = 0; i < mProxies.size(); ++i)
		view->getRenderer()->AddViewProp(mProxies[i]->getActor());
}

void Slices3DRep::removeRepActorsFromViewRenderer(ViewPtr view)
{
	for (unsigned i = 0; i < mProxies.size(); ++i)
		view->getRenderer()->RemoveViewProp(mProxies[i]->getActor());
}

} // namespace cx
