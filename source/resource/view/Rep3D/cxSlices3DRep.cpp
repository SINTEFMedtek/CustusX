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

void Slice3DProxy::setSliceProxy(SliceProxyInterfacePtr sliceProxy)
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

void Slices3DRep::addPlane(PLANE_TYPE plane, PatientModelServicePtr dataManager)
{
	SliceProxyPtr sliceProxy = SliceProxy::create(dataManager);
	sliceProxy->initializeFromPlane(plane, false, true, 150, 0.25);
	sliceProxy->setAlwaysUseDefaultCenter(true);
	mSliceProxies.push_back(sliceProxy);

	Slice3DProxyPtr proxy = Slice3DProxy::New();
	proxy->setSliceProxy(sliceProxy);
	mProxies.push_back(proxy);
}

void Slices3DRep::setImages(std::vector<ImagePtr> images)
{
	if (images.empty())
	{
		reportWarning("Slices3DRep::setImages: No images");
		return;
	}

	Vector3D center_r = images[0]->get_rMd().coord(images[0]->boundingBox().center());
	for (unsigned i = 0; i < mSliceProxies.size(); ++i)
		mSliceProxies[i]->setDefaultCenter(center_r);

	for (unsigned i = 0; i < mProxies.size(); ++i)
		mProxies[i]->setImage(images[0]);
}

void Slices3DRep::setTool(ToolPtr tool)
{
	for (unsigned i = 0; i < mSliceProxies.size(); ++i)
		mSliceProxies[i]->setTool(tool);
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
