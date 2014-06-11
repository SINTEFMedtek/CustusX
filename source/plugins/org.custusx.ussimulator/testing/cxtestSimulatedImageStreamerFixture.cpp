#include "cxtestSimulatedImageStreamerFixture.h"

#include "cxtestDummyDataManager.h"
#include "cxtestUtilities.h"
#include "cxDummyTool.h"
#include "cxDummyToolManager.h"
#include "cxConfig.h"

namespace cxtest
{
SimulatedImageStreamerFixture::SimulatedImageStreamerFixture()
{
	mSender.reset(new TestSender());
	REQUIRE(mSender);
}

SimulatedImageStreamerFixture::SimulatedImageStreamerFixture(TestSenderPtr sender) :
	mSender(sender)
{
	REQUIRE(mSender);
}

void SimulatedImageStreamerFixture::init()
{
	//	cx::TrackingServicePtr trackingService = cx::DummyToolManager::create(); not required??
	mDataService = cxtest::createDummyDataService();
	REQUIRE(mDataService);
	mImage = cxtest::Utilities::create3DImage();
	REQUIRE(mImage);
	mTool = cx::DummyToolTestUtilities::createDummyTool(cx::DummyToolTestUtilities::createProbeDataLinear());
	REQUIRE(mTool);
}

cx::SimulatedImageStreamerPtr SimulatedImageStreamerFixture::createSimulatedImageStreamer()
{
	this->init();
	mImagestreamer.reset(new cx::SimulatedImageStreamer());
	REQUIRE(mImagestreamer);
	return mImagestreamer;
}

cx::SimulatedImageStreamerPtr SimulatedImageStreamerFixture::createRunningSimulatedImageStreamer()
{
	this->createSimulatedImageStreamer();

	REQUIRE(mImagestreamer->initialize(mImage, mTool, mDataService));
	REQUIRE(mImagestreamer->startStreaming(mSender));
	return mImagestreamer;
}

int SimulatedImageStreamerFixture::simulateAndCheckUS(int numFrames)
{
	cx::SimulatedImageStreamerPtr imagestreamer = createRunningSimulatedImageStreamer();

	bool silent = true;
	checkSimulatedFrames(numFrames, silent);

	int retval = imagestreamer->getAverageTimePerSimulatedFrame();
	imagestreamer->stopStreaming();
	return retval;
}

TestSenderPtr SimulatedImageStreamerFixture::getSender()
{
	return mSender;
}

void SimulatedImageStreamerFixture::checkSenderGotImageFromStreamer()
{
	REQUIRE(mSender);
	cx::PackagePtr package = mSender->getSentPackage();
	REQUIRE(package);
	cx::ImagePtr image = package->mImage;
	REQUIRE(image);
}

void SimulatedImageStreamerFixture::checkSimulatedFrame(bool silentAtArrive)
{
	REQUIRE(mSender);
	REQUIRE(waitForQueuedSignal(mSender.get(), SIGNAL(newPackage()), 200, silentAtArrive));
	checkSenderGotImageFromStreamer();
}

void SimulatedImageStreamerFixture::requireNoSimulatedFrame(bool silentAtArrive)
{
	REQUIRE(mSender);
	REQUIRE_FALSE(waitForQueuedSignal(mSender.get(), SIGNAL(newPackage()), 200, silentAtArrive));
//	REQUIRE_FALSE(mSender->getSentPackage());
}

void SimulatedImageStreamerFixture::checkSimulatedFrames(int numFrames, bool silentAtArrive)
{
	for(int i = 0; i < numFrames; ++i)
		this->checkSimulatedFrame(silentAtArrive);
}

#ifdef CX_BUILD_US_SIMULATOR
ImageSimulator SimulatedImageStreamerFixture::constructImageSimulatorVariable()
{
	ImageSimulator mUSSimulator;
	return mUSSimulator;
}

boost::shared_ptr<ImageSimulator> SimulatedImageStreamerFixture::constructImageSimulatorBoostPtr()
{
	boost::shared_ptr<ImageSimulator> mUSSimulator(new ImageSimulator());
	return mUSSimulator;
}
#endif //CX_BUILD_US_SIMULATOR

}//namespace cxtest
