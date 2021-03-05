/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxInteractiveClipper.h"

#include "cxSliceComputer.h"
#include "cxSlicePlaneClipper.h"
#include "cxSlicePlanes3DRep.h"
#include "cxSliceProxy.h"
#include "cxData.h"
#include "cxTrackingService.h"
#include "cxCoreServices.h"
#include "cxLogger.h"
#include "cxEnumConversion.h"
#include "cxPatientModelService.h"

namespace cx
{

InteractiveClipper::InteractiveClipper(CoreServicesPtr services) :
	mUseClipper(true),
	mServices(services),
	mUseActiveTool(true)
{

	// create a slice planes proxy containing all slice definitions,
	// for use with the clipper
	PatientModelServicePtr dm = mServices->patient();
	mSlicePlanesProxy = SlicePlanesProxyPtr(new SlicePlanesProxy());
	mSlicePlanesProxy->addSimpleSlicePlane(ptSAGITTAL, dm);
	mSlicePlanesProxy->addSimpleSlicePlane(ptCORONAL, dm);
	mSlicePlanesProxy->addSimpleSlicePlane(ptAXIAL, dm);
	mSlicePlanesProxy->addSimpleSlicePlane(ptANYPLANE, dm);
	mSlicePlanesProxy->addSimpleSlicePlane(ptSIDEPLANE, dm);
	mSlicePlanesProxy->addSimpleSlicePlane(ptRADIALPLANE, dm);
    mSlicePlanesProxy->addSimpleSlicePlane(ptTOOLSIDEPLANE, dm);

	mSlicePlaneClipper = SlicePlaneClipper::New();

	connect(mSlicePlaneClipper.get(), SIGNAL(slicePlaneChanged()), this, SLOT(changedSlot()));
	connect(this, SIGNAL(changed()), this, SLOT(changedSlot()));
	connect(mServices->tracking().get(), SIGNAL(activeToolChanged(const QString&)), this, SLOT(activeToolChangedSlot()));

	this->activeToolChangedSlot();
	this->changedSlot();
}

void InteractiveClipper::addXml(QDomNode& dataNode)
{
	QDomElement elem = dataNode.toElement();

	QString plane = enum2string<PLANE_TYPE>(this->getSlicePlane());

	elem.setAttribute("plane", plane);
	elem.setAttribute("invert", this->getInvertPlane());
	elem.setAttribute("uids", this->getDataUids());
	}

void InteractiveClipper::parseXml(QDomNode dataNode)
{
	QDomElement elem = dataNode.toElement();

	QString existingPlane = enum2string<PLANE_TYPE>(this->getSlicePlane());
	PLANE_TYPE newPlane = string2enum<PLANE_TYPE>(elem.attribute("plane", existingPlane));

	this->setSlicePlane(newPlane);
	this->invertPlane(elem.attribute("invert", QString::number(this->getInvertPlane())).toInt());
	this->setDataUids(elem.attribute("uids", this->getDataUids()));
}

QString InteractiveClipper::getDataUids()
{
	QStringList dataUids;
	std::map<QString, DataPtr>::iterator iter = mDatas.begin();
	for (; iter != mDatas.end(); ++iter)
	{
		dataUids << iter->first;
	}
	return dataUids.join(" ");
}

void InteractiveClipper::setDataUids(QString uids)
{
	QStringList dataUids = uids.split(" ");
	for(int i = 0; i < dataUids.size(); ++i)
	{
		DataPtr data = mServices->patient()->getData(dataUids.at(i));
		this->addData(data);
	}
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
	if (!mData)
		return;

	mData->addPersistentClipPlane(mSlicePlaneClipper->getClipPlaneCopy());
}
void InteractiveClipper::clearClipPlanesInVolume()
{
	if (!mData)
		return;
	mData->clearPersistentClipPlanes();
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
	this->updateClipPlanesInData();
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

DataPtr InteractiveClipper::getData() const
{
	return mData;
}

void InteractiveClipper::setData(DataPtr data)
{
	if (mData)
		mData->setInteractiveClipPlane(vtkPlanePtr());
	mData = data;
	emit changed();
}

void InteractiveClipper::addData(DataPtr data)
{
	if(!data)
		return;
	mDatas[data->getUid()] = data;
	this->updateClipPlanesInData();
}

void InteractiveClipper::removeData(DataPtr data)
{
	if(!data)
		return;
	std::map<QString, DataPtr>::iterator iter = mDatas.find(data->getUid());
	if(iter != mDatas.end())
	{
		iter->second->removeInteractiveClipPlane(mSlicePlaneClipper->getClipPlane());
		mDatas.erase(iter);
	}
	this->updateClipPlanesInData();
}

bool InteractiveClipper::exists(DataPtr data)
{
	if(!data)
		return false;
	return mDatas.count(data->getUid());
}

void InteractiveClipper::updateClipPlanesInData()
{
	if (mUseClipper)
		this->addAllInteractiveClipPlanes();
	else
		this->removeAllInterActiveClipPlanes();

	emit changed();
}

std::map<QString, DataPtr> InteractiveClipper::getDatas()
{
	return mDatas;
}

void InteractiveClipper::addAllInteractiveClipPlanes()
{
	std::map<QString, DataPtr>::iterator iter = mDatas.begin();
	for(; iter != mDatas.end(); ++iter)
		iter->second->addInteractiveClipPlane(mSlicePlaneClipper->getClipPlane());
}

void InteractiveClipper::removeAllInterActiveClipPlanes()
{
	std::map<QString, DataPtr>::iterator iter = mDatas.begin();
	for(; iter != mDatas.end(); ++iter)
		iter->second->removeInteractiveClipPlane(mSlicePlaneClipper->getClipPlane());
}

void InteractiveClipper::changedSlot()
{
	if (!mData)
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
		mData->setInteractiveClipPlane(mSlicePlaneClipper->getClipPlane());
	}
	else
	{
		mData->setInteractiveClipPlane(vtkPlanePtr());
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
	ToolPtr activeTool = mServices->tracking()->getActiveTool();

	if(mUseActiveTool)
		this->setTool(activeTool);
}

void InteractiveClipper::setTool(ToolPtr tool)
{
	SlicePlanesProxy::DataMap data = mSlicePlanesProxy->getData();
	for (SlicePlanesProxy::DataMap::iterator iter = data.begin(); iter != data.end(); ++iter)
	{
		iter->second.mSliceProxy->setTool(tool);
	}
}

void InteractiveClipper::useActiveTool(bool on)
{
	mUseActiveTool = on;
}

} // namespace cx
