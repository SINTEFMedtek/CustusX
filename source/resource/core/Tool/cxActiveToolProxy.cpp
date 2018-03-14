/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxActiveToolProxy.h"
#include "cxTrackingService.h"
#include "cxTool.h"

namespace cx
{

ActiveToolProxy::ActiveToolProxy(TrackingServicePtr trackingService) :
	mTrackingService(trackingService)
{
	connect(mTrackingService.get(), &TrackingService::activeToolChanged, this, &ActiveToolProxy::activeToolChangedSlot);
	connect(mTrackingService.get(), &TrackingService::stateChanged, this, &ActiveToolProxy::trackingStateChanged);

	if (mTrackingService->getActiveTool())
		this->activeToolChangedSlot(mTrackingService->getActiveTool()->getUid());
}

void ActiveToolProxy::trackingStateChanged()
{
	if (mTrackingService->getState() != Tool::tsTRACKING)
		emit tps(0);
}

void ActiveToolProxy::activeToolChangedSlot(const QString& uid)
{
	if (mTool && mTool->getUid() == uid)
		return;

	if (mTool)
	{
		disconnect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this,
						SIGNAL(toolTransformAndTimestamp(Transform3D, double)));
		disconnect(mTool.get(), &Tool::toolVisible, this, &ActiveToolProxy::toolVisible);
		disconnect(mTool.get(), &Tool::tooltipOffset, this, &ActiveToolProxy::tooltipOffset);
		disconnect(mTool.get(), &Tool::toolProbeSector, this, &ActiveToolProxy::toolProbeSector);
		disconnect(mTool.get(), &Tool::tps, this, &ActiveToolProxy::tps);

		emit tps(0);
	}

	mTool = mTrackingService->getActiveTool();

	if (mTool)
	{
		connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this,
						SIGNAL(toolTransformAndTimestamp(Transform3D, double)));
		connect(mTool.get(), &Tool::toolVisible, this, &ActiveToolProxy::toolVisible);
		connect(mTool.get(), &Tool::tooltipOffset, this, &ActiveToolProxy::tooltipOffset);
		connect(mTool.get(), &Tool::toolProbeSector, this, &ActiveToolProxy::toolProbeSector);
		connect(mTool.get(), &Tool::tps, this, &ActiveToolProxy::tps);

		emit activeToolChanged(mTool->getUid());
		emit toolVisible(mTool->getVisible());
		emit toolTransformAndTimestamp(mTool->get_prMt(), mTool->getTimestamp());
		emit tooltipOffset(mTool->getTooltipOffset());
	}
}

}
