// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

/*
 * sscSlicePlaneClipper.cpp
 *
 *  Created on: Aug 20, 2010
 *      Author: christiana
 */

#include "sscSlicePlaneClipper.h"

#include <vector>
#include <vtkPlane.h>
#include <vtkVolume.h>
#include <vtkAbstractVolumeMapper.h>
#include <vtkPlaneCollection.h>
#include <vtkVolumeMapper.h>

#include "sscSliceProxy.h"
#include "sscVolumetricRep.h"
#include "sscImage.h"
#include "sscDataManager.h"

namespace ssc
{

SlicePlaneClipperPtr SlicePlaneClipper::New()
{
	return SlicePlaneClipperPtr(new SlicePlaneClipper());
}

SlicePlaneClipper::SlicePlaneClipper() :
	mInvertPlane(false)
{
}

SlicePlaneClipper::~SlicePlaneClipper()
{
	this->clearVolumes();
}

void SlicePlaneClipper::setSlicer(ssc::SliceProxyPtr slicer)
{
	if (mSlicer==slicer)
		return;
	if (mSlicer)
	{
		disconnect(mSlicer.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(changedSlot()));
	}
	mSlicer = slicer;
	if (mSlicer)
	{
		connect(mSlicer.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(changedSlot()));
	}

	this->updateClipPlane();
	for (VolumesType::iterator iter=mVolumes.begin(); iter!=mVolumes.end(); ++iter)
	{
		this->addClipPlane(*iter, mClipPlane);
	}
	this->changedSlot();
}

void SlicePlaneClipper::addClipPlane(ssc::VolumetricBaseRepPtr volume, vtkPlanePtr clipPlane)
{
	if (!clipPlane)
		return;
	if (!volume->getVtkVolume())
		return;
	vtkAbstractVolumeMapper* mapper = volume->getVtkVolume()->GetMapper();
	if (mapper->GetClippingPlanes() && mapper->GetClippingPlanes()->IsItemPresent(clipPlane))
		return;
	mapper->AddClippingPlane(clipPlane);
}

ssc::SliceProxyPtr SlicePlaneClipper::getSlicer()
{
	return mSlicer;
}

void SlicePlaneClipper::clearVolumes()
{
	while(!mVolumes.empty())
	{
		this->removeVolume(*mVolumes.begin());
	}
}

void SlicePlaneClipper::volumeRepChangedSlot()
{
	VolumesType volumes = mVolumes;
	for (VolumesType::iterator iter=volumes.begin(); iter!=volumes.end(); ++iter)
	{
		this->removeVolume(*iter);
		this->addVolume(*iter);
	}
}

void SlicePlaneClipper::addVolume(ssc::VolumetricBaseRepPtr volume)
{
	if (!volume)
		return;
	mVolumes.insert(volume);
	connect(volume.get(), SIGNAL(internalVolumeChanged()), this, SLOT(volumeRepChangedSlot()));
	this->addClipPlane(volume, mClipPlane);
	this->changedSlot();
}

void SlicePlaneClipper::removeVolume(ssc::VolumetricBaseRepPtr volume)
{
	if (!volume)
		return;

	if (volume->getVtkVolume())
	{
		vtkAbstractMapper* mapper = volume->getVtkVolume()->GetMapper();
		if (mapper)
		{
			vtkPlaneCollection* planes = mapper->GetClippingPlanes();
			if (planes && planes->IsItemPresent(mClipPlane))
			{
				mapper->RemoveClippingPlane(mClipPlane);
			}
		}
	}

	disconnect(volume.get(), SIGNAL(internalVolumeChanged()), this, SLOT(volumeRepChangedSlot()));
	mVolumes.erase(volume);
	this->changedSlot();
}

SlicePlaneClipper::VolumesType SlicePlaneClipper::getVolumes()
{
	return mVolumes;
}

void SlicePlaneClipper::setInvertPlane(bool on)
{
	mInvertPlane = on;
	changedSlot();
}

bool SlicePlaneClipper::getInvertPlane() const
{
	return mInvertPlane;
}

/** return an untransformed plane normal to use during clipping.
 *  The direction is dependent in invertedPlane()
 */
ssc::Vector3D SlicePlaneClipper::getUnitNormal() const
{
	if (mInvertPlane)
		return ssc::Vector3D(0,0,1);
	else
		return ssc::Vector3D(0,0,-1);
}

/** return a vtkPlane representing the current clip plane.
 */
vtkPlanePtr SlicePlaneClipper::getClipPlaneCopy()
{
	vtkPlanePtr retval = vtkPlanePtr::New();
	retval->SetNormal(mClipPlane->GetNormal());
	retval->SetOrigin(mClipPlane->GetOrigin());
	return retval;
}

void SlicePlaneClipper::updateClipPlane()
{
	if (!mSlicer)
		return;
	if (!mClipPlane)
		mClipPlane = vtkPlanePtr::New();

	ssc::Transform3D rMs = mSlicer->get_sMr().inv();

	ssc::Vector3D n = rMs.vector(this->getUnitNormal());
	ssc::Vector3D p = rMs.coord(ssc::Vector3D(0,0,0));
	mClipPlane->SetNormal(n.begin());
	mClipPlane->SetOrigin(p.begin());
}

void SlicePlaneClipper::changedSlot()
{
	if (!mSlicer)
		return;

	this->updateClipPlane();
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------


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

	mPlanes = mImage->getClipPlanes();
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

	ssc::DoubleBoundingBox3D bb_d = mImage->getCroppingBox();

	mapper->SetCroppingRegionPlanes(bb_d.begin());
	mapper->Update();
}

} // namespace ssc
