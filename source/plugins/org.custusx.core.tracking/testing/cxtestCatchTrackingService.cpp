
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
