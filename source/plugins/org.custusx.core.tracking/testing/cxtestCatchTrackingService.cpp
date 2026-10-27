
/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include "cxDummyToolManager.h"
#include "cxTrackingImplService.h"
#include "cxTrackingSystemService.h"

namespace
{
class FallbackRecordingSystem : public cx::TrackingSystemService
{
public:
	QString mFallbackConfigurationFile;

	QString getUid() const override { return "fallback_recording_system"; }
	void setState(const cx::Tool::State val) override { Q_UNUSED(val); }
	std::vector<cx::ToolPtr> getTools() override { return std::vector<cx::ToolPtr>(); }
	cx::TrackerConfigurationPtr getConfiguration() override { return cx::TrackerConfigurationPtr(); }
	cx::ToolPtr getReference() override { return cx::ToolPtr(); }
	void startFallbackTracking(QString configurationFile) override { mFallbackConfigurationFile = configurationFile; }
};

/** Configures as a tracking system does: accepts only its own implementation, and configures in a thread when it does */
class ConfigurationTestSystem : public cx::TrackingSystemService
{
public:
	enum Behaviour { DECLINE, ACCEPT_IN_THREAD, CONFIGURE_AT_ONCE };

	ConfigurationTestSystem(Behaviour behaviour, cx::Tool::State state = cx::Tool::tsNONE) : mBehaviour(behaviour)
	{
		mState = state;
	}
	QString getUid() const override { return "configuration_test_system"; }
	void setState(const cx::Tool::State val) override
	{
		if (val < cx::Tool::tsCONFIGURED || mState >= cx::Tool::tsCONFIGURED || mBehaviour == DECLINE)
			return;
		if (mBehaviour == ACCEPT_IN_THREAD)
			emit updateTrackingSystemImplementation("test"); // configured later
		else
			mState = val;
	}
	std::vector<cx::ToolPtr> getTools() override { return std::vector<cx::ToolPtr>(); }
	cx::TrackerConfigurationPtr getConfiguration() override { return cx::TrackerConfigurationPtr(); }
	cx::ToolPtr getReference() override { return cx::ToolPtr(); }

private:
	Behaviour mBehaviour;
};
typedef std::shared_ptr<ConfigurationTestSystem> ConfigurationTestSystemPtr;
} // namespace


TEST_CASE("DummyToolManager setup/shutdown works multiple times", "[unit][org.custus.core.tracking]")
{
    for (unsigned i=0; i<2; ++i)
    {
		cx::TrackingServicePtr service = cx::DummyToolManager::create();
		REQUIRE(service);
		CHECK(service.unique());
		service.reset();
    }
}

TEST_CASE("TrackingImplService: the other tracking systems are asked to track the tools of a device in use", "[unit][org.custus.core.tracking]")
{
	std::shared_ptr<FallbackRecordingSystem> deviceInUse(new FallbackRecordingSystem());
	std::shared_ptr<FallbackRecordingSystem> other(new FallbackRecordingSystem());
	std::vector<cx::TrackingSystemServicePtr> systems = {deviceInUse, other};

	cx::TrackingImplService::startFallbackTracking(systems, deviceInUse.get(), "tools.xml");

	CHECK(deviceInUse->mFallbackConfigurationFile.isEmpty());
	CHECK(other->mFallbackConfigurationFile == "tools.xml");
}

TEST_CASE("TrackingImplService: configuring is reported as failed when no tracking system accepts the configuration", "[unit][org.custus.core.tracking]")
{
	typedef ConfigurationTestSystem System;
	ConfigurationTestSystemPtr igstk(new System(System::DECLINE));
	ConfigurationTestSystemPtr openigtlink(new System(System::DECLINE));
	std::vector<cx::TrackingSystemServicePtr> systems = {igstk, openigtlink};

	CHECK_FALSE(cx::TrackingImplService::setStateOfTrackingSystems(systems, cx::Tool::tsCONFIGURED));
	CHECK_FALSE(cx::TrackingImplService::setStateOfTrackingSystems(systems, cx::Tool::tsTRACKING));
	CHECK_FALSE(cx::TrackingImplService::setStateOfTrackingSystems(std::vector<cx::TrackingSystemServicePtr>(), cx::Tool::tsCONFIGURED));
}

TEST_CASE("TrackingImplService: configuring is not reported as failed when a tracking system accepts the configuration", "[unit][org.custus.core.tracking]")
{
	typedef ConfigurationTestSystem System;
	ConfigurationTestSystemPtr declining(new System(System::DECLINE));

	SECTION("configuring in a thread")
	{
		ConfigurationTestSystemPtr ndi(new System(System::ACCEPT_IN_THREAD));
		std::vector<cx::TrackingSystemServicePtr> systems = {declining, ndi};
		CHECK(cx::TrackingImplService::setStateOfTrackingSystems(systems, cx::Tool::tsCONFIGURED));
		CHECK(ndi->getState() == cx::Tool::tsNONE);
	}
	SECTION("configuring at once")
	{
		ConfigurationTestSystemPtr dummy(new System(System::CONFIGURE_AT_ONCE));
		std::vector<cx::TrackingSystemServicePtr> systems = {declining, dummy};
		CHECK(cx::TrackingImplService::setStateOfTrackingSystems(systems, cx::Tool::tsCONFIGURED));
		CHECK(dummy->getState() == cx::Tool::tsCONFIGURED);
	}
}

TEST_CASE("TrackingImplService: configuring is not reported as failed when not asked for", "[unit][org.custus.core.tracking]")
{
	typedef ConfigurationTestSystem System;
	ConfigurationTestSystemPtr declining(new System(System::DECLINE));
	ConfigurationTestSystemPtr configured(new System(System::DECLINE, cx::Tool::tsCONFIGURED));

	CHECK(cx::TrackingImplService::setStateOfTrackingSystems({declining}, cx::Tool::tsNONE));
	CHECK(cx::TrackingImplService::setStateOfTrackingSystems({declining, configured}, cx::Tool::tsTRACKING));
}
