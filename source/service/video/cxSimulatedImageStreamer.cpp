#include "cxSimulatedImageStreamer.h"

#include "sscSliceProxy.h"
#include "sscSlicedImageProxy.h"

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
	mImageToSend = getSlice(mSourceImage, matrix);
}

ssc::ImagePtr SimulatedImageStreamer::getSlice(ssc::ImagePtr source, Transform3D matrix)
{
	ssc::ImagePtr slice;

	ssc::SliceProxyPtr sliceProxy(new ssc::SliceProxy());
	sliceProxy->setTool(mTool);
	sliceProxy->setFollowType(ssc::ftFIXED_CENTER);
	sliceProxy->setOrientation(ssc::otORTHOGONAL);
	sliceProxy->setPlane(ssc::ptANYPLANE);

	ssc::SlicedImageProxyPtr slicedImageProxy(new ssc::SlicedImageProxy());
	slicedImageProxy->setSliceProxy(sliceProxy);
	slicedImageProxy->setImage(mSourceImage);
	slicedImageProxy->update();

	vtkImageDataPtr vtkSlice = slicedImageProxy->getOutput();
	slice = ssc::ImagePtr(new ssc::Image("TEST_UID", vtkSlice, "TEST_NAME"));

	return slice;
}

} /* namespace cx */
