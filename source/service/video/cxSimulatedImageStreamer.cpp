#include "cxSimulatedImageStreamer.h"

#include "vtkPNGWriter.h"
#include "vtkImageReslice.h"
#include "vtkMatrix4x4.h"
#include "vtkImageData.h"
#include "vtkImageMask.h"
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "sscSliceProxy.h"
#include "sscSlicedImageProxy.h"
#include "sscProbeSector.h"
#include "sscProbeData.h"
#include "sscToolManager.h"
#include "sscTransform3D.h"
#include "sscVolumeHelpers.h"
#include "cxToolManager.h"

namespace cx
{
SimulatedImageStreamer::SimulatedImageStreamer()
{
	this->setSendInterval(40);
}

SimulatedImageStreamer::~SimulatedImageStreamer()
{}

void SimulatedImageStreamer::initialize()
{
	ImagePtr image = dataManager()->getActiveImage();
	ToolPtr tool = cxToolManager::getInstance()->findFirstProbe();
	this->initialize(image, tool);
}

void SimulatedImageStreamer::initialize(ImagePtr image, ToolPtr tool)
{
	if(!image || !tool)
	{
		this->setInitialized(false);
		return;
	}
	this->createSendTimer();

	this->setSourceImage(image);
	mTool = tool;
	connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(sliceSlot()));
	connect(mTool->getProbe().get(), SIGNAL(activeConfigChanged()), this, SLOT(generateMaskSlot()));

	this->generateMaskSlot();

	this->setInitialized(true);
}

bool SimulatedImageStreamer::startStreaming(SenderPtr sender)
{
	if (!this->isInitialized())
	{
		messageManager()->sendError("SimulatedImageStreamer: Failed to start streaming: Not initialized.");
		return false;
	}
	mSender = sender;
	mSendTimer->start(this->getSendInterval());
	return true;
}

void SimulatedImageStreamer::stopStreaming()
{
	if(mSendTimer)
		mSendTimer->stop();
}

QString SimulatedImageStreamer::getType()
{
	return "SimulatedImageStreamer";
}

void SimulatedImageStreamer::streamSlot()
{
	PackagePtr package(new Package());
	package->mImage = mImageToSend;
	mSender->send(package);
}

void SimulatedImageStreamer::generateMaskSlot()
{
	messageManager()->sendDebug("START");
	ProbeSectorPtr sector = mTool->getProbe()->getSector();
	mMask = sector->getMask();
	messageManager()->sendDebug("END");
	this->sliceSlot();
}

void SimulatedImageStreamer::sliceSlot()
{
	if(!mTool || !mSourceImage)
		return;
	mImageToSend = this->getSlice(mSourceImage);
}

void SimulatedImageStreamer::setSourceToActiveImageSlot()
{
	ImagePtr image = dataManager()->getActiveImage();
	this->setSourceImage(image);
}

void SimulatedImageStreamer::setSourceToImageSlot(QString imageUid)
{
	ImagePtr image = dataManager()->getImage(imageUid);
	this->setSourceImage(image);
}

void SimulatedImageStreamer::setSourceImage(ImagePtr image)
{
	mSourceImage = image;
	this->sliceSlot();
}

ImagePtr SimulatedImageStreamer::getSlice(ImagePtr source)
{
	vtkMatrix4x4Ptr sliceAxes = this->calculateSliceAxes();
	vtkImageDataPtr framegrabbedSlice = this->getSliceUsingProbeDefinition(source, sliceAxes);
	vtkImageDataPtr maskedFramedgrabbedSlice = this->maskSlice(framegrabbedSlice);
	ImagePtr slice = this->convertToSscImage(maskedFramedgrabbedSlice, source);
	slice->setLookupTable2D(source->getLookupTable2D());
	slice->setTransferFunctions3D(source->getTransferFunctions3D());

	return slice;
}

vtkMatrix4x4Ptr SimulatedImageStreamer::calculateSliceAxes()
{
	vtkMatrix4x4Ptr sliceAxes = vtkMatrix4x4Ptr::New();

	Transform3D dMv = this->getTransformFromProbeSectorImageSpaceToImageSpace();
	sliceAxes->DeepCopy(dMv.getVtkMatrix());

	return sliceAxes;
}

vtkImageDataPtr SimulatedImageStreamer::getSliceUsingProbeDefinition(ImagePtr source, vtkMatrix4x4Ptr sliceAxes)
{
	ProbeData probedata = mTool->getProbe()->getProbeData();

	vtkImageReslicePtr reslicer = this->createReslicer(source, sliceAxes);

	vtkImageDataPtr retval = vtkImageDataPtr::New();
	retval->DeepCopy(reslicer->GetOutput());

	return retval;
}

vtkImageDataPtr SimulatedImageStreamer::maskSlice(vtkImageDataPtr unmaskedSlice)
{
	vtkImageMaskPtr maskFilter = vtkImageMaskPtr::New();
	maskFilter->SetMaskInput(mMask);
	maskFilter->SetImageInput(unmaskedSlice);
	maskFilter->SetMaskedOutputValue(0.0);
	maskFilter->Update();

	vtkImageDataPtr maskedSlice = maskFilter->GetOutput();
	return maskedSlice;
}

ImagePtr SimulatedImageStreamer::convertToSscImage(vtkImageDataPtr slice, ImagePtr volume)
{
	ImagePtr retval = ImagePtr(new Image("Simulated US", slice, "Simulated US"));
	return retval;
}

vtkImageReslicePtr SimulatedImageStreamer::createReslicer(ImagePtr source, vtkMatrix4x4Ptr sliceAxes)
{
	ProbeData probedata = mTool->getProbe()->getProbeData();

	vtkImageReslicePtr reslicer = vtkImageReslicePtr::New();
	reslicer->SetInput(source->getBaseVtkImageData());
	reslicer->SetBackgroundLevel(source->getMin());
	reslicer->SetInterpolationModeToLinear();
	reslicer->SetOutputDimensionality(2);
	reslicer->SetResliceAxes(sliceAxes);
	reslicer->AutoCropOutputOn();
	reslicer->SetOutputOrigin(0,0,0);
	reslicer->SetOutputExtent(0, probedata.getImage().mSize.width()-1, 0, probedata.getImage().mSize.height()-1, 0, 0);
	reslicer->SetOutputSpacing(probedata.getImage().mSpacing.data());
	reslicer->Update();

	return reslicer;
}

Transform3D SimulatedImageStreamer::getTransformFromProbeSectorImageSpaceToImageSpace()
{
	ProbeData probedata = mTool->getProbe()->getProbeData();
	ProbeSector probesector;
	probesector.setData(probedata);

	Transform3D uMt = probesector.get_tMu().inv();
	Transform3D vMu = probesector.get_uMv().inv();
	Transform3D vMt = vMu * uMt;
	Transform3D tMpr = mTool->get_prMt().inv();
	Transform3D prMr = toolManager()->get_rMpr()->inv();
	Transform3D rMd = mSourceImage->get_rMd();
	Transform3D vMd = vMt * tMpr * prMr * rMd;
	Transform3D dMv = vMd.inv();

	return dMv;
}

} /* namespace cx */
