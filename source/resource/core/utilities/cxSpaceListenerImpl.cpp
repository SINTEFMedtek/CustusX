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

#include "cxSpaceListenerImpl.h"

#include "cxDataManager.h"
#include "cxToolManager.h"
#include "cxData.h"

namespace cx
{

SpaceListenerImpl::SpaceListenerImpl(TrackingServicePtr toolManager, DataServicePtr dataManager)
{
	mToolManager = toolManager;
	mDataManager = dataManager;
}

//SpaceListenerImpl::SpaceListenerImpl(CoordinateSystem space) :
//	mSpace(space)
//{
//	this->doConnect();
//}

SpaceListenerImpl::~SpaceListenerImpl()
{
}

void SpaceListenerImpl::setSpace(CoordinateSystem space)
{
	this->doDisconnect();
	mSpace = space;
	this->doConnect();
	emit changed();
}

void SpaceListenerImpl::reconnect()
{
	this->doDisconnect();
	this->doConnect();
	emit changed();
}

CoordinateSystem SpaceListenerImpl::getSpace() const
{
	return mSpace;
}

void SpaceListenerImpl::doConnect()
{
	if (mSpace.mId == csDATA)
	{
		DataPtr data = mDataManager->getData(mSpace.mRefObject);
		if (data)
		{
			connect(data.get(), SIGNAL(transformChanged()), this, SIGNAL(changed()));
			connect(mDataManager.get(), SIGNAL(dataAddedOrRemoved(QString)), this, SIGNAL(changed()));
		}
	}

	if (mSpace.mId == csSENSOR || mSpace.mId == csTOOL || mSpace.mId == csTOOL_OFFSET)
	{
		ToolPtr tool = mToolManager->getTool(mSpace.mRefObject);
		if (tool)
		{
			connect(tool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D,double)), this, SIGNAL(changed()));
			connect(tool.get(), SIGNAL(tooltipOffset(double)), this, SIGNAL(changed()));

			if (mSpace.mRefObject == "active")
			{
				connect(mToolManager.get(), SIGNAL(dominantToolChanged(const QString&)), this, SIGNAL(changed()));
				connect(mToolManager.get(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(reconnect()));
			}
			connect(mDataManager.get(), SIGNAL(rMprChanged()), this, SIGNAL(changed()));
		}
	}

	if (mSpace.mId == csPATIENTREF)
	{
		connect(mDataManager.get(), SIGNAL(rMprChanged()), this, SIGNAL(changed()));
	}
}

void SpaceListenerImpl::doDisconnect()
{
	if (mSpace.mId == csDATA)
	{
		DataPtr data = mDataManager->getData(mSpace.mRefObject);
		if (data)
		{
			disconnect(data.get(), SIGNAL(transformChanged()), this, SIGNAL(changed()));
			disconnect(mDataManager.get(), SIGNAL(dataAddedOrRemoved(QString)), this, SIGNAL(changed()));
		}
	}

	if (mSpace.mId == csSENSOR || mSpace.mId == csTOOL || mSpace.mId == csTOOL_OFFSET)
	{
		ToolPtr tool = mToolManager->getTool(mSpace.mRefObject);
		if (tool)
		{
			disconnect(tool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D,double)), this, SIGNAL(changed()));
			disconnect(tool.get(), SIGNAL(tooltipOffset(double)), this, SIGNAL(changed()));

			if (mSpace.mRefObject == "active")
			{
				disconnect(mToolManager.get(), SIGNAL(dominantToolChanged(const QString&)), this, SIGNAL(changed()));
				disconnect(mToolManager.get(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(reconnect()));
			}
			disconnect(mDataManager.get(), SIGNAL(rMprChanged()), this, SIGNAL(changed()));
		}
	}

	if (mSpace.mId == csPATIENTREF)
	{
		disconnect(mDataManager.get(), SIGNAL(rMprChanged()), this, SIGNAL(changed()));
	}
}

} // namespace cx
