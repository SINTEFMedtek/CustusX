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


#include "cxVideoSourceGraphics.h"

#include <vtkImageData.h>
#include <vtkPolyData.h>


#include "cxTool.h"
#include "cxRegistrationTransform.h"
#include "cxVideoGraphics.h"
#include "cxVideoSource.h"
#include "cxSpaceProvider.h"

namespace cx
{

VideoSourceGraphics::VideoSourceGraphics(SpaceProviderPtr spaceProvider, bool useMaskFilter)
{
	mSpaceProvider = spaceProvider;
	mClipToSector = true;
	mPipeline.reset(new VideoGraphics());
	mShowInToolSpace = true;
}

VideoSourceGraphics::~VideoSourceGraphics()
{
}

void VideoSourceGraphics::setShowInToolSpace(bool on)
{
	mShowInToolSpace = on;
}

vtkActorPtr VideoSourceGraphics::getActor()
{
	return mPipeline->getActor();
}

ToolPtr VideoSourceGraphics::getTool()
{
	return mTool;
}

ProbeSector VideoSourceGraphics::getProbeDefinition()
{
	return mProbeDefinition;
}

void VideoSourceGraphics::setTool(ToolPtr tool)
{
	if (tool==mTool)
		return;

	if (mTool)
	{
		disconnect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(receiveTransforms(Transform3D, double)));
		disconnect(mTool.get(), SIGNAL(toolProbeSector()), this, SLOT(probeSectorChanged()));
	}

	// accept only tool with a probe sector
	if (tool && tool->getProbe())
	{
		mTool = tool;
	}

	// setup new
	if (mTool )
	{
		connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(receiveTransforms(Transform3D, double)));
		connect(mTool.get(), SIGNAL(toolProbeSector()), this, SLOT(probeSectorChanged()));
	}

	this->probeSectorChanged();
}

void VideoSourceGraphics::setClipToSector(bool on)
{
	mClipToSector = on;
	this->probeSectorChanged();
}

void VideoSourceGraphics::probeSectorChanged()
{
	if (!mTool || !mTool->getProbe())
		return;

	mProbeDefinition.setData(mTool->getProbe()->getProbeDefinition());
	if (mClipToSector)
	{
		mPipeline->setClip(mProbeDefinition.getSector());
	}
	else
	{
		mPipeline->setClip(NULL);
	}
	this->receiveTransforms(mTool->get_prMt(), 0);

	mPipeline->update();
}


void VideoSourceGraphics::setRealtimeStream(VideoSourcePtr data)
{
	//Don't do anything if data is unchanged
	if (mData == data)
		return;
	if (mData)
	{
		disconnect(mData.get(), &VideoSource::newFrame, this, &VideoSourceGraphics::newDataSlot);
		mPipeline->setInputVideo(NULL);
	}

	mData = data;

	if (mData)
	{
		connect(mData.get(), &VideoSource::newFrame, this, &VideoSourceGraphics::newDataSlot);
		mPipeline->setInputVideo(mData->getVtkImageData());
	}

	this->newDataSlot();
}

void VideoSourceGraphics::receiveTransforms(Transform3D prMt, double timestamp)
{
	if (!mShowInToolSpace)
		return;
	Transform3D rMpr = mSpaceProvider->get_rMpr();
	Transform3D tMu = mProbeDefinition.get_tMu();
	Transform3D rMu = rMpr * prMt * tMu;
	mPipeline->setActorUserMatrix(rMu.getVtkMatrix());
}

void VideoSourceGraphics::newDataSlot()
{
	if (!mData || !mData->validData())
	{
		mPipeline->setVisibility(false);
		emit newData();
		return;
	}

	mPipeline->update();

	bool visible = mData->validData();
	if (mShowInToolSpace)
		visible = visible && mTool && mTool->getVisible();
	mPipeline->setVisibility(visible);

	emit newData();
}

} // namespace cx
