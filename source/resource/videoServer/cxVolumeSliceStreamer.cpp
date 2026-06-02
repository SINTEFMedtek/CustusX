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

#include "cxVolumeSliceStreamer.h"

#include "vtkImageReslice.h"
#include "vtkMatrix4x4.h"
#include "vtkImageData.h"
#include "vtkImageMask.h"

#include "cxReporter.h"
#include "cxSliceProxy.h"
#include "cxProbeSector.h"
#include "cxProbeDefinition.h"
#include "cxTransform3D.h"
#include "cxVolumeHelpers.h"
#include "cxSlicedImageProxy.h"
#include "cxLogger.h"
#include "cxTypeConversions.h"
#include "cxPatientModelService.h"
#include "cxTool.h"
#include "cxSender.h"

namespace cx
{

VolumeSliceStreamer::VolumeSliceStreamer(PatientModelServicePtr patientModelService) :
	mTimer(new CyclicActionLogger()),
	mPatientModelService(patientModelService)
{
	this->setSendInterval(40);
}

VolumeSliceStreamer::~VolumeSliceStreamer()
{}

bool VolumeSliceStreamer::initialize(ImagePtr image, ToolPtr tool)
{
	this->setSourceImage(image);
	return initialize(tool);
}

bool VolumeSliceStreamer::initialize(ToolPtr tool)
{
	if(!mSourceImage || !tool)
	{
		this->setInitialized(false);
		return false;
	}
	this->createSendTimer();

	mTool = tool;
	connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(sliceSlot()));
	connect(mTool->getProbe().get(), SIGNAL(activeConfigChanged()), this, SLOT(resetMask()));

	this->resetMask();
	this->setInitialized(true);
	return true;
}

void VolumeSliceStreamer::startStreaming(SenderPtr sender)
{
	if (!this->isInitialized())
	{
		reportError("VolumeSliceStreamer: Failed to start streaming: Not initialized.");
		return;
	}
	mSender = sender;
	mSendTimer->start(this->getSendInterval());
}

void VolumeSliceStreamer::stopStreaming()
{
	if(mSendTimer)
		mSendTimer->stop();
}

bool VolumeSliceStreamer::isStreaming()
{
	return this->isInitialized();
}

void VolumeSliceStreamer::streamSlot()
{
	PackagePtr package(new Package());
	package->mImage = this->getSlice();
	mSender->send(package);
}

ImagePtr VolumeSliceStreamer::getSlice()
{
	if(!mTool || !mSourceImage)
		return ImagePtr();

	if (!mCachedImageToSend)
		mCachedImageToSend = this->calculateSlice(mSourceImage);

	return mCachedImageToSend;
}

vtkImageDataPtr VolumeSliceStreamer::getMask()
{
	if (!mCachedMask)
	{
		ProbeSectorPtr sector = mTool->getProbe()->getSector();
		mCachedMask = sector->getMask();
	}
	return mCachedMask;
}

void VolumeSliceStreamer::resetMask()
{
	mCachedMask = NULL;
	mCachedImageToSend.reset();
}

void VolumeSliceStreamer::sliceSlot()
{
	mCachedImageToSend.reset();
}

void VolumeSliceStreamer::setSourceImage(ImagePtr image)
{
	mSourceImage = image;
	if(mSourceImage)
		connect(mSourceImage.get(), &Image::transferFunctionsChanged, this, &VolumeSliceStreamer::sliceSlot);
	this->sliceSlot();
}

ImagePtr VolumeSliceStreamer::calculateSlice(ImagePtr source)
{
	vtkImageDataPtr slice = this->sliceOriginal(source);
	return this->convertToSscImage(slice, source);
}

vtkImageDataPtr VolumeSliceStreamer::sliceOriginal(ImagePtr source)
{
	mTimer->begin();
	vtkImageDataPtr framegrabbedSlice = this->frameGrab(source);
	mTimer->time("Grab");
	vtkImageDataPtr maskedSlice = this->maskSlice(framegrabbedSlice);
	mTimer->time("Mask");
	return maskedSlice;
}

vtkImageDataPtr VolumeSliceStreamer::frameGrab(ImagePtr source)
{
	SlicedImageProxyPtr imageSlicer(new SlicedImageProxy);
	imageSlicer->setImage(source);

	SimpleSliceProxyPtr slicer(new SimpleSliceProxy);
	Transform3D vMr = this->getTransform_vMr();
	slicer->set_sMr(vMr);
	imageSlicer->setSliceProxy(slicer);

	ProbeDefinition probeDefinition = mTool->getProbe()->getProbeDefinition();
	Eigen::Array3i outDim(probeDefinition.getSize().width(), probeDefinition.getSize().height(), 1);
	imageSlicer->setOutputFormat(Vector3D(0,0,0), outDim, probeDefinition.getSpacing());

	vtkImageDataPtr retval = vtkImageDataPtr::New();
	imageSlicer->update();
	imageSlicer->getOutputPort()->Update();
	retval->DeepCopy(imageSlicer->getOutput());

	return retval;
}

vtkImageDataPtr VolumeSliceStreamer::maskSlice(vtkImageDataPtr unmaskedSlice)
{
	vtkImageMaskPtr maskFilter = vtkImageMaskPtr::New();
	maskFilter->SetMaskInputData(this->getMask());
	maskFilter->SetImageInputData(unmaskedSlice);
	maskFilter->SetMaskedOutputValue(0.0);
	maskFilter->Update();
	return maskFilter->GetOutput();
}

ImagePtr VolumeSliceStreamer::convertToSscImage(vtkImageDataPtr slice, ImagePtr volume)
{
	ImagePtr retval = ImagePtr(new Image("Volume Slice", slice, "Volume Slice"));
	mTimer->time("Convert");
	return retval;
}

Transform3D VolumeSliceStreamer::getTransform_vMr()
{
	ProbeDefinition probeDefinition = mTool->getProbe()->getProbeDefinition();
	ProbeSector probesector;
	probesector.setData(probeDefinition);

	Transform3D uMt = probesector.get_tMu().inv();
	Transform3D vMu = probesector.get_uMv().inv();
	Transform3D vMt = vMu * uMt;

	Transform3D tMpr = mTool->get_prMt().inv();
	Transform3D prMr = mPatientModelService->get_rMpr().inv();

	return vMt * tMpr * prMr;
}

int VolumeSliceStreamer::getAverageTimePerSimulatedFrame()
{
	cx::reporter()->sendDebug("Grab frame: " + qstring_cast(mTimer->getTime(QString("Grab"))));
	cx::reporter()->sendDebug("Mask frame: " + qstring_cast(mTimer->getTime(QString("Mask"))));
	cx::reporter()->sendDebug("Convert frame to Image: " + qstring_cast(mTimer->getTime(QString("Convert"))));
	return mTimer->getTotalLoggedTime();
}

} /* namespace cx */
