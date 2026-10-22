/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include "cxDummyTool.h"
#include "cxTrackingSystemBronchoscopyService.h"

namespace
{
class ToolListSystem : public cx::TrackingSystemService
{
public:
	ToolListSystem(QString uid, std::vector<cx::ToolPtr> tools) : mUid(uid), mTools(tools) {}

	QString getUid() const override { return mUid; }
	void setState(const cx::Tool::State val) override { Q_UNUSED(val); }
	std::vector<cx::ToolPtr> getTools() override { return mTools; }
	cx::TrackerConfigurationPtr getConfiguration() override { return cx::TrackerConfigurationPtr(); }
	cx::ToolPtr getReference() override { return cx::ToolPtr(); }

private:
	QString mUid;
	std::vector<cx::ToolPtr> mTools;
};
} // namespace

TEST_CASE("TrackingSystemBronchoscopyService: the tracking system with the tool is used, whatever its uid", "[unit][org.custusx.bronchoscopynavigation]")
{
	cx::ToolPtr bronchoscope(new cx::DummyTool("Bronchoscope"));
	cx::ToolPtr other(new cx::DummyTool("Other"));
	cx::TrackingSystemServicePtr playback(new ToolListSystem("org.custusx.core.tracking.system.playback", {other}));
	cx::TrackingSystemServicePtr ndi(new ToolListSystem("org.custusx.core.tracking.system.ndi", {bronchoscope}));

	CHECK(cx::TrackingSystemBronchoscopyService::findTrackingSystemWithTool({playback, ndi}, bronchoscope) == ndi);
	CHECK(cx::TrackingSystemBronchoscopyService::findTrackingSystemWithTool({playback, ndi}, other) == playback);
}

TEST_CASE("TrackingSystemBronchoscopyService: no tracking system is found for a missing tool", "[unit][org.custusx.bronchoscopynavigation]")
{
	cx::ToolPtr bronchoscope(new cx::DummyTool("Bronchoscope"));
	cx::TrackingSystemServicePtr igstk(new ToolListSystem("org.custusx.core.tracking.system.igstk", {}));

	CHECK(!cx::TrackingSystemBronchoscopyService::findTrackingSystemWithTool({igstk}, bronchoscope));
	CHECK(!cx::TrackingSystemBronchoscopyService::findTrackingSystemWithTool({igstk}, cx::ToolPtr()));
}
