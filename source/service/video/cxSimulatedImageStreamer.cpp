#include "cxSimulatedImageStreamer.h"

#include <boost/math/special_functions/round.hpp>

#include "vtkPNGWriter.h"
#include "vtkImageReslice.h"
#include "vtkMatrix4x4.h"
#include "vtkImageData.h"
#include "vtkImageMask.h"
#include "cxReporter.h"
#include "cxDataManager.h"
#include "cxSliceProxy.h"
#include "cxProbeSector.h"
#include "cxProbeData.h"
//#include "cxToolManager.h"
#include "cxTransform3D.h"
#include "cxVolumeHelpers.h"
//#include "cxToolManager.h"

#include "cxSlicedImageProxy.h"
#include "cxSliceProxy.h"
#include "vtkImageChangeInformation.h"
#include "cxLogger.h"
#include "cxTypeConversions.h"
#include "cxSettings.h"
#include "cxProbeImpl.h"
#include <boost/make_shared.hpp>

namespace cx
{

SimulatedImageStreamer::SimulatedImageStreamer() :
	mTimer(new CyclicActionLogger())
{
	this->setSendInterval(40);
    this->initUSSimulator();
}

SimulatedImageStreamer::~SimulatedImageStreamer()
{}

void SimulatedImageStreamer::initUSSimulator()
{
#ifdef CX_BUILD_US_SIMULATOR
    mUSSimulator.reset(new ImageSimulator());
		/*mUSSimulator->setShadowsAirOn(false);
		mUSSimulator->setShadowsBoneOn(false);
		mUSSimulator->setReflectionsOn(false);
		mUSSimulator->setAbsorptionOn(false);
		mUSSimulator->setSpeckleOn(false);*/
#endif //CX_BUILD_US_SIMULATOR
}

void SimulatedImageStreamer::initialize(ImagePtr image, ToolPtr tool, DataServicePtr dataManager)
{
	if(!image || !tool || !dataManager)
	{
		this->setInitialized(false);
		return;
	}
	mDataManager = dataManager;
	this->createSendTimer();

	this->setSourceImage(image);
	mTool = tool;
	connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(sliceSlot()));
	connect(mTool->getProbe().get(), SIGNAL(activeConfigChanged()), this, SLOT(resetMask()));
	connect(mTool->getProbe().get(), SIGNAL(sectorChanged()), this, SLOT(defineSectorInSimulator()));

	this->defineSectorInSimulator();

//	this->generateMaskSlot();

	this->setInitialized(true);
}

