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


#include <vtkRenderer.h>
#include <vtkVolumeMapper.h>
#include <vtkImageData.h>

#include "cxStreamRep3D.h"
#include "cxTrackedStream.h"
#include "cxVideoSourceGraphics.h"
#include "cxView.h"
#include "cxTool.h"
#include "cxPatientModelService.h"
#include "cxVolumeProperty.h"

//Transfer function test code
#include "cxImageTF3D.h"


namespace cx
{

StreamRep3DPtr StreamRep3D::New(SpaceProviderPtr spaceProvider, PatientModelServicePtr patientModelService, const QString& uid)
{
	return wrap_new(new StreamRep3D(spaceProvider, patientModelService), uid);
}

StreamRep3D::StreamRep3D(SpaceProviderPtr spaceProvider, PatientModelServicePtr patientModelService) :
	VolumetricRep(),
	mTrackedStream(TrackedStreamPtr()),
	mPatientModelService(patientModelService)
{
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
//	mRTStream->setTool(tool);
}

void StreamRep3D::newVideoSource(VideoSourcePtr videoSource)
{
	if(!videoSource)
		return;

	ImagePtr image = mTrackedStream->createImage();//TODO: Implement transfer functions in TrackedStream
	this->setImage(image);
//	mPatientModelService->insertData(image);

	this->initTransferFunction(image);

	if(mVideoSource)
		disconnect(videoSource.get(), &VideoSource::newFrame, this, &StreamRep3D::newFrame);
	mVideoSource = videoSource;

	connect(videoSource.get(), &VideoSource::newFrame, this, &StreamRep3D::newFrame);
}

//Test: Set transfer function so that we can see the volume change
void StreamRep3D::initTransferFunction(ImagePtr image)
{
	ImageTF3DPtr tf3D = image->getTransferFunctions3D();
	IntIntMap opacity;
	opacity[90] = 0;
	opacity[100] = 5;
	opacity[200] = image->getMaxAlphaValue();
	tf3D->resetAlpha(opacity);
	image->setTransferFunctions3D(tf3D);
}

void StreamRep3D::newFrame()
{
	mImage->setVtkImageData(mVideoSource->getVtkImageData(), false);
	vtkImageDataPtr volume = mImage->resample(this->mMaxVoxels);

	mMapper->SetInputData(volume);
}

TrackedStreamPtr StreamRep3D::getTrackedStream()
{
	return mTrackedStream;
}

QString StreamRep3D::getType() const
{
	return "StreamRep3D";
}

} //cx
