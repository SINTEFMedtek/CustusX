/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include <memory>
#include <QFile>
#include <QTemporaryDir>
#include "cxNdiTrackerManager.h"
#include "cxNdiFrameRateEstimator.h"
#include "cxtestNdiFakeDevice.h"

namespace cxtest
{

namespace
{

cx::ToolFileParser::ToolInternalStructurePtr createAuroraTool(QString uid, int port, bool reference = false)
{
	cx::ToolFileParser::ToolInternalStructurePtr retval = std::make_shared<cx::ToolFileParser::ToolInternalStructure>();
	retval->mUid = uid;
	retval->mIsReference = reference;
	retval->mIsPointer = !reference;
	retval->mTrackerType = cx::tsAURORA;
	retval->mPortNumber = port;
	retval->mChannelNumber = 0;
	retval->m5DOF = false;
	retval->mWireless = false;
	return retval;
}

/** Aurora with a pointer on port 0 (handle 0x0A) and a reference on port 1 (handle 0x0B). */
struct ManagerFixture
{
	ManagerFixture(bool withReference = true)
	{
		device.reset(new FakeNdiDevice());
		device->addWiredHandle(kPointerHandle, 0);
		device->addWiredHandle(kReferenceHandle, 1);

		cx::ToolFileParser::TrackerInternalStructure tracker;
		tracker.mType = cx::tsAURORA;
		cx::ToolFileParser::ToolInternalStructurePtr reference;
		if (withReference)
		{
			reference = createAuroraTool("reference", 1, true);
		}
		manager.reset(new cx::NdiTrackerManager(tracker, {createAuroraTool("pointer", 0)}, reference, device));

		QObject::connect(manager.get(), &cx::NdiTrackerManager::error, [this]() { errors++; });
		QObject::connect(manager->getTools()["pointer"].get(), &cx::NdiTool::toolTransformAndTimestamp,
						 [this](cx::Transform3D prMt, double, cx::ToolPositionMetadata) { pointerTransforms.push_back(prMt); });
	}

	void startTracking()
	{
		manager->initializeSlot(true);
		manager->trackSlot(true);
	}

	cx::NdiToolPtr pointer() { return manager->getTools()["pointer"]; }

