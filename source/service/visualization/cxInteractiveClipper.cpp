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

#include "sscVolumetricRep.h"
#include "sscMessageManager.h"
#include "cxRepManager.h"
#include "sscDataManager.h"
#include "sscSliceComputer.h"

#include "sscSlicePlaneClipper.h"
#include "sscSlicePlanes3DRep.h"
#include "sscSliceProxy.h"
#include "sscImage.h"
#include "sscToolManager.h"

namespace cx
{

//InteractiveClipper::InteractiveClipper(ssc::SlicePlanesProxyPtr slicePlanesProxy) :
//	mSlicePlanesProxy(slicePlanesProxy), mUseClipper(false)
//{
//	mSlicePlaneClipper = ssc::SlicePlaneClipper::New();
//	connect(this, SIGNAL(changed()), this, SLOT(changedSlot()));
////	connect(ssc::dataManager(), SIGNAL(activeImageChanged(QString)), this, SIGNAL(changed()));
//
//	this->changedSlot();
//}

InteractiveClipper::InteractiveClipper() :
	mUseClipper(false)
{

	// create a slice planes proxy containing all slice definitions,
	// for use with the clipper
	mSlicePlanesProxy = ssc::SlicePlanesProxyPtr(new ssc::SlicePlanesProxy());
	mSlicePlanesProxy->addSimpleSlicePlane(ssc::ptSAGITTAL);
	mSlicePlanesProxy->addSimpleSlicePlane(ssc::ptCORONAL);
	mSlicePlanesProxy->addSimpleSlicePlane(ssc::ptAXIAL);
	mSlicePlanesProxy->addSimpleSlicePlane(ssc::ptANYPLANE);
	mSlicePlanesProxy->addSimpleSlicePlane(ssc::ptSIDEPLANE);
	mSlicePlanesProxy->addSimpleSlicePlane(ssc::ptRADIALPLANE);

	mSlicePlaneClipper = ssc::SlicePlaneClipper::New();
	connect(this, SIGNAL(changed()), this, SLOT(changedSlot()));
//	connect(ssc::dataManager(), SIGNAL(activeImageChanged(QString)), this, SIGNAL(changed()));
	connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot()));

	this->dominantToolChangedSlot();
	this->changedSlot();
}


void InteractiveClipper::setSlicePlane(ssc::PLANE_TYPE plane)
{
	if (mSlicePlaneClipper->getSlicer() && mSlicePlaneClipper->getSlicer()->getComputer().getPlaneType() == plane)
		return;

	if (mSlicePlanesProxy->getData().count(plane))
	{
		//    std::cout << "InteractiveClipper::setSlicePlane(" << plane << ")" << std::endl;
		mSlicePlaneClipper->setSlicer(mSlicePlanesProxy->getData()[plane].mSliceProxy);
		emit changed();
	}
}

void InteractiveClipper::saveClipPlaneToVolume()
{
//	ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
	if (!mImage)
		return;

	mImage->addClipPlane(mSlicePlaneClipper->getClipPlaneCopy());
}
void InteractiveClipper::clearClipPlanesInVolume()
{
//	ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
	if (!mImage)
		return;
	mImage->clearClipPlanes();
}

ssc::PLANE_TYPE InteractiveClipper::getSlicePlane()
{
	if (!mSlicePlaneClipper->getSlicer())
		return ssc::ptCOUNT;
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

ssc::PLANE_TYPE InteractiveClipper::getPlaneType()
{
	if (!mSlicePlaneClipper->getSlicer())
		return ssc::ptCOUNT;
	return mSlicePlaneClipper->getSlicer()->getComputer().getPlaneType();
}

ssc::ImagePtr InteractiveClipper::getImage() const
{
	return mImage;
}

void InteractiveClipper::setImage(ssc::ImagePtr image)
{
//	std::cout << "InteractiveClipper::setImage " << image.get() << std::endl;
	mImage = image;
//	this->changedSlot(); // auto by changed()
	emit changed();
}

void InteractiveClipper::changedSlot()
{
	//  std::cout << "InteractiveClipper::changedSlot()" << std::endl;

	if (mUseClipper)
	{
		mSlicePlaneClipper->clearVolumes();
		mSlicePlaneClipper->addVolume(RepManager::getInstance()->getVolumetricRep(mImage));

		ssc::PLANE_TYPE currentPlane = this->getPlaneType();

		std::vector<ssc::PLANE_TYPE> planes = this->getAvailableSlicePlanes();

		if (!std::count(planes.begin(), planes.end(), currentPlane)) //if (this->getPlaneType()==ssc::ptCOUNT)
		{
			if (planes.empty()) // no slices: remove clipping
			{
				currentPlane = ssc::ptCOUNT;
			}
			else
			{
				currentPlane = planes.front();
			}
			//      std::cout << "init slicer(" << currentPlane << ")" << std::endl;
		}

		// reset plane anyway. It might be the same planeType but a different sliceProxy.
		mSlicePlaneClipper->setSlicer(mSlicePlanesProxy->getData()[currentPlane].mSliceProxy);
	}
	else
	{
		mSlicePlaneClipper->clearVolumes();
	}
}

std::vector<ssc::PLANE_TYPE> InteractiveClipper::getAvailableSlicePlanes() const
{
	//  std::cout << "InteractiveClipper::getAvailableSlicePlanes() " << mSlicePlanesProxy->getData().size() << std::endl;

	std::vector<ssc::PLANE_TYPE> retval;
	ssc::SlicePlanesProxy::DataMap data = mSlicePlanesProxy->getData();
	for (ssc::SlicePlanesProxy::DataMap::iterator iter = data.begin(); iter != data.end(); ++iter)
	{
		retval.push_back(iter->first);
	}
	return retval;
}

void InteractiveClipper::dominantToolChangedSlot()
{
	ssc::ToolPtr dominantTool = ssc::toolManager()->getDominantTool();

	ssc::SlicePlanesProxy::DataMap data = mSlicePlanesProxy->getData();
	for (ssc::SlicePlanesProxy::DataMap::iterator iter = data.begin(); iter != data.end(); ++iter)
	{
		iter->second.mSliceProxy->setTool(dominantTool);
	}
}

} // namespace cx
