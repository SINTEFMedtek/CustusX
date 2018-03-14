/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxVideoSourceGraphics.h"

#include <vtkImageData.h>
#include <vtkPolyData.h>


#include "cxTool.h"
#include "cxRegistrationTransform.h"
#include "cxVideoGraphics.h"
#include "cxVideoSource.h"
#include "cxSpaceProvider.h"
#include "cxLogger.h"

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
