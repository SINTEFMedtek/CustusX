/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxImageMapperMonitor.h"

#include <vector>
#include <vtkPlane.h>
#include <vtkVolume.h>
#include <vtkAbstractVolumeMapper.h>
#include <vtkPlaneCollection.h>
#include <vtkVolumeMapper.h>

#include "cxImage.h"


namespace cx
{

ImageMapperMonitorPtr ImageMapperMonitor::create(vtkVolumePtr volume, ImagePtr image)
{
	ImageMapperMonitorPtr retval(new ImageMapperMonitor(volume, image));
	retval->init(); // contains virtual functions
	return retval;
}

ImageMapperMonitor::ImageMapperMonitor(vtkVolumePtr volume, ImagePtr image) : mVolume(volume), mImage(image)
{
	if (!mImage)
		return;
	connect(mImage.get(), &Data::clipPlanesChanged, this, &ImageMapperMonitor::clipPlanesChangedSlot);
	connect(mImage.get(), &Image::cropBoxChanged, this, &ImageMapperMonitor::applyCropping);
}

void ImageMapperMonitor::init()
{
	this->applyClipping();
	this->applyCropping();
}

ImageMapperMonitor::~ImageMapperMonitor()
{
	this->clearClipping();
}

void ImageMapperMonitor::clipPlanesChangedSlot()
{
	this->applyClipping();
}

void ImageMapperMonitor::clearClipping()
{
	if (!mImage)
		return;

	for (unsigned i=0; i<mPlanes.size(); ++i)
	{
		mVolume->GetMapper()->RemoveClippingPlane(mPlanes[i]);
	}
	mPlanes.clear();
}

void ImageMapperMonitor::applyClipping()
{
	this->clearClipping();

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

void ImageMapperMonitor::applyCropping()
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
