#ifndef CXTESTSIMULATEDIMAGESTREAMERFIXTURE_H
#define CXTESTSIMULATEDIMAGESTREAMERFIXTURE_H

#include "catch.hpp"
#include "cxSimulatedImageStreamer.h"
#include "cxtestSender.h"
#include "cxtestQueuedSignalListener.h"
#include "cxForwardDeclarations.h"

namespace cxtest
{

class SimulatedImageStreamerFixture
{
public:
	SimulatedImageStreamerFixture();
	SimulatedImageStreamerFixture(TestSenderPtr sender);

	cx::SimulatedImageStreamerPtr createSimulatedImageStreamer();
	cx::SimulatedImageStreamerPtr createRunningSimulatedImageStreamer();
	int simulateAndCheckUS(int numFrames);
	TestSenderPtr getSender();

	void checkSimulatedFrame(bool silentAtArrive = false);
	void requireNoSimulatedFrame(bool silentAtArrive = false);
	void checkSimulatedFrames(int numFrames, bool silentAtArrive = false);

	static ImageSimulator constructImageSimulatorVariable();
	static boost::shared_ptr<ImageSimulator> constructImageSimulatorBoostPtr();

private:
	TestSenderPtr mSender;
	cx::DataServicePtr mDataService;
	cx::ImagePtr mImage;
	cx::DummyToolPtr mTool;
	cx::SimulatedImageStreamerPtr mImagestreamer;

	void init();
	void checkSenderGotImageFromStreamer();
};

}//namespace cxtest
#endif // CXTESTSIMULATEDIMAGESTREAMERFIXTURE_H
