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

#include "cxTrackedStream.h"

#include <vtkImageData.h>

#include "cxTool.h"
#include "cxRegistrationTransform.h"
#include "cxVideoSource.h"

#include "cxProbeSector.h"
#include "cxSpaceProvider.h"

namespace cx
{

TrackedStreamPtr TrackedStream::create(const QString &uid, const QString &name)
{
	return TrackedStreamPtr(new TrackedStream(uid, name, ToolPtr(), VideoSourcePtr()));
}

TrackedStream::TrackedStream(const QString& uid, const QString& name, const ToolPtr &probe, const VideoSourcePtr &videosource) :
	Data(uid, name), mProbeTool(probe), mVideoSource(VideoSourcePtr()),
	mImage(ImagePtr()),
	mSpaceProvider(SpaceProviderPtr())
{
	if(mProbeTool)
		emit newTool(mProbeTool);

	setVideoSource(videosource);
}

TrackedStream::~TrackedStream()
{
	if(mVideoSource)
	{
		disconnect(mVideoSource.get(), &VideoSource::newFrame, this, &TrackedStream::newFrame);
		disconnect(mVideoSource.get(), &VideoSource::streaming, this, &TrackedStream::streaming);
	}
}

void TrackedStream::setProbeTool(const ToolPtr &probeTool)
{
	if(mProbeTool)
		disconnect(mProbeTool.get(), &Tool::toolTransformAndTimestamp, this, &TrackedStream::toolTransformAndTimestamp);

	mProbeTool = probeTool;
	emit newTool(mProbeTool);

	if(mProbeTool)
		connect(mProbeTool.get(), &Tool::toolTransformAndTimestamp, this, &TrackedStream::toolTransformAndTimestamp);
}

void TrackedStream::toolTransformAndTimestamp(Transform3D prMt, double timestamp)
{
	//tMu calculation in ProbeSector differ from the one used here
//	Transform3D tMu = mProbeDefinition.get_tMu();
	Transform3D tMu = this->get_tMu();
	Transform3D rMpr = mSpaceProvider->get_rMpr();
	Transform3D rMu = rMpr * prMt * tMu;

	if (mImage)
		mImage->get_rMd_History()->setRegistration(rMu);
	emit newPosition();
}

Transform3D TrackedStream::get_tMu()
{
	//Made tMu by copying and modifying code from ProbeSector::get_tMu()
	ProbeDefinition probeDefinition = mProbeTool->getProbe()->getProbeDefinition();
	Vector3D origin_p = probeDefinition.getOrigin_p();
	Vector3D spacing = probeDefinition.getSpacing();
	Vector3D origin_u(origin_p[0]*spacing[0], origin_p[1]*spacing[1], origin_p[2]*spacing[2]);

	Transform3D Rx = createTransformRotateX(M_PI / 2.0);
	Transform3D Ry = createTransformRotateY(-M_PI / 2.0);
	Transform3D R = (Rx * Ry);
	Transform3D T = createTransformTranslate(-origin_u);

	Transform3D tMu = R * T;
	return tMu;
}

ToolPtr TrackedStream::getProbeTool()
{
	return mProbeTool;
}

void TrackedStream::setVideoSource(const VideoSourcePtr &videoSource)
{
	if(mVideoSource)
	{
		disconnect(mVideoSource.get(), &VideoSource::newFrame, this, &TrackedStream::newFrame);
		disconnect(mVideoSource.get(), &VideoSource::streaming, this, &TrackedStream::streaming);
	}

	mVideoSource = videoSource;
	emit streamChanged(this->getUid());
	emit newVideoSource(mVideoSource);

	if(mVideoSource)
	{
		connect(mVideoSource.get(), &VideoSource::newFrame, this, &TrackedStream::newFrameSlot);
		connect(mVideoSource.get(), &VideoSource::streaming, this, &TrackedStream::streaming);
	}
}

void TrackedStream::newFrameSlot()
{
	//TODO: Check if we need to turn this on/off
	if (mImage && mVideoSource)
	{
		mImage->setVtkImageData(mVideoSource->getVtkImageData(), false);
		emit newFrame();
	}
}

VideoSourcePtr TrackedStream::getVideoSource()
{
	return mVideoSource;
}

void TrackedStream::setSpaceProvider(SpaceProviderPtr spaceProvider)
{
	mSpaceProvider = spaceProvider;
}

void TrackedStream::addXml(QDomNode &dataNode)
{
	Data::addXml(dataNode);
}

void TrackedStream::parseXml(QDomNode &dataNode)
{
	Data::parseXml(dataNode);
}

DoubleBoundingBox3D TrackedStream::boundingBox() const
{
	DoubleBoundingBox3D bounds;
	if(this->hasVideo())
		bounds = DoubleBoundingBox3D(mVideoSource->getVtkImageData()->GetBounds());
	return bounds;
}

QString TrackedStream::getType() const
{
	return getTypeName();
}

QString TrackedStream::getTypeName()
{
	return "trackedStream";
}

ImagePtr TrackedStream::getChangingImage()
{
	if(!mVideoSource)
		return ImagePtr();
	if (!mImage)
		mImage = ImagePtr(new Image(this->getUid()+"_TrackedStreamHelper", mVideoSource->getVtkImageData(), this->getName()+"_TrackedStreamHelper"));
	return mImage;
}

bool TrackedStream::is3D()
{
	if(this->hasVideo() && ( mVideoSource->getVtkImageData()->GetDataDimension() == 3) )
		return true;
	else
		return false;
}

bool TrackedStream::is2D()
{
	if(this->hasVideo() && ( mVideoSource->getVtkImageData()->GetDataDimension() == 2) )
		return true;
	else
		return false;
}

bool TrackedStream::hasVideo() const
{
	if(!mVideoSource || !mVideoSource->getVtkImageData())
		return false;
	return true;
}

bool TrackedStream::isStreaming() const
{
	if (this->hasVideo())
		return mVideoSource->isStreaming();
	return false;
}

} //cx
