/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "catch.hpp"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryDir>
#include "cxDefinitions.h"
#include "cxTrackingComparisonWidget.h"

namespace cxtest
{

namespace
{

void createFile(QString path)
{
	QFile file(path);
	file.open(QIODevice::WriteOnly);
}

} // namespace

TEST_CASE("TrackingComparisonWidget: finds each tool configuration once, sorted by name", "[unit][gui]")
{
	QTemporaryDir root;
	QDir(root.path()).mkpath("profile");
	QDir(root.path()).mkpath("installed");
	createFile(root.path() + "/profile/MyTools.xml");
	createFile(root.path() + "/profile/LabDefault.xml");
	createFile(root.path() + "/installed/LabDefault.xml");
	createFile(root.path() + "/installed/LabAuroraTools.xml");
	createFile(root.path() + "/installed/notes.txt");

	QStringList found = cx::TrackingComparisonWidget::findToolConfigurations({root.path() + "/profile", root.path() + "/installed", root.path() + "/missing"});

	REQUIRE(found.size() == 3);
	CHECK(QFileInfo(found[0]).fileName() == "LabAuroraTools.xml");
	CHECK(found[1] == QFileInfo(root.path() + "/profile/LabDefault.xml").absoluteFilePath());
	CHECK(QFileInfo(found[2]).fileName() == "MyTools.xml");
}

TEST_CASE("TrackingComparisonWidget: only the OpenIGTLink run starts the server command", "[unit][gui]")
{
	std::vector<cx::TrackingComparisonRun> runs = cx::TrackingComparisonWidget::createRuns(
				{cx::TRACKING_SYSTEM_IMPLEMENTATION_IGSTK, cx::TRACKING_SYSTEM_IMPLEMENTATION_IGTLINK}, " /opt/ndistreamer ", "-o --tools=a.rom");

	REQUIRE(runs.size() == 2);
	CHECK(runs[0].implementation == cx::TRACKING_SYSTEM_IMPLEMENTATION_IGSTK);
	CHECK(runs[0].command.isEmpty());
	CHECK(runs[1].command == "/opt/ndistreamer");
	CHECK(runs[1].arguments == QStringList({"-o", "--tools=a.rom"}));
}

TEST_CASE("TrackingComparisonWidget: tracking system uids", "[unit][gui]")
{
	CHECK(cx::TrackingComparisonWidget::getTrackingSystemUid(cx::TRACKING_SYSTEM_IMPLEMENTATION_IGSTK) == "org.custusx.core.tracking.system.igstk");
	CHECK(cx::TrackingComparisonWidget::getTrackingSystemUid(cx::TRACKING_SYSTEM_IMPLEMENTATION_NDI) == "org.custusx.core.tracking.system.ndi");
	CHECK(cx::TrackingComparisonWidget::getTrackingSystemUid(cx::TRACKING_SYSTEM_IMPLEMENTATION_IGTLINK) == "org.custusx.core.openigtlink3");
	CHECK(cx::TrackingComparisonWidget::getTrackingSystemUid("unknown").isEmpty());
}

} // namespace cxtest
