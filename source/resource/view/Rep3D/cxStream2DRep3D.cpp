/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