bool SimulatedImageStreamer::startStreaming(SenderPtr sender)
{
	if (!this->isInitialized())
	{
		reportError("SimulatedImageStreamer: Failed to start streaming: Not initialized.");
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
	package->mImage = this->getSlice();
	mSender->send(package);
}

ImagePtr SimulatedImageStreamer::getSlice()
{
	if(!mTool || !mSourceImage)
		return ImagePtr();

	if (!mCachedImageToSend)
	{
		mCachedImageToSend = this->calculateSlice(mSourceImage);
	}

	return mCachedImageToSend;
}

vtkImageDataPtr SimulatedImageStreamer::getMask()
{
	if (!mCachedMask)
	{
		ProbeSectorPtr sector = mTool->getProbe()->getSector();
		mCachedMask = sector->getMask();
	}
	return mCachedMask;
}

void SimulatedImageStreamer::resetMask()
{
	mCachedMask = NULL;
	mCachedImageToSend.reset();
}

void SimulatedImageStreamer::sliceSlot()
{
	mCachedImageToSend.reset();
}

void SimulatedImageStreamer::setSourceToImageSlot(QString imageUid)
{
	ImagePtr image = mDataManager->getImage(imageUid);
	this->setSourceImage(image);
}

void SimulatedImageStreamer::setSourceImage(ImagePtr image)
{
	mSourceImage = image;
	connect(mSourceImage.get(), SIGNAL(transferFunctionsChanged()), this, SLOT(sliceSlot()));
	this->sliceSlot();
}

ImagePtr SimulatedImageStreamer::calculateSlice(ImagePtr source)
{
	vtkImageDataPtr simulatedSlice;
	QString simulationType = settings()->value("USsimulation/type").toString();

	if(simulationType == "CT to US")
		simulatedSlice = simulateUSFromCTSlice(source);
	else if(simulationType == "MR to US")
		simulatedSlice = simulateUSFromMRSlice(source);
	else if(simulationType == "Original data")
		simulatedSlice = sliceOriginal(source);
	else
	{
		cx::reporter()->sendWarning("SimulatedImageStreamer::calculateSlice(): Unknown simulation: " + simulationType);
		simulatedSlice = sliceOriginal(source);
	}

	ImagePtr slice = this->convertToSscImage(simulatedSlice, source);
	return slice;
}


vtkImageDataPtr SimulatedImageStreamer::simulateUSFromCTSlice(ImagePtr source)
{
	vtkImageDataPtr simulatedSlice;

#ifdef CX_BUILD_US_SIMULATOR
//	std::cout << "CT to US simulator running" << std::endl;
	vtkImageDataPtr simInput = this->createSimulatorInputSlice(source);
	simulatedSlice = mUSSimulator->simulateFromCT(simInput);
	mTimer->time("Simulate");
#else
	cx::reporter()->sendError("CT to US simulator not running");
	simulatedSlice = sliceOriginal(source);
#endif //CX_BUILD_US_SIMULATOR

	return simulatedSlice;
}

//TODO: implement
vtkImageDataPtr SimulatedImageStreamer::simulateUSFromMRSlice(ImagePtr source)
{
	vtkImageDataPtr simulatedSlice;
//	vtkImageDataPtr simInput = this->createSimulatorInputSlice(source);
	simulatedSlice = sliceOriginal(source);
//	cx::reporter()->sendError("MR to US simulator not running");
	return simulatedSlice;
}

vtkImageDataPtr SimulatedImageStreamer::createSimulatorInputSlice(ImagePtr source)
{
	mTimer->begin();
	vtkImageDataPtr framegrabbedSlice = this->frameGrab(source, false);
	mTimer->time("Grab");
	mTimer->time("Mask");
	return framegrabbedSlice;
}

void SimulatedImageStreamer::defineSectorInSimulator()
{
#ifdef CX_BUILD_US_SIMULATOR
	if (!mUSSimulator)
		return;
	ProbeSectorPtr sector = mTool->getProbe()->getSector();
	ProbeDefinition sectorParams = sector->mData;

	mUSSimulator->setProbeType(static_cast<ImageSimulator::PROBE_TYPE>(sectorParams.getType()));//TODO: Make ImageSimulator use ProbeDefinition::TYPE

	Eigen::Vector3d origin_p = sectorParams.getOrigin_p();
	Eigen::Vector3d spacing = sectorParams.getSpacing();
	Vector3D origin_v = multiply_elems(origin_p, spacing);
	mUSSimulator->setOrigin(origin_v);
	double width = sectorParams.getWidth();
	double depth = sectorParams.getDepthEnd() - sectorParams.getDepthStart();
	double offset = sectorParams.getDepthStart();
//	std::cout << "width: " << width << " depth: " << depth << " offset: " << offset << std::endl;
	mUSSimulator->verifyAndSetSectorSize(width, depth, offset);

	this->sliceSlot();
#endif //CX_BUILD_US_SIMULATOR
}

vtkImageDataPtr SimulatedImageStreamer::sliceOriginal(ImagePtr source)
{
		mTimer->begin();
		vtkImageDataPtr framegrabbedSlice = this->frameGrab(source);
		mTimer->time("Grab");
		vtkImageDataPtr maskedFramedgrabbedSlice = this->maskSlice(framegrabbedSlice);
		mTimer->time("Mask");
		mTimer->time("Simulate");
		return maskedFramedgrabbedSlice;
}

vtkImageDataPtr SimulatedImageStreamer::frameGrab(ImagePtr source, bool applyLUT)
{
	SlicedImageProxyPtr imageSlicer(new SlicedImageProxy);
	imageSlicer->setImage(source);

	SimpleSliceProxyPtr slicer(new SimpleSliceProxy);
	Transform3D vMr = this->getTransform_vMr();
	slicer->set_sMr(vMr);
	imageSlicer->setSliceProxy(slicer);

	ProbeDefinition probedata = mTool->getProbe()->getProbeData();
	Eigen::Array3i outDim(probedata.getSize().width(), probedata.getSize().height(), 1);
	imageSlicer->setOutputFormat(Vector3D(0,0,0), outDim, probedata.getSpacing());

	vtkImageDataPtr retval = vtkImageDataPtr::New();

	imageSlicer->update();
	if (applyLUT)
	{
		imageSlicer->getOutputPort()->Update();
		retval->DeepCopy(imageSlicer->getOutput());
	}
	else //Don't use LUT
	{
		imageSlicer->getOutputPortWithoutLUT()->Update();
		retval->DeepCopy(imageSlicer->getOutputWithoutLUT());
	}

	return retval;
}

vtkImageDataPtr SimulatedImageStreamer::maskSlice(vtkImageDataPtr unmaskedSlice)
{
	vtkImageMaskPtr maskFilter = vtkImageMaskPtr::New();
	maskFilter->SetMaskInputData(this->getMask());
	maskFilter->SetImageInputData(unmaskedSlice);
	maskFilter->SetMaskedOutputValue(0.0);
	maskFilter->Update();

	vtkImageDataPtr maskedSlice = maskFilter->GetOutput();
	return maskedSlice;
}

ImagePtr SimulatedImageStreamer::convertToSscImage(vtkImageDataPtr slice, ImagePtr volume)
{
	ImagePtr retval = ImagePtr(new Image("Simulated US", slice, "Simulated US"));
	mTimer->time("Convert");
	return retval;
}

Transform3D SimulatedImageStreamer::getTransform_vMr()
{
	ProbeDefinition probedata = mTool->getProbe()->getProbeData();
	ProbeSector probesector;
	probesector.setData(probedata);

	Transform3D uMt = probesector.get_tMu().inv();
	Transform3D vMu = probesector.get_uMv().inv();
	Transform3D vMt = vMu * uMt;

	Transform3D tMpr = mTool->get_prMt().inv();
	Transform3D prMr = mDataManager->get_rMpr().inv();

	Transform3D vMr = vMt * tMpr * prMr;
	return vMr;
}

int SimulatedImageStreamer::getAverageTimePerSimulatedFrame()
{
	cx::reporter()->sendDebug("Grab frame: " + qstring_cast(mTimer->getTime(QString("Grab"))));
	cx::reporter()->sendDebug("Mask frame: " + qstring_cast(mTimer->getTime(QString("Mask"))));
	cx::reporter()->sendDebug("Run simulation: " + qstring_cast(mTimer->getTime(QString("Simulate"))));
	cx::reporter()->sendDebug("Convert frame to Image: " + qstring_cast(mTimer->getTime(QString("Convert"))));
	return mTimer->getTotalLoggedTime();
}

} /* namespace cx */
