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

#include "cxInteractiveClipper.h"

#include "cxVolumetricRep.h"

#include "cxSliceComputer.h"

#include "cxSlicePlaneClipper.h"
#include "cxSlicePlanes3DRep.h"
#include "cxSliceProxy.h"
#include "cxImage.h"
#include "cxTrackingService.h"

#include "cxCoreServices.h"


namespace cx
{

InteractiveClipper::InteractiveClipper(CoreServicesPtr backend) :
	mUseClipper(false),
	mBackend(backend)
{

	// create a slice planes proxy containing all slice definitions,
	// for use with the clipper
	PatientModelServicePtr dm = mBackend->patient();
	mSlicePlanesProxy = SlicePlanesProxyPtr(new SlicePlanesProxy());
	mSlicePlanesProxy->addSimpleSlicePlane(ptSAGITTAL, dm);
	mSlicePlanesProxy->addSimpleSlicePlane(ptCORONAL, dm);
	mSlicePlanesProxy->addSimpleSlicePlane(ptAXIAL, dm);
	mSlicePlanesProxy->addSimpleSlicePlane(ptANYPLANE, dm);
	mSlicePlanesProxy->addSimpleSlicePlane(ptSIDEPLANE, dm);
	mSlicePlanesProxy->addSimpleSlicePlane(ptRADIALPLANE, dm);

	mSlicePlaneClipper = SlicePlaneClipper::New();

	connect(mSlicePlaneClipper.get(), SIGNAL(slicePlaneChanged()), this, SLOT(changedSlot()));
	connect(this, SIGNAL(changed()), this, SLOT(changedSlot()));
	connect(mBackend->tracking().get(), SIGNAL(activeToolChanged(const QString&)), this, SLOT(activeToolChangedSlot()));

	this->activeToolChangedSlot();
	this->changedSlot();
}


void InteractiveClipper::setSlicePlane(PLANE_TYPE plane)
{
	if (mSlicePlaneClipper->getSlicer() && mSlicePlaneClipper->getSlicer()->getComputer().getPlaneType() == plane)
		return;

	if (mSlicePlanesProxy->getData().count(plane))
	{
		mSlicePlaneClipper->setSlicer(mSlicePlanesProxy->getData()[plane].mSliceProxy);
		emit changed();
	}
}

void InteractiveClipper::saveClipPlaneToVolume()
{
	if (!mImage)
		return;

	mImage->addPersistentClipPlane(mSlicePlaneClipper->getClipPlaneCopy());
}
void InteractiveClipper::clearClipPlanesInVolume()
{
	if (!mImage)
		return;
	mImage->clearPersistentClipPlanes();
}

PLANE_TYPE InteractiveClipper::getSlicePlane()
{
	if (!mSlicePlaneClipper->getSlicer())
		return ptCOUNT;
	return mSlicePlaneClipper->getSlicer()->getComputer().getPlaneType();
}

bool InteractiveClipper::getUseClipper() const
{
	return mUseClipper;
}
bool InteractiveClipper::getInvertPlane() const
{
	return mSlicePlaneClipper->getInvertPlane();
}
void InteractiveClipper::useClipper(bool on)
{
	mUseClipper = on;
	emit changed();
}
void InteractiveClipper::invertPlane(bool on)
{
	mSlicePlaneClipper->setInvertPlane(on);
	emit changed();
}

PLANE_TYPE InteractiveClipper::getPlaneType()
{
	if (!mSlicePlaneClipper->getSlicer())
		return ptCOUNT;
	return mSlicePlaneClipper->getSlicer()->getComputer().getPlaneType();
}

ImagePtr InteractiveClipper::getImage() const
{
	return mImage;
}

void InteractiveClipper::setImage(ImagePtr image)
{
	if (mImage)
		mImage->setInteractiveClipPlane(vtkPlanePtr());
	mImage = image;
	emit changed();
}

void InteractiveClipper::changedSlot()
{
	if (!mImage)
		return;

	if (mUseClipper)
	{
		PLANE_TYPE currentPlane = this->getPlaneType();

		std::vector<PLANE_TYPE> planes = this->getAvailableSlicePlanes();

		if (!std::count(planes.begin(), planes.end(), currentPlane)) //if (this->getPlaneType()==ptCOUNT)
		{
			if (planes.empty()) // no slices: remove clipping
			{
				currentPlane = ptCOUNT;
			}
			else
			{
				currentPlane = planes.front();
			}
		}

		// reset plane anyway. It might be the same planeType but a different sliceProxy.
		mSlicePlaneClipper->setSlicer(mSlicePlanesProxy->getData()[currentPlane].mSliceProxy);
		mImage->setInteractiveClipPlane(mSlicePlaneClipper->getClipPlane());
	}
	else
	{
		mImage->setInteractiveClipPlane(vtkPlanePtr());
	}

}

std::vector<PLANE_TYPE> InteractiveClipper::getAvailableSlicePlanes() const
{
	std::vector<PLANE_TYPE> retval;
	SlicePlanesProxy::DataMap data = mSlicePlanesProxy->getData();
	for (SlicePlanesProxy::DataMap::iterator iter = data.begin(); iter != data.end(); ++iter)
	{
		retval.push_back(iter->first);
	}
	return retval;
}

void InteractiveClipper::activeToolChangedSlot()
{
	ToolPtr activTool = mBackend->tracking()->getActiveTool();

	SlicePlanesProxy::DataMap data = mSlicePlanesProxy->getData();
	for (SlicePlanesProxy::DataMap::iterator iter = data.begin(); iter != data.end(); ++iter)
	{
		iter->second.mSliceProxy->setTool(activTool);
	}
}

} // namespace cx
