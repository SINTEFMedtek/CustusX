/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
	return this->convertToCxImage(slice, source);
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

ImagePtr VolumeSliceStreamer::convertToCxImage(vtkImageDataPtr slice, ImagePtr volume)
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

} /* namespace cx */
