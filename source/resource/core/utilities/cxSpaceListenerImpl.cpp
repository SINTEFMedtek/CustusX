/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxSpaceListenerImpl.h"

#include "cxPatientModelService.h"
#include "cxTrackingService.h"
#include "cxData.h"
#include "cxTool.h"
#include "cxActiveData.h"
#include "cxLogger.h"
#include "cxActiveToolProxy.h"

namespace cx
{

SpaceListenerImpl::SpaceListenerImpl(TrackingServicePtr trackingService, PatientModelServicePtr dataManager)
{
	mTrackingService = trackingService;
	mDataManager = dataManager;
}

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
		ActiveDataPtr activeData = mDataManager->getActiveData();
		if (!mSpace.mRefObject.isEmpty() && (mSpace.mRefObject == "active"))
			connect(activeData.get(), &ActiveData::activeDataChanged, this, &SpaceListenerImpl::reconnect);

		DataPtr data = mDataManager->getData(mSpace.mRefObject);
		// NOTE: metrics have no own space, and thus are ignored if connected to.
		//       this breaks a loop if e.g. a dist depends on a pt and the dist becomes active.
		if (data && !data->getSpace().isEmpty())
		{
			connect(data.get(), SIGNAL(transformChanged()), this, SIGNAL(changed()));
			connect(mDataManager.get(), SIGNAL(dataAddedOrRemoved()), this, SIGNAL(changed()));
		}
	}

	if (mSpace.mId == csSENSOR || mSpace.mId == csTOOL || mSpace.mId == csTOOL_OFFSET)
	{
		if (mSpace.mRefObject == "active")
		{
			mActiveTool = ActiveToolProxy::New(mTrackingService);
			connect(mActiveTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D,double)), this, SIGNAL(changed()));
			connect(mActiveTool.get(), SIGNAL(tooltipOffset(double)), this, SIGNAL(changed()));
		}
		else
		{
			ToolPtr tool = mTrackingService->getTool(mSpace.mRefObject);
			if (tool)
			{
				connect(tool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D,double)), this, SIGNAL(changed()));
				connect(tool.get(), SIGNAL(tooltipOffset(double)), this, SIGNAL(changed()));
			}
		}
		connect(mDataManager.get(), SIGNAL(rMprChanged()), this, SIGNAL(changed()));
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
		ActiveDataPtr activeData = mDataManager->getActiveData();
		if (mSpace.mRefObject == "active")
			disconnect(activeData.get(), &ActiveData::activeDataChanged, this, &SpaceListenerImpl::reconnect);

		DataPtr data = mDataManager->getData(mSpace.mRefObject);
		if (data)
		{
			disconnect(data.get(), SIGNAL(transformChanged()), this, SIGNAL(changed()));
			disconnect(mDataManager.get(), SIGNAL(dataAddedOrRemoved()), this, SIGNAL(changed()));
		}
	}

	if (mSpace.mId == csSENSOR || mSpace.mId == csTOOL || mSpace.mId == csTOOL_OFFSET)
	{
		if (mActiveTool)
		{
			disconnect(mActiveTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D,double)), this, SIGNAL(changed()));
			disconnect(mActiveTool.get(), SIGNAL(tooltipOffset(double)), this, SIGNAL(changed()));
			mActiveTool.reset();
		}
		else
		{
			ToolPtr tool = mTrackingService->getTool(mSpace.mRefObject);
			if (tool)
			{
				disconnect(tool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D,double)), this, SIGNAL(changed()));
				disconnect(tool.get(), SIGNAL(tooltipOffset(double)), this, SIGNAL(changed()));
			}
		}
		disconnect(mDataManager.get(), SIGNAL(rMprChanged()), this, SIGNAL(changed()));
	}

	if (mSpace.mId == csPATIENTREF)
	{
		disconnect(mDataManager.get(), SIGNAL(rMprChanged()), this, SIGNAL(changed()));
	}
}

} // namespace cx
