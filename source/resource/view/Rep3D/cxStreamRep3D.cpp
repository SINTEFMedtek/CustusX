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


//#include "boost/shared_ptr.hpp"
#include <vtkRenderer.h>

#include "cxStreamRep3D.h"
#include "cxTrackedStream.h"
//#include "cxVideoGraphics.h"
#include "cxVideoSourceGraphics.h"
#include "cxView.h"
#include "cxTool.h"


namespace cx
{

StreamRep3DPtr StreamRep3D::New(SpaceProviderPtr spaceProvider, const QString& uid)
{
	return wrap_new(new StreamRep3D(spaceProvider), uid);
}

StreamRep3D::StreamRep3D(SpaceProviderPtr spaceProvider) :
	RepImpl(),
	mTrackedStream(TrackedStreamPtr()),
	mSpaceProvider(spaceProvider)
{
//	mPipeline.reset(new VideoGraphics());
	mRTStream.reset(new VideoSourceGraphics(mSpaceProvider));
}

void StreamRep3D::addRepActorsToViewRenderer(ViewPtr view)
{
//	view->getRenderer()->AddActor(mPipeline->getActor());
	view->getRenderer()->AddActor(mRTStream->getActor());
}

void StreamRep3D::removeRepActorsFromViewRenderer(ViewPtr view)
{
//	view->getRenderer()->RemoveActor(mPipeline->getActor());
	view->getRenderer()->RemoveActor(mRTStream->getActor());
}

void StreamRep3D::setTrackedStream(TrackedStreamPtr trackedStream)
{
	mTrackedStream = trackedStream;

	connect(mTrackedStream.get(), &TrackedStream::newTool, this, &StreamRep3D::newTool);
	connect(mTrackedStream.get(), &TrackedStream::newVideoSource, this, &StreamRep3D::newVideoSource);

	this->newTool(mTrackedStream->getProbeTool());
	this->newVideoSource(mTrackedStream->getVideoSource());
}

void StreamRep3D::newTool(ToolPtr tool)
{
	mRTStream->setTool(tool);
//	if(tool)
//		connect(tool.get(), &Tool::toolTransformAndTimestamp, this, &StreamRep3D::newTransform);
}

void StreamRep3D::newVideoSource(VideoSourcePtr videoSource)
{
	if(videoSource)
	{
		mRTStream->setRealtimeStream(videoSource);
//		mPipeline->setInputVideo(videoSource->getVtkImageData());
//		connect(videoSource.get(), &VideoSource::newFrame, this, &StreamRep3D::newFrame);
	}
}

//void StreamRep3D::newFrame()
//{
//	//TODO: See VideoSourceGraphics::newDataSlot()
//	mPipeline->update();
//	mPipeline->setVisibility(true);
//}

TrackedStreamPtr StreamRep3D::getTrackedStream()
{
	return mTrackedStream;
}

QString StreamRep3D::getType() const
{
	return "StreamRep3D";
}

//void StreamRep3D::newTransform(Transform3D prMt, double timestamp)
//{
//	//TODO: Calculate correct transform. See VideoSourceGraphics::receiveTransforms
//	mPipeline->setActorUserMatrix(prMt.getVtkMatrix());
//}

} //cx
