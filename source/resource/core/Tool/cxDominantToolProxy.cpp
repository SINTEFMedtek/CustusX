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

#include "cxDominantToolProxy.h"
#include "cxTrackingService.h"
#include "cxTool.h"

namespace cx
{

DominantToolProxy::DominantToolProxy(TrackingServicePtr trackingService) :
	mTrackingService(trackingService)
{
	connect(mTrackingService.get(), SIGNAL(dominantToolChanged(const QString&)), this,
					SLOT(dominantToolChangedSlot(const QString&)));
	connect(mTrackingService.get(), SIGNAL(dominantToolChanged(const QString&)), this,
					SIGNAL(dominantToolChanged(const QString&)));

	if (mTrackingService->getActiveTool())
		this->dominantToolChangedSlot(mTrackingService->getActiveTool()->getUid());
}

void DominantToolProxy::dominantToolChangedSlot(const QString& uid)
{
	if (mTool && mTool->getUid() == uid)
		return;

	if (mTool)
	{
		disconnect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this,
						SIGNAL(toolTransformAndTimestamp(Transform3D, double)));
		disconnect(mTool.get(), SIGNAL(toolVisible(bool)), this, SIGNAL(toolVisible(bool)));
		disconnect(mTool.get(), SIGNAL(tooltipOffset(double)), this, SIGNAL(tooltipOffset(double)));
		disconnect(mTool.get(), SIGNAL(toolProbeSector()), this, SIGNAL(toolProbeSector()));
		disconnect(mTool.get(), SIGNAL(tps(int)), this, SIGNAL(tps(int)));
	}

	mTool = mTrackingService->getActiveTool();

	if (mTool)
	{
		connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this,
						SIGNAL(toolTransformAndTimestamp(Transform3D, double)));
		connect(mTool.get(), SIGNAL(toolVisible(bool)), this, SIGNAL(toolVisible(bool)));
		connect(mTool.get(), SIGNAL(tooltipOffset(double)), this, SIGNAL(tooltipOffset(double)));
		connect(mTool.get(), SIGNAL(toolProbeSector()), this, SIGNAL(toolProbeSector()));
		connect(mTool.get(), SIGNAL(tps(int)), this, SIGNAL(tps(int)));

		emit toolVisible(mTool->getVisible());
		emit toolTransformAndTimestamp(mTool->get_prMt(), mTool->getTimestamp());
		emit tooltipOffset(mTool->getTooltipOffset());
	}
}

}
