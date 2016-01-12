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

#include "cxSpaceListenerImpl.h"

#include "cxPatientModelService.h"
#include "cxTrackingService.h"
#include "cxData.h"
#include "cxTool.h"
#include "cxActiveData.h"
#include "cxLogger.h"

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
		if (mSpace.mRefObject == "active")
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
		ToolPtr tool = mTrackingService->getTool(mSpace.mRefObject);
		if (tool)
		{
			connect(tool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D,double)), this, SIGNAL(changed()));
			connect(tool.get(), SIGNAL(tooltipOffset(double)), this, SIGNAL(changed()));

			if (mSpace.mRefObject == "active")
			{
//				connect(mTrackingService.get(), SIGNAL(activeToolChanged(const QString&)), this, SIGNAL(changed()));
				connect(mTrackingService.get(), SIGNAL(activeToolChanged(const QString&)), this, SLOT(reconnect()));
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
		ToolPtr tool = mTrackingService->getTool(mSpace.mRefObject);
		if (tool)
		{
			disconnect(tool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D,double)), this, SIGNAL(changed()));
			disconnect(tool.get(), SIGNAL(tooltipOffset(double)), this, SIGNAL(changed()));

			if (mSpace.mRefObject == "active")
			{
				disconnect(mTrackingService.get(), SIGNAL(activeToolChanged(const QString&)), this, SIGNAL(changed()));
				disconnect(mTrackingService.get(), SIGNAL(activeToolChanged(const QString&)), this, SLOT(reconnect()));
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
