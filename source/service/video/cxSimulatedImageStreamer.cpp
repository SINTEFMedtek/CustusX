#include "cxSimulatedImageStreamer.h"

#include "vtkPNGWriter.h"
#include "vtkImageReslice.h"
#include "vtkMatrix4x4.h"
#include "vtkImageData.h"
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
	connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(sliceSlot(Transform3D, double)));

	this->setInitialized(true);
}

bool SimulatedImageStreamer::startStreaming(SenderPtr sender)
{
	if (!this->isInitialized())
	{
		std::cout << "SimulatedImageStreamer: Failed to start streaming: Not initialized." << std::endl;
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

void SimulatedImageStreamer::sliceSlot(Transform3D matrix, double timestamp)
{
	mImageToSend = getSlice(mSourceImage);
}

ssc::ImagePtr SimulatedImageStreamer::getSlice(ssc::ImagePtr source)
{
	ssc::ImagePtr slice;
	ssc::ProbeData probedata = mTool->getProbe()->getData();
	ssc::ProbeSector probesector;
	probesector.setData(probedata);

	vtkMatrix4x4Ptr mMatrixAxes = vtkMatrix4x4Ptr::New();
	ssc::Transform3D uMt = probesector.get_tMu().inv();
	ssc::Transform3D vMu = probesector.get_uMv().inv();
	ssc::Transform3D vMt = vMu * uMt;
	uMt = vMt;
	ssc::Transform3D tMpr = mTool->get_prMt().inv();
	ssc::Transform3D prMr = ssc::toolManager()->get_rMpr()->inv();
	ssc::Transform3D rMd = mSourceImage->get_rMd();
	ssc::Transform3D uMd = uMt * tMpr * prMr * rMd;
	mMatrixAxes->DeepCopy(uMd.inv().getVtkMatrix());

	vtkImageReslicePtr mReslicer = vtkImageReslicePtr::New();
	mReslicer->SetInput(source->getBaseVtkImageData());
	mReslicer->SetBackgroundLevel(source->getMin());
	mReslicer->SetInterpolationModeToLinear();
	mReslicer->SetOutputDimensionality(2);
	mReslicer->SetResliceAxes(mMatrixAxes);
	mReslicer->AutoCropOutputOn(); // fix used in 2.0.9, but slower update rate
	mReslicer->SetOutputOrigin(0,0,0);
	mReslicer->SetOutputExtent(0, probedata.getImage().mSize.width()-1, 0, probedata.getImage().mSize.height()-1, 0, 0);
	mReslicer->SetOutputSpacing(probedata.getImage().mSpacing.data());
	mReslicer->Update();

	vtkImageDataPtr vtkSlice = vtkImageDataPtr::New();

	vtkSlice->DeepCopy(mReslicer->GetOutput());
	slice = ssc::ImagePtr(new ssc::Image("TEST_UID", vtkSlice, "TEST_NAME"));
	slice->resetTransferFunction(source->getTransferFunctions3D(), source->getLookupTable2D());

//	std::cout << uMd << std::endl;
//	vtkSlice->Print(std::cout);

	return slice;
}

} /* namespace cx */
