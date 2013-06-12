#include "cxSimulatedImageStreamer.h"

#include "vtkPNGWriter.h"
#include "vtkImageReslice.h"
#include "vtkMatrix4x4.h"
#include "vtkImageData.h"
#include "sscMessageManager.h"
#include "sscSliceProxy.h"
#include "sscSlicedImageProxy.h"
#include "sscProbeSector.h"
#include "sscProbeData.h"
#include "sscToolManager.h"
#include "sscTransform3D.h"

#include "sscVolumeHelpers.h"

namespace cx
{
SimulatedImageStreamer::SimulatedImageStreamer()
{
	this->setSendInterval(40);
}

SimulatedImageStreamer::~SimulatedImageStreamer()
{}

void SimulatedImageStreamer::initialize(ssc::ImagePtr image, ssc::ToolPtr tool)
{
	if(!image || !tool)
	{
		this->setInitialized(false);
		return;
	}
	this->createSendTimer();

	mSourceImage = image;
	mTool = tool;
	connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(sliceSlot()));

	this->setInitialized(true);
}

bool SimulatedImageStreamer::startStreaming(SenderPtr sender)
{
	if (!this->isInitialized())
	{
		ssc::messageManager()->sendError("SimulatedImageStreamer: Failed to start streaming: Not initialized.");
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

void SimulatedImageStreamer::sliceSlot()
{
	mImageToSend = getSlice(mSourceImage);
}

ssc::ImagePtr SimulatedImageStreamer::getSlice(ssc::ImagePtr source)
{
	vtkMatrix4x4Ptr sliceAxes = this->calculateSliceAxes();
	vtkImageDataPtr vtkSlice = this->getSliceUsingProbeDefinition(source, sliceAxes);
	ssc::ImagePtr slice = this->createSscImage(vtkSlice, source);

	return slice;
}

vtkMatrix4x4Ptr SimulatedImageStreamer::calculateSliceAxes()
{
	vtkMatrix4x4Ptr sliceAxes = vtkMatrix4x4Ptr::New();

	ssc::Transform3D dMv = this->getTransformFromProbeSectorImageSpaceToImageSpace();
	sliceAxes->DeepCopy(dMv.getVtkMatrix());

	return sliceAxes;
}

vtkImageDataPtr SimulatedImageStreamer::getSliceUsingProbeDefinition(ssc::ImagePtr source, vtkMatrix4x4Ptr sliceAxes)
{
	ssc::ProbeData probedata = mTool->getProbe()->getProbeData();

	vtkImageReslicePtr reslicer = this->createReslicer(source, sliceAxes);

	vtkImageDataPtr retval = vtkImageDataPtr::New();
	retval->DeepCopy(reslicer->GetOutput());

	return retval;
}

ssc::ImagePtr SimulatedImageStreamer::createSscImage(vtkImageDataPtr slice, ssc::ImagePtr volume)
{
	ssc::ImagePtr retval = ssc::ImagePtr(new ssc::Image("TEST_UID", slice, "TEST_NAME"));
	retval->resetTransferFunction(volume->getTransferFunctions3D(), volume->getLookupTable2D());
	return retval;
}

vtkImageReslicePtr SimulatedImageStreamer::createReslicer(ssc::ImagePtr source, vtkMatrix4x4Ptr sliceAxes)
{
	ssc::ProbeData probedata = mTool->getProbe()->getProbeData();

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

ssc::Transform3D SimulatedImageStreamer::getTransformFromProbeSectorImageSpaceToImageSpace()
{
	ssc::ProbeData probedata = mTool->getProbe()->getProbeData();
	ssc::ProbeSector probesector;
	probesector.setData(probedata);

	ssc::Transform3D uMt = probesector.get_tMu().inv();
	ssc::Transform3D vMu = probesector.get_uMv().inv();
	ssc::Transform3D vMt = vMu * uMt;
	ssc::Transform3D tMpr = mTool->get_prMt().inv();
	ssc::Transform3D prMr = ssc::toolManager()->get_rMpr()->inv();
	ssc::Transform3D rMd = mSourceImage->get_rMd();
	ssc::Transform3D vMd = vMt * tMpr * prMr * rMd;
	ssc::Transform3D dMv = vMd.inv();

	return dMv;
}

} /* namespace cx */