	enum { kPointerHandle = 0x0A, kReferenceHandle = 0x0B };
	std::shared_ptr<FakeNdiDevice> device;
	cx::NdiTrackerManagerPtr manager;
	int errors = 0;
	std::vector<cx::Transform3D> pointerTransforms;
	unsigned long frameNumber = 0;
};

cx::Transform3D translation(double x, double y, double z)
{
	return cx::createTransformTranslate(cx::Vector3D(x, y, z));
}

cx::NdiToolSample createSampleWithStatus(int portHandle, int portStatus)
{
	cx::NdiToolSample retval = createSample(portHandle, true);
	retval.portStatus = portStatus;
	return retval;
}

/** Poll count new frames, each with the given pointer and reference samples. */
void pollFrames(ManagerFixture& fixture, int count, cx::NdiToolSample pointer, cx::NdiToolSample reference)
{
	for (int i = 0; i < count; ++i)
	{
		fixture.device->setNextFrame(++fixture.frameNumber, {pointer, reference});
		fixture.manager->poll();
	}
}

} // namespace

TEST_CASE("NdiTrackerManager: initialize connects and sets up the tools", "[unit][org.custusx.core.tracking.system.ndi]")
{
	ManagerFixture fixture;
	cx::NdiConnectionSettings connection;
	connection.hostname = "/dev/ttyUSB3";
	fixture.manager->setConnectionSettings(connection);

	fixture.manager->initializeSlot(true);

	CHECK(fixture.manager->isInitialized());
	CHECK(fixture.device->lastConnection.hostname == "/dev/ttyUSB3");
	CHECK(fixture.pointer()->isInitialized());
	CHECK(fixture.pointer()->getPortHandle() == ManagerFixture::kPointerHandle);
	CHECK(fixture.manager->getReferenceTool()->getPortHandle() == ManagerFixture::kReferenceHandle);
	CHECK(fixture.errors == 0);
}

TEST_CASE("NdiTrackerManager: failing to connect gives an error and no initialization", "[unit][org.custusx.core.tracking.system.ndi]")
{
	ManagerFixture fixture;
	fixture.device->openSucceeds = false;

	fixture.manager->initializeSlot(true);

	CHECK_FALSE(fixture.manager->isInitialized());
	CHECK(fixture.errors == 1);
}

TEST_CASE("NdiTrackerManager: a device used by another application is reported before the error", "[unit][org.custusx.core.tracking.system.ndi]")
{
	ManagerFixture fixture;
	fixture.device->openSucceeds = false;
	fixture.device->inUse = true;
	QStringList emitted;
	QObject::connect(fixture.manager.get(), &cx::NdiTrackerManager::deviceInUse, [&emitted]() { emitted << "deviceInUse"; });
	QObject::connect(fixture.manager.get(), &cx::NdiTrackerManager::error, [&emitted]() { emitted << "error"; });

	fixture.manager->initializeSlot(true);
	CHECK(emitted == QStringList({"deviceInUse", "error"}));

	emitted.clear();
	fixture.device->inUse = false;
	fixture.manager->initializeSlot(true);
	CHECK(emitted == QStringList({"error"}));
}

TEST_CASE("NdiTrackerManager: tool positions are reported relative to the reference", "[unit][org.custusx.core.tracking.system.ndi]")
{
	ManagerFixture fixture;
	fixture.startTracking();
	REQUIRE(fixture.manager->isTracking());

	cx::Transform3D cMref = translation(100, 0, 0);
	cx::Transform3D cMpointer = translation(110, 20, 0);
	fixture.device->setNextFrame(1, {createSample(ManagerFixture::kPointerHandle, true, cMpointer),
									  createSample(ManagerFixture::kReferenceHandle, true, cMref)});
	fixture.manager->poll();

	CHECK(fixture.pointer()->isVisible());
	REQUIRE(fixture.pointerTransforms.size() == 1);
	CHECK(cx::similar(fixture.pointerTransforms[0], translation(10, 20, 0)));
	std::vector<int> expectedHandles = {int(ManagerFixture::kPointerHandle), int(ManagerFixture::kReferenceHandle)};
	CHECK(fixture.device->requestedHandles == expectedHandles);
}

TEST_CASE("NdiTrackerManager: without a reference, positions are in tracker space", "[unit][org.custusx.core.tracking.system.ndi]")
{
	ManagerFixture fixture(false);
	fixture.startTracking();

	fixture.device->setNextFrame(1, {createSample(ManagerFixture::kPointerHandle, true, translation(1, 2, 3))});
	fixture.manager->poll();

	REQUIRE(fixture.pointerTransforms.size() == 1);
	CHECK(cx::similar(fixture.pointerTransforms[0], translation(1, 2, 3)));
}

TEST_CASE("NdiTrackerManager: tools are invisible while the reference is not visible", "[unit][org.custusx.core.tracking.system.ndi]")
{
	ManagerFixture fixture;
	fixture.startTracking();
	fixture.device->setNextFrame(1, {createSample(ManagerFixture::kPointerHandle, true), createSample(ManagerFixture::kReferenceHandle, true)});
	fixture.manager->poll();
	REQUIRE(fixture.pointer()->isVisible());

	for (unsigned long frame = 2; frame <= 2 + cx::NdiTool::kMissTolerance; ++frame)
	{
		fixture.device->setNextFrame(frame, {createSample(ManagerFixture::kPointerHandle, true), createSample(ManagerFixture::kReferenceHandle, false)});
		fixture.manager->poll();
	}

	CHECK_FALSE(fixture.pointer()->isVisible());
	CHECK(fixture.pointerTransforms.size() == 1); // no positions while the reference is missing
}

TEST_CASE("NdiTrackerManager: a brief dropout during tracking does not make the tool flicker", "[unit][org.custusx.core.tracking.system.ndi]")
{
	// The 2-frame miss tolerance from the first NDI plugin (CustusX#31) must still apply
	// while tracking. Only stop/disconnect/lost device hide tools immediately.
	ManagerFixture fixture;
	fixture.startTracking();
	fixture.device->setNextFrame(1, {createSample(ManagerFixture::kPointerHandle, true), createSample(ManagerFixture::kReferenceHandle, true)});
	fixture.manager->poll();
	REQUIRE(fixture.pointer()->isVisible());

	int visibilityChanges = 0;
	QObject::connect(fixture.pointer().get(), &cx::NdiTool::toolVisible, [&visibilityChanges](bool) { visibilityChanges++; });
	for (unsigned long frame = 2; frame < 2 + cx::NdiTool::kMissTolerance; ++frame)
	{
		fixture.device->setNextFrame(frame, {createSample(ManagerFixture::kPointerHandle, false), createSample(ManagerFixture::kReferenceHandle, true)});
		fixture.manager->poll();
	}
	fixture.device->setNextFrame(10, {createSample(ManagerFixture::kPointerHandle, true), createSample(ManagerFixture::kReferenceHandle, true)});
	fixture.manager->poll();

	CHECK(fixture.pointer()->isVisible());
	CHECK(visibilityChanges == 0);
}

TEST_CASE("NdiTrackerManager: poll interval for devices that wait for the next frame and devices that don't", "[unit][org.custusx.core.tracking.system.ndi]")
{
	// BX waits for the next frame (Aurora): poll again at once, waiting would lose frames
	CHECK(cx::NdiTrackerManager::getPollInterval_ms(true, 0, 20) == cx::NdiTrackerManager::kNewFramePollInterval_ms);
	// BX replied at once with the last frame: wait until shortly before the next frame
	CHECK(cx::NdiTrackerManager::getPollInterval_ms(true, 3, 20) == 20);
	CHECK(cx::NdiTrackerManager::getPollInterval_ms(false, 0, 20) == cx::NdiTrackerManager::kRepeatPollInterval_ms);
	CHECK(cx::NdiTrackerManager::getPollInterval_ms(false, 5, 20) == cx::NdiTrackerManager::kRepeatPollInterval_ms);
}

TEST_CASE("NdiTrackerManager: polls again at once while every poll gives a new frame", "[unit][org.custusx.core.tracking.system.ndi]")
{
	ManagerFixture fixture;
	fixture.startTracking();
	for (unsigned long frame = 1; frame <= 3; ++frame)
	{
		fixture.device->setNextFrame(frame, {createSample(ManagerFixture::kPointerHandle, true), createSample(ManagerFixture::kReferenceHandle, true)});
		fixture.manager->poll();
		CHECK(fixture.manager->getCurrentPollInterval_ms() == cx::NdiTrackerManager::kNewFramePollInterval_ms);
	}
	fixture.manager->poll();
	CHECK(fixture.manager->getCurrentPollInterval_ms() == cx::NdiTrackerManager::kRepeatPollInterval_ms);
}

TEST_CASE("NdiFrameRateEstimator: waits until shortly before the next expected frame", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::NdiFrameRateEstimator estimator;
	double initialInterval = cx::NdiFrameRateEstimator::kInitialInterval_ms;
	double lookAhead = cx::NdiFrameRateEstimator::kLookAhead_ms;
	CHECK(estimator.getEstimatedInterval_ms() == Approx(initialInterval));

	// Frames every 25 ms (40 Hz, Aurora): the estimate converges towards 25 ms
	for (int i = 0; i < 50; ++i)
	{
		estimator.addFrame(1000.0 + 25.0 * i);
	}
	CHECK(estimator.getEstimatedInterval_ms() == Approx(25.0).epsilon(0.01));

	double lastFrame = 1000.0 + 25.0 * 49;
	CHECK(estimator.getWaitBeforeNextPoll_ms(lastFrame) == int(25.0 - lookAhead));
	CHECK(estimator.getWaitBeforeNextPoll_ms(lastFrame + 22.0) == 0); // already close: poll fast
}

TEST_CASE("NdiFrameRateEstimator: ignores long gaps and clamps the estimate", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::NdiFrameRateEstimator estimator;
	double maxInterval = cx::NdiFrameRateEstimator::kMaxInterval_ms;
	double minInterval = cx::NdiFrameRateEstimator::kMinInterval_ms;

	estimator.addFrame(1000.0);
	estimator.addFrame(3000.0); // 2 s gap, e.g. tools out of view: ignored
	CHECK(estimator.getEstimatedInterval_ms() == Approx(cx::NdiFrameRateEstimator::kInitialInterval_ms));

	for (int i = 1; i < 100; ++i)
	{
		estimator.addFrame(3000.0 + i * 400.0);
	}
	CHECK(estimator.getEstimatedInterval_ms() == Approx(maxInterval));

	for (int i = 1; i < 200; ++i)
	{
		estimator.addFrame(50000.0 + i * 1.0);
	}
	CHECK(estimator.getEstimatedInterval_ms() == Approx(minInterval));
}

TEST_CASE("NdiTrackerManager: system status bits are described once, when they appear", "[unit][org.custusx.core.tracking.system.ndi]")
{
	CHECK(cx::NdiTrackerManager::describeNewSystemStatus(0, 0).isEmpty());
	CHECK(cx::NdiTrackerManager::describeNewSystemStatus(0, 0x0040) == QStringList({"A tool was plugged in. Restart tracking to use it."}));
	CHECK(cx::NdiTrackerManager::describeNewSystemStatus(0x0040, 0x0040).isEmpty());
	CHECK(cx::NdiTrackerManager::describeNewSystemStatus(0x0040, 0x0050) == QStringList({"Hardware failure"}));
	CHECK(cx::NdiTrackerManager::describeNewSystemStatus(0, 0x0021).size() == 2);
	CHECK(cx::NdiTrackerManager::describeNewSystemStatus(0, 0x8000).isEmpty());
}

TEST_CASE("NdiTrackerManager: a tool plugged in while tracking is set up when tracking starts again", "[unit][org.custusx.core.tracking.system.ndi]")
{
	ManagerFixture fixture;
	fixture.startTracking();
	REQUIRE(fixture.pointer()->getPortHandle() == ManagerFixture::kPointerHandle);

	// The pointer is unplugged and plugged in again: it gets a new port handle
	const int newPointerHandle = 0x0C;
	fixture.device->handles.erase(ManagerFixture::kPointerHandle);
	fixture.device->addWiredHandle(newPointerHandle, 0);
	fixture.device->setNextFrame(1, {createSample(ManagerFixture::kReferenceHandle, true)});
	fixture.device->nextFrame.systemStatus = 0x0040;
	fixture.manager->poll();
	CHECK(fixture.pointer()->getPortHandle() == ManagerFixture::kPointerHandle);

	fixture.manager->trackSlot(false);
	fixture.manager->trackSlot(true);
	CHECK(fixture.pointer()->getPortHandle() == newPointerHandle);
}

TEST_CASE("NdiTrackerManager: a tool removed while tracking is not initialized after the new tool setup", "[unit][org.custusx.core.tracking.system.ndi]")
{
	ManagerFixture fixture;
	fixture.startTracking();
	REQUIRE(fixture.pointer()->isInitialized());

	fixture.device->handles.erase(ManagerFixture::kPointerHandle);
	fixture.device->setNextFrame(1, {createSample(ManagerFixture::kReferenceHandle, true)});
	fixture.device->nextFrame.systemStatus = 0x0080;
	fixture.manager->poll();
	fixture.manager->trackSlot(false);
	fixture.manager->trackSlot(true);

	CHECK_FALSE(fixture.pointer()->isInitialized());
	CHECK(fixture.pointer()->getPortHandle() == -1);
}

TEST_CASE("NdiTrackerManager: the wireless port handles are freed before a new tool setup", "[unit][org.custusx.core.tracking.system.ndi]")
{
	std::shared_ptr<FakeNdiDevice> device(new FakeNdiDevice());
	cx::ToolFileParser::TrackerInternalStructure tracker;
	tracker.mType = cx::tsPOLARIS;
	QTemporaryDir folder;
	QFile rom(folder.path() + "/pointer.rom");
	REQUIRE(rom.open(QIODevice::WriteOnly));
	rom.close();
	cx::ToolFileParser::ToolInternalStructurePtr pointer = createAuroraTool("pointer", 0);
	pointer->mSROMFilename = rom.fileName();
	cx::NdiTrackerManager manager(tracker, {pointer}, cx::ToolFileParser::ToolInternalStructurePtr(), device);
	manager.initializeSlot(true);
	manager.trackSlot(true);
	int firstHandle = manager.getTools()["pointer"]->getPortHandle();
	REQUIRE(firstHandle >= 0);

	device->setNextFrame(1, {createSample(firstHandle, true)});
	device->nextFrame.systemStatus = 0x0020;
	manager.poll();
	manager.trackSlot(false);
	manager.trackSlot(true);

	CHECK(device->freed.count(firstHandle) == 1);
	CHECK(manager.getTools()["pointer"]->getPortHandle() != firstHandle);
	CHECK(manager.getTools()["pointer"]->isInitialized());
}

TEST_CASE("NdiTrackerManager: only new tool change bits trigger a new tool setup", "[unit][org.custusx.core.tracking.system.ndi]")
{
	CHECK(cx::NdiTrackerManager::hasNewToolChange(0, 0x0040));
	CHECK(cx::NdiTrackerManager::hasNewToolChange(0, 0x0080));
	CHECK(cx::NdiTrackerManager::hasNewToolChange(0, 0x0020));
	CHECK_FALSE(cx::NdiTrackerManager::hasNewToolChange(0x0040, 0x0040));
	CHECK_FALSE(cx::NdiTrackerManager::hasNewToolChange(0, 0x0002));
}

TEST_CASE("NdiTrackerManager: polls again at once after repeated frames that the device waited for", "[unit][org.custusx.core.tracking.system.ndi]")
{
	// Polaris: BX waits for the next camera frame, but the tools are updated every few frames
	ManagerFixture fixture;
	fixture.startTracking();
	fixture.device->readDelay_ms = 10;
	fixture.device->setNextFrame(1, {createSample(ManagerFixture::kPointerHandle, true), createSample(ManagerFixture::kReferenceHandle, true)});
	fixture.manager->poll();
	fixture.manager->poll();
	fixture.manager->poll();
	fixture.device->setNextFrame(4, {createSample(ManagerFixture::kPointerHandle, true), createSample(ManagerFixture::kReferenceHandle, true)});
	fixture.manager->poll();
	CHECK(fixture.manager->getCurrentPollInterval_ms() == cx::NdiTrackerManager::kNewFramePollInterval_ms);
}

TEST_CASE("NdiTrackerManager: waits for the next frame after quick repeated frames", "[unit][org.custusx.core.tracking.system.ndi]")
{
	// A device that replies at once with the last frame should not be polled continuously
	ManagerFixture fixture;
	fixture.startTracking();
	fixture.device->setNextFrame(1, {createSample(ManagerFixture::kPointerHandle, true), createSample(ManagerFixture::kReferenceHandle, true)});
	fixture.manager->poll();
	fixture.manager->poll();
	fixture.device->setNextFrame(2, {createSample(ManagerFixture::kPointerHandle, true), createSample(ManagerFixture::kReferenceHandle, true)});
	fixture.manager->poll();
	CHECK(fixture.manager->getCurrentPollInterval_ms() > cx::NdiTrackerManager::kRepeatPollInterval_ms);
}

TEST_CASE("NdiTrackerManager: a repeated frame number is not distributed again", "[unit][org.custusx.core.tracking.system.ndi]")
{
	ManagerFixture fixture;
	fixture.startTracking();
	fixture.device->setNextFrame(7, {createSample(ManagerFixture::kPointerHandle, true), createSample(ManagerFixture::kReferenceHandle, true)});

	fixture.manager->poll();
	fixture.manager->poll();

	CHECK(fixture.pointerTransforms.size() == 1);
}

TEST_CASE("NdiTrackerManager: repeated read failures stop tracking with an error", "[unit][org.custusx.core.tracking.system.ndi]")
{
	ManagerFixture fixture;
	fixture.startTracking();
	fixture.device->setNextFrame(1, {createSample(ManagerFixture::kPointerHandle, true), createSample(ManagerFixture::kReferenceHandle, true)});
	fixture.manager->poll();
	REQUIRE(fixture.pointer()->isVisible());

	fixture.device->failReads = cx::NdiTrackerManager::kMaxConsecutiveReadFailures - 1;
	for (int i = 0; i < cx::NdiTrackerManager::kMaxConsecutiveReadFailures - 1; ++i)
	{
		fixture.manager->poll();
	}
	CHECK(fixture.manager->isTracking()); // occasional failures are tolerated
	CHECK(fixture.errors == 0);
	fixture.device->setNextFrame(2, {createSample(ManagerFixture::kPointerHandle, true), createSample(ManagerFixture::kReferenceHandle, true)});
	fixture.manager->poll(); // a successful read resets the failure count

	fixture.device->failReads = cx::NdiTrackerManager::kMaxConsecutiveReadFailures;
	for (int i = 0; i < cx::NdiTrackerManager::kMaxConsecutiveReadFailures; ++i)
	{
		fixture.manager->poll();
	}
	CHECK_FALSE(fixture.manager->isTracking());
	CHECK(fixture.errors == 1);
	CHECK_FALSE(fixture.pointer()->isVisible());
}

TEST_CASE("NdiTrackerManager: stop tracking hides the tools immediately", "[unit][org.custusx.core.tracking.system.ndi]")
{
	ManagerFixture fixture;
	fixture.startTracking();
	fixture.device->setNextFrame(1, {createSample(ManagerFixture::kPointerHandle, true), createSample(ManagerFixture::kReferenceHandle, true)});
	fixture.manager->poll();
	REQUIRE(fixture.pointer()->isVisible());

	fixture.manager->trackSlot(false);

	CHECK_FALSE(fixture.manager->isTracking());
	CHECK_FALSE(fixture.device->tracking);
	CHECK_FALSE(fixture.pointer()->isVisible());
}

TEST_CASE("NdiTrackerManager: uninitialize stops tracking, frees the port handles and closes the device", "[unit][org.custusx.core.tracking.system.ndi]")
{
	ManagerFixture fixture;
	fixture.startTracking();

	fixture.manager->initializeSlot(false);

	CHECK_FALSE(fixture.manager->isInitialized());
	CHECK_FALSE(fixture.device->tracking);
	CHECK(fixture.device->freed.count(ManagerFixture::kPointerHandle));
	CHECK(fixture.device->freed.count(ManagerFixture::kReferenceHandle));
	CHECK(fixture.device->closeCount >= 1);
	CHECK_FALSE(fixture.pointer()->isInitialized());
	CHECK(fixture.pointer()->getPortHandle() == -1);
}

TEST_CASE("NdiTrackerManager: Polaris tools are wireless, Aurora tools are wired", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::ToolFileParser::ToolInternalStructurePtr structure = createAuroraTool("tool", 2);
	structure->mSROMFilename = "tool.rom";

	cx::NdiToolConfig aurora = cx::NdiTrackerManager::createToolConfig(structure, cx::tsAURORA);
	CHECK_FALSE(aurora.wireless);
	CHECK(aurora.port == 2);

	cx::NdiToolConfig polaris = cx::NdiTrackerManager::createToolConfig(structure, cx::tsPOLARIS_SPECTRA);
	CHECK(polaris.wireless);
	CHECK(polaris.sromFile == "tool.rom");
}

TEST_CASE("NdiTrackerManager: a tool partially out of the volume gets a position warning", "[unit][org.custusx.core.tracking.system.ndi]")
{
	QString warning;
	ManagerFixture fixture;
	QObject::connect(fixture.pointer().get(), &cx::NdiTool::positionWarning, [&warning](QString w) { warning = w; });
	fixture.startTracking();
	cx::NdiToolSample reference = createSample(ManagerFixture::kReferenceHandle, true);

	pollFrames(fixture, 1, createSample(ManagerFixture::kPointerHandle, true), reference);
	CHECK(warning.isEmpty());

	pollFrames(fixture, cx::NdiPositionQuality::kFramesBeforeWarning,
			   createSampleWithStatus(ManagerFixture::kPointerHandle, cx::NdiPortStatus::PartiallyInVolume), reference);
	CHECK(warning == "partially out of the measurement volume");
	CHECK(fixture.pointer()->isVisible());

	pollFrames(fixture, cx::NdiPositionQuality::kFramesBeforeClearing, createSample(ManagerFixture::kPointerHandle, true), reference);
	CHECK(warning.isEmpty());
}

TEST_CASE("NdiTrackerManager: a reference warning is also a warning for the other tools", "[unit][org.custusx.core.tracking.system.ndi]")
{
	QString pointerWarning;
	QString referenceWarning;
	ManagerFixture fixture;
	QObject::connect(fixture.pointer().get(), &cx::NdiTool::positionWarning, [&pointerWarning](QString w) { pointerWarning = w; });
	QObject::connect(fixture.manager->getReferenceTool().get(), &cx::NdiTool::positionWarning, [&referenceWarning](QString w) { referenceWarning = w; });
	fixture.startTracking();

	pollFrames(fixture, cx::NdiPositionQuality::kFramesBeforeWarning, createSample(ManagerFixture::kPointerHandle, true),
			   createSampleWithStatus(ManagerFixture::kReferenceHandle, cx::NdiPortStatus::OutOfVolume));

	CHECK(referenceWarning == "out of the measurement volume");
	CHECK(pointerWarning == "Reference: out of the measurement volume");
}

TEST_CASE("NdiTrackerManager: stop tracking clears the position warning", "[unit][org.custusx.core.tracking.system.ndi]")
{
	QString warning;
	ManagerFixture fixture;
	QObject::connect(fixture.pointer().get(), &cx::NdiTool::positionWarning, [&warning](QString w) { warning = w; });
	fixture.startTracking();
	pollFrames(fixture, cx::NdiPositionQuality::kFramesBeforeWarning,
			   createSampleWithStatus(ManagerFixture::kPointerHandle, cx::NdiPortStatus::OutOfVolume),
			   createSample(ManagerFixture::kReferenceHandle, true));
	REQUIRE_FALSE(warning.isEmpty());

	fixture.manager->trackSlot(false);

	CHECK(warning.isEmpty());
}

TEST_CASE("NdiTrackerManager: combine tool and reference warnings", "[unit][org.custusx.core.tracking.system.ndi]")
{
	CHECK(cx::NdiTrackerManager::combinePositionWarnings("", "").isEmpty());
	CHECK(cx::NdiTrackerManager::combinePositionWarnings("a", "") == "a");
	CHECK(cx::NdiTrackerManager::combinePositionWarnings("", "b") == "Reference: b");
	CHECK(cx::NdiTrackerManager::combinePositionWarnings("a", "b") == "a; reference: b");
}

TEST_CASE("NdiTrackerManager: marker information is requested from Polaris only", "[unit][org.custusx.core.tracking.system.ndi]")
{
	ManagerFixture aurora;
	aurora.manager->initializeSlot(true);
	CHECK_FALSE(aurora.device->lastConnection.requestMarkerInfo);

	CHECK(cx::NdiTrackerManager::isPolaris(cx::tsPOLARIS));
	CHECK(cx::NdiTrackerManager::isPolaris(cx::tsPOLARIS_SPECTRA));
	CHECK(cx::NdiTrackerManager::isPolaris(cx::tsPOLARIS_VICRA));
	CHECK_FALSE(cx::NdiTrackerManager::isPolaris(cx::tsAURORA));
	CHECK(cx::NdiTrackerManager::getDefaultMaxErrorRMS(cx::tsAURORA) == Approx(cx::NdiPositionQuality::kDefaultMaxErrorRMSAurora_mm));
	CHECK(cx::NdiTrackerManager::getDefaultMaxErrorRMS(cx::tsPOLARIS) == Approx(cx::NdiPositionQuality::kDefaultMaxErrorRMSPolaris_mm));
}

TEST_CASE("NdiTrackerManager: the illuminator rate is set on Polaris only", "[unit][org.custusx.core.tracking.system.ndi]")
{
	CHECK(cx::NdiTrackerManager::getIlluminatorRateToSet(cx::tsPOLARIS_SPECTRA, 60) == 60);
	CHECK(cx::NdiTrackerManager::getIlluminatorRateToSet(cx::tsPOLARIS_VICRA, 60) == 60);
	CHECK(cx::NdiTrackerManager::getIlluminatorRateToSet(cx::tsPOLARIS, 0) == 0);
	CHECK(cx::NdiTrackerManager::getIlluminatorRateToSet(cx::tsAURORA, 60) == 0);

	ManagerFixture aurora;
	aurora.manager->initializeSlot(true);
	CHECK(aurora.device->illuminatorRate_Hz == 0);
}

TEST_CASE("NdiTrackerManager: the RMS error limit from the settings is used", "[unit][org.custusx.core.tracking.system.ndi]")
{
	QString warning;
	ManagerFixture fixture;
	QObject::connect(fixture.pointer().get(), &cx::NdiTool::positionWarning, [&warning](QString w) { warning = w; });
	cx::NdiConnectionSettings connection;
	connection.maxErrorRMS_mm = 0.2;
	fixture.manager->setConnectionSettings(connection);
	fixture.startTracking();

	cx::NdiToolSample pointer = createSample(ManagerFixture::kPointerHandle, true);
	pointer.errorRMS_mm = 0.3;
	pollFrames(fixture, cx::NdiPositionQuality::kFramesBeforeWarning, pointer, createSample(ManagerFixture::kReferenceHandle, true));

	CHECK(warning == "RMS error 0.30 (limit 0.20)");
}

} // namespace cxtest
