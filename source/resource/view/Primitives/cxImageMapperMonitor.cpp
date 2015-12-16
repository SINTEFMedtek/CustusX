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
