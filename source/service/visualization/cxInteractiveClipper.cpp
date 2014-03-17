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

#include "cxInteractiveClipper.h"

#include "cxVolumetricRep.h"
#include "cxReporter.h"
#include "cxRepManager.h"
#include "cxDataManager.h"
#include "cxSliceComputer.h"

#include "cxSlicePlaneClipper.h"
#include "cxSlicePlanes3DRep.h"
#include "cxSliceProxy.h"
#include "cxImage.h"
#include "cxToolManager.h"
#include "cxLogger.h"
#include "cxVisualizationServiceBackend.h"


namespace cx
{

InteractiveClipper::InteractiveClipper(VisualizationServiceBackendPtr backend) :
	mUseClipper(false),
	mBackend(backend)
{

	// create a slice planes proxy containing all slice definitions,
	// for use with the clipper
	DataServicePtr dm = mBackend->getDataManager();
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
	connect(mBackend->getToolManager().get(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot()));

	this->dominantToolChangedSlot();
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

void InteractiveClipper::dominantToolChangedSlot()
{
	ToolPtr dominantTool = mBackend->getToolManager()->getDominantTool();

	SlicePlanesProxy::DataMap data = mSlicePlanesProxy->getData();
	for (SlicePlanesProxy::DataMap::iterator iter = data.begin(); iter != data.end(); ++iter)
	{
		iter->second.mSliceProxy->setTool(dominantTool);
	}
}

} // namespace cx
