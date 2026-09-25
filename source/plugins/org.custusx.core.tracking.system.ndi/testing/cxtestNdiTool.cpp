/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include <boost/make_shared.hpp>
#include "cxNdiTool.h"
#include "cxToolFileParser.h"
#include "cxDefinitions.h"
#include "cxTransform3D.h"
#include "cxVector3D.h"

namespace cxtest
{

static cx::ToolFileParser::ToolInternalStructurePtr makeValidStructure(
	const QString& uid = "test_tool_uid",
	bool isReference = false)
{
	cx::ToolFileParser::ToolInternalStructurePtr s =
		boost::make_shared<cx::ToolFileParser::ToolInternalStructure>();
	s->mUid = uid;
	s->mIsPointer = !isReference;
	s->mIsReference = isReference;
	s->mTrackerType = cx::tsPOLARIS;
	return s;
}

TEST_CASE("NdiTool: null structure gives invalid tool", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::NdiTool tool(nullptr);
	CHECK_FALSE(tool.isValid());
	CHECK(tool.getUid().isEmpty());
}

TEST_CASE("NdiTool: valid structure gives valid tool", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::NdiTool tool(makeValidStructure());
	CHECK(tool.isValid());
	CHECK(tool.getUid() == "test_tool_uid");
}

TEST_CASE("NdiTool: isReference returns correct value", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::NdiTool pointer(makeValidStructure("ptr", false));
	cx::NdiTool reference(makeValidStructure("ref", true));

	CHECK_FALSE(pointer.isReference());
	CHECK(reference.isReference());
}

TEST_CASE("NdiTool: port handle initially -1", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::NdiTool tool(makeValidStructure());
	CHECK(tool.getPortHandle() == -1);
}

TEST_CASE("NdiTool: setPortHandle/getPortHandle round-trip", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::NdiTool tool(makeValidStructure());
	tool.setPortHandle(42);
	CHECK(tool.getPortHandle() == 42);
}

TEST_CASE("NdiTool: isInitialized initially false", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::NdiTool tool(makeValidStructure());
	CHECK_FALSE(tool.isInitialized());
}

TEST_CASE("NdiTool: setInitialized changes state", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::NdiTool tool(makeValidStructure());
	tool.setInitialized(true);
	CHECK(tool.isInitialized());
	tool.setInitialized(false);
	CHECK_FALSE(tool.isInitialized());
}

TEST_CASE("NdiTool: isVisible initially false", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::NdiTool tool(makeValidStructure());
	CHECK_FALSE(tool.isVisible());
}

TEST_CASE("NdiTool: updateTransform visible=true emits transform signal", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::NdiTool tool(makeValidStructure());

	bool transformReceived = false;
	cx::Transform3D receivedMatrix = cx::Transform3D::Identity();
	double receivedTimestamp = 0;

	QObject::connect(&tool, &cx::NdiTool::toolTransformAndTimestamp,
		[&](cx::Transform3D matrix, double timestamp, cx::ToolPositionMetadata)
		{
			transformReceived = true;
			receivedMatrix = matrix;
			receivedTimestamp = timestamp;
		});

	cx::Transform3D expected = cx::Transform3D::Identity();
	expected.matrix()(0, 3) = 10.0;

	tool.updateTransform(expected, 1234.5, true);

	CHECK(transformReceived);
	CHECK(cx::similar(receivedMatrix, expected));
	CHECK(receivedTimestamp == Approx(1234.5));
	CHECK(tool.isVisible());
}

TEST_CASE("NdiTool: updateTransform visible=false emits no transform signal", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::NdiTool tool(makeValidStructure());

	bool transformReceived = false;
	bool visibleSignalReceived = false;

	QObject::connect(&tool, &cx::NdiTool::toolTransformAndTimestamp,
		[&](cx::Transform3D, double, cx::ToolPositionMetadata)
		{ transformReceived = true; });

	QObject::connect(&tool, &cx::NdiTool::toolVisible,
		[&](bool) { visibleSignalReceived = true; });

	// Initial state is already false, so no visibility change -> no toolVisible signal
	tool.updateTransform(cx::Transform3D::Identity(), 1000.0, false);

	CHECK_FALSE(transformReceived);
	CHECK_FALSE(visibleSignalReceived);
	CHECK_FALSE(tool.isVisible());
}

TEST_CASE("NdiTool: toolVisible only emitted on visibility change", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::NdiTool tool(makeValidStructure());

	int visibleCount = 0;
	bool lastVisible = false;
	QObject::connect(&tool, &cx::NdiTool::toolVisible,
		[&](bool v) { ++visibleCount; lastVisible = v; });

	// false -> true: emits
	tool.updateTransform(cx::Transform3D::Identity(), 1.0, true);
	CHECK(visibleCount == 1);
	CHECK(lastVisible == true);

	// true -> true: no emit
	tool.updateTransform(cx::Transform3D::Identity(), 2.0, true);
	CHECK(visibleCount == 1);

	// true -> false: only emits after miss tolerance is exhausted
	for (int i = 0; i <= cx::NdiTool::kMissTolerance; ++i)
		tool.updateTransform(cx::Transform3D::Identity(), 3.0 + i, false);
	CHECK(visibleCount == 2);
	CHECK(lastVisible == false);
}

TEST_CASE("NdiTool: brief outage does not toggle visibility", "[unit][org.custusx.core.tracking.system.ndi]")
{
	cx::NdiTool tool(makeValidStructure());

	int visibleCount = 0;
	QObject::connect(&tool, &cx::NdiTool::toolVisible,
		[&](bool) { ++visibleCount; });

	tool.updateTransform(cx::Transform3D::Identity(), 1.0, true); // goes visible
	CHECK(visibleCount == 1);

	// kMissTolerance absent frames: should NOT go invisible
	for (int i = 0; i < cx::NdiTool::kMissTolerance; ++i)
		tool.updateTransform(cx::Transform3D::Identity(), 2.0 + i, false);
	CHECK(visibleCount == 1);
	CHECK(tool.isVisible());

	// One more absent frame: now goes invisible
	tool.updateTransform(cx::Transform3D::Identity(), 2.0 + cx::NdiTool::kMissTolerance, false);
	CHECK(visibleCount == 2);
	CHECK_FALSE(tool.isVisible());
}

} // namespace cxtest
