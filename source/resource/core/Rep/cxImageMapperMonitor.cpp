// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxImageMapperMonitor.h"

#include <vector>
#include <vtkPlane.h>
#include <vtkVolume.h>
#include <vtkAbstractVolumeMapper.h>
#include <vtkPlaneCollection.h>
#include <vtkVolumeMapper.h>

#include "sscImage.h"
#include "sscDataManager.h"

namespace cx
{

ImageMapperMonitor::ImageMapperMonitor(vtkVolumePtr volume, ImagePtr image) : mVolume(volume), mImage(image)
{
	if (!mImage)
		return;

	connect(mImage.get(), SIGNAL(clipPlanesChanged()), this, SLOT(clipPlanesChangedSlot()));
	connect(mImage.get(), SIGNAL(cropBoxChanged()), this, SLOT(cropBoxChangedSlot()));
	this->fillClipPlanes();
	this->cropBoxChangedSlot();
}

ImageMapperMonitor::~ImageMapperMonitor()
{
	this->clearClipPlanes();
}

void ImageMapperMonitor::clipPlanesChangedSlot()
{
	this->clearClipPlanes();
	this->fillClipPlanes();
}

void ImageMapperMonitor::clearClipPlanes()
{
	if (!mImage)
		return;

	for (unsigned i=0; i<mPlanes.size(); ++i)
	{
		mVolume->GetMapper()->RemoveClippingPlane(mPlanes[i]);
	}
	mPlanes.clear();
}

void ImageMapperMonitor::fillClipPlanes()
{
	if (!mImage)
		return;

	mPlanes = mImage->getAllClipPlanes();
	for (unsigned i=0; i<mPlanes.size(); ++i)
	{
		mVolume->GetMapper()->AddClippingPlane(mPlanes[i]);
	}
}

vtkVolumeMapperPtr ImageMapperMonitor::getMapper()
{
	vtkVolumeMapperPtr mapper = dynamic_cast<vtkVolumeMapper*>(mVolume->GetMapper());
	return mapper;
}

void ImageMapperMonitor::cropBoxChangedSlot()
{
	if (!mImage)
		return;

	vtkVolumeMapperPtr mapper = this->getMapper();
	if (!mapper)
		return;
	mapper->SetCropping(mImage->getCropping());

	DoubleBoundingBox3D bb_d = mImage->getCroppingBox();

	mapper->SetCroppingRegionPlanes(bb_d.begin());
	mapper->Update();
}

} // namespace cx
