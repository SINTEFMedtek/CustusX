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

#include "cxStream2DRep3D.h"

#include <vtkRenderer.h>

#include "cxTrackedStream.h"
#include "cxTool.h"
#include "cxVideoSourceGraphics.h"
#include "cxView.h"
#include "cxLogger.h"

namespace cx
{

Stream2DRep3DPtr Stream2DRep3D::New(SpaceProviderPtr spaceProvider, const QString& uid)
{
	return wrap_new(new Stream2DRep3D(spaceProvider), uid);
}

Stream2DRep3D::Stream2DRep3D(SpaceProviderPtr spaceProvider) :
	RepImpl(),
	mSpaceProvider(spaceProvider)
{
	bool useMask = true;
	mRTStream.reset(new VideoSourceGraphics(mSpaceProvider, useMask));
}

QString Stream2DRep3D::getType() const
{
	return "Stream2DRep3D";
}

void Stream2DRep3D::setTrackedStream(TrackedStreamPtr trackedStream)
{
	if(mTrackedStream)
	{
		disconnect(mTrackedStream.get(), &TrackedStream::newTool, this, &Stream2DRep3D::trackedStreamChanged);
		disconnect(mTrackedStream.get(), &TrackedStream::newVideoSource, this, &Stream2DRep3D::trackedStreamChanged);
	}

	mTrackedStream = trackedStream;

	if(mTrackedStream)
	{
		connect(mTrackedStream.get(), &TrackedStream::newTool, this, &Stream2DRep3D::trackedStreamChanged);
		connect(mTrackedStream.get(), &TrackedStream::newVideoSource, this, &Stream2DRep3D::trackedStreamChanged);
	}
	this->trackedStreamChanged();
}

void Stream2DRep3D::addRepActorsToViewRenderer(ViewPtr view)
{
	view->getRenderer()->AddActor(mRTStream->getActor());
}

void Stream2DRep3D::removeRepActorsFromViewRenderer(ViewPtr view)
{
	view->getRenderer()->RemoveActor(mRTStream->getActor());
}

void Stream2DRep3D::trackedStreamChanged()
{
	ToolPtr tool = mTrackedStream->getProbeTool();
	mRTStream->setTool(tool);
	mRTStream->setRealtimeStream(mTrackedStream->getVideoSource());
}

bool Stream2DRep3D::isReady()
{
	if(!mTrackedStream->getProbeTool())
		return false;
	if(!mTrackedStream->getVideoSource())
		return false;
	return true;
}

} //cx
