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
#include "cxReporter.h"
#include "cxSettings.h"

//To be removed
#include "cxImageTF3D.h"
#include "cxRegistrationTransform.h"


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
	this->setVisualizerType();
}

void StreamRep3D::setTrackedStream(TrackedStreamPtr trackedStream)
{
	if(mTrackedStream)
	{
		disconnect(mTrackedStream.get(), &TrackedStream::newTool, this, &StreamRep3D::newTool);
		disconnect(mTrackedStream.get(), &TrackedStream::newVideoSource, this, &StreamRep3D::newVideoSource);
		disconnect(mTrackedStream.get(), &TrackedStream::newFrame, this, &StreamRep3D::vtkImageDataChangedSlot);
	}

	mTrackedStream = trackedStream;

	if(mTrackedStream)
	{
		connect(mTrackedStream.get(), &TrackedStream::newTool, this, &StreamRep3D::newTool);
		connect(mTrackedStream.get(), &TrackedStream::newVideoSource, this, &StreamRep3D::newVideoSource);
		connect(mTrackedStream.get(), &TrackedStream::newFrame, this, &StreamRep3D::vtkImageDataChangedSlot);
		this->newTool(mTrackedStream->getProbeTool());
		this->newVideoSource(mTrackedStream->getVideoSource());
	}
}

void StreamRep3D::newTool(ToolPtr tool)
{
}

void StreamRep3D::newVideoSource(VideoSourcePtr videoSource)
{
	if(!videoSource)
		return;

	ImagePtr image = mTrackedStream->getChangingImage();
	this->setImage(image);
//	mPatientModelService->insertData(image);

	this->initTransferFunction(image);

	mVideoSource = videoSource;
}

// Set transfer function so that we can see the volume change.
// Should be replaced with GUI for TrackedStream
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

TrackedStreamPtr StreamRep3D::getTrackedStream()
{
	return mTrackedStream;
}

QString StreamRep3D::getType() const
{
	return "StreamRep3D";
}

void StreamRep3D::setVisualizerType()
{
	QString visualizerType = settings()->value("View3D/ImageRender3DVisualizer").toString();
	if(visualizerType == "vtkVolumeTextureMapper3D")
		this->setUseVolumeTextureMapper();
	else if(visualizerType == "vtkGPUVolumeRayCastMapper")
		this->setUseGPUVolumeRayCastMapper();
	else
		reportError(QString("StreamRep3D::setVisualizerType(): No visualizer found for string=%1").arg(visualizerType));
}

} //cx
