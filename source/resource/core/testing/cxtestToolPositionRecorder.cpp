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
#include <QRegExp>
#include <QTemporaryDir>
#include <QTextStream>
#include "cxDummyTool.h"
#include "cxToolPositionRecorder.h"
#include "cxTrackingSystemComparison.h"
#include "cxTransform3D.h"

namespace cxtest
{

namespace
{

QStringList readLines(QString filename)
{
	QStringList retval;
	QFile file(filename);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QTextStream stream(&file);
		while (!stream.atEnd())
		{
			retval << stream.readLine();
		}
	}
	return retval;
}

} // namespace

TEST_CASE("ToolPositionStatistics: mean, standard deviation and rate", "[unit][tool]")
{
	cx::ToolPositionStatistics statistics;
	statistics.addPosition(cx::Vector3D(1, 0, 10), 1000, false);
	statistics.addPosition(cx::Vector3D(3, 0, 10), 1050, true);
	statistics.addPosition(cx::Vector3D(5, 0, 10), 1100, false);

	CHECK(statistics.positions == 3);
	CHECK(statistics.positionsWithWarning == 1);
	CHECK(cx::similar(statistics.mean, cx::Vector3D(3, 0, 10)));
	CHECK(cx::similar(statistics.getStandardDeviation(), cx::Vector3D(2, 0, 0)));
	CHECK(statistics.getRate_Hz() == Approx(20.0));
}

TEST_CASE("ToolPositionStatistics: drift between the first and the last second", "[unit][tool]")
{
	cx::ToolPositionStatistics statistics;
	for (int i = 0; i <= 100; ++i)
	{
		double time_ms = 1000.0 + 50.0 * i;
		statistics.addPosition(cx::Vector3D(time_ms / 1000.0, 0, 0), time_ms, false);
	}
	// first second: mean x 1.5, last second: mean x 5.5
	CHECK(statistics.getDrift_mm() == Approx(4.0));

	cx::ToolPositionStatistics still;
	for (int i = 0; i < 100; ++i)
	{
		still.addPosition(cx::Vector3D(1, 2, 3), 50.0 * i, false);
	}
	CHECK(still.getDrift_mm() == Approx(0.0));
}

TEST_CASE("ToolPositionStatistics: no rate or spread from a single position", "[unit][tool]")
{
	cx::ToolPositionStatistics statistics;
	statistics.addPosition(cx::Vector3D(1, 2, 3), 1000, false);
	CHECK(statistics.getRate_Hz() == Approx(0.0));
	CHECK(cx::similar(statistics.getStandardDeviation(), cx::Vector3D::Zero()));
}

TEST_CASE("ToolPositionRecorder: rows are CSV with quoted text", "[unit][tool]")
{
	CHECK(cx::ToolPositionRecorder::quoteCsv("plain") == "plain");
	CHECK(cx::ToolPositionRecorder::quoteCsv("a, b") == "\"a, b\"");
	CHECK(cx::ToolPositionRecorder::quoteCsv("say \"hi\"") == "\"say \"\"hi\"\"\"");

	cx::Transform3D prMt = cx::createTransformTranslate(cx::Vector3D(1, 2, 3));
	QString row = cx::ToolPositionRecorder::formatRow("1_ndi", "pointer", "position", 1000.5, 1001.5, true, prMt,
													  "partially out of the measurement volume, RMS error 0.60 (limit 0.50)", "errorRMS_mm:0.6");
	CHECK(row == "1_ndi,pointer,position,1000.500,1001.500,1,1.0000,2.0000,3.0000,"
				 "1.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,1.000000,"
				 "\"partially out of the measurement volume, RMS error 0.60 (limit 0.50)\",errorRMS_mm:0.6");
	CHECK(cx::ToolPositionRecorder::getHeader().split(",").size() == 20);
}

TEST_CASE("ToolPositionRecorder: records positions and visibility of the tools", "[unit][tool]")
{
	QTemporaryDir folder;
	cx::DummyToolPtr tool(new cx::DummyTool("pointer"));
	cx::ToolPositionRecorder recorder;
	recorder.setTools({tool});
	QString filename = folder.path() + "/positions.csv";

	REQUIRE(recorder.start(filename, "test"));
	tool->setVisible(true);
	tool->set_prMt(cx::createTransformTranslate(cx::Vector3D(1, 2, 3)));
	tool->set_prMt(cx::createTransformTranslate(cx::Vector3D(3, 2, 3)));
	recorder.stop();
	tool->set_prMt(cx::createTransformTranslate(cx::Vector3D(100, 2, 3))); // not recorded

	QStringList lines = readLines(filename);
	REQUIRE(lines.size() == 1 + recorder.getRowCount());
	CHECK(lines[0] == cx::ToolPositionRecorder::getHeader());
	CHECK(lines.filter(QRegExp("^test,pointer,position,")).size() == 2);
	CHECK(lines.filter(QRegExp("^test,pointer,visible,")).size() == 1);
	CHECK(lines[1].startsWith("test,pointer,"));

	std::map<QString, cx::ToolPositionStatistics> statistics = recorder.getStatistics();
	REQUIRE(statistics.count("pointer"));
	CHECK(statistics["pointer"].positions == 2);
	CHECK(cx::similar(statistics["pointer"].mean, cx::Vector3D(2, 2, 3)));
	CHECK(recorder.getSummary().startsWith("test pointer: 2 positions"));
}

TEST_CASE("ToolPositionRecorder: start fails for a file that can't be written", "[unit][tool]")
{
	cx::ToolPositionRecorder recorder;
	CHECK_FALSE(recorder.start("/nonexistent_folder/positions.csv", "test"));
	CHECK_FALSE(recorder.isRecording());
}

TEST_CASE("TrackingSystemComparison: replaces the tracking system implementation in a tool configuration", "[unit][tool]")
{
	QString config = "<?xml version = \"1.0\"?>\n"
					 "<configuration clinical_app=\"Laboratory\">\n"
					 "<trackingsystemimplementation type=\"igstk\">\n"
					 "<tracker type=\"polaris\"><toolfile reference=\"yes\">Shared/ref.xml</toolfile></tracker>\n"
					 "</trackingsystemimplementation>\n"
					 "</configuration>\n";

	QString ndi = cx::TrackingSystemComparison::setTrackingSystemImplementation(config, "ndi");
	CHECK(ndi.contains("<trackingsystemimplementation type=\"ndi\">"));
	CHECK(ndi.contains("Shared/ref.xml"));
	CHECK_FALSE(ndi.contains("igstk"));

	CHECK_FALSE(ndi.contains("openigtlinkApplyRefToTools"));

	QString openigtlink = cx::TrackingSystemComparison::setTrackingSystemImplementation(config, "openigtlink", true);
	CHECK(openigtlink.contains("type=\"openigtlink\""));
	CHECK(openigtlink.contains("openigtlinkApplyRefToTools=\"yes\""));
	CHECK(cx::TrackingSystemComparison::setTrackingSystemImplementation(config, "openigtlink", false).contains("openigtlinkApplyRefToTools=\"no\""));

	CHECK(cx::TrackingSystemComparison::setTrackingSystemImplementation("<configuration/>", "ndi").isEmpty());
	CHECK(cx::TrackingSystemComparison::setTrackingSystemImplementation("not xml", "ndi").isEmpty());
	CHECK(cx::TrackingSystemComparison::getRunLabel(0, "igstk") == "1_igstk");
}

TEST_CASE("TrackingSystemComparison: compares the mean positions of the tools between runs", "[unit][tool]")
{
	cx::ToolPositionStatistics pointer;
	pointer.addPosition(cx::Vector3D(10, 0, 0), 0, false);
	cx::ToolPositionStatistics movedPointer;
	movedPointer.addPosition(cx::Vector3D(10, 3, 4), 0, false);
	cx::ToolPositionStatistics reference;
	reference.reference = true;
	reference.addPosition(cx::Vector3D(0, 0, 0), 0, false);
	cx::ToolPositionStatistics movedReference = reference;
	movedReference.mean = cx::Vector3D(100, 0, 0);

	cx::TrackingComparisonResult first;
	first.label = "1_igstk";
	first.statistics = {{"pointer", pointer}, {"reference", reference}, {"probe", pointer}};
	cx::TrackingComparisonResult second;
	second.label = "2_ndi";
	second.statistics = {{"pointer", movedPointer}, {"reference", movedReference}};

	std::map<QString, double> differences = cx::TrackingSystemComparison::getMeanDifferences(first.statistics, second.statistics);
	REQUIRE(differences.size() == 1);
	CHECK(differences["pointer"] == Approx(5.0));

	QStringList lines = cx::TrackingSystemComparison::compareRuns({first, second});
	REQUIRE(lines.size() == 2);
	CHECK(lines[0] == "2_ndi pointer: mean position 5.000 mm from 1_igstk");
	CHECK(lines[1] == "2_ndi probe: no positions, not compared with 1_igstk");
	CHECK(cx::TrackingSystemComparison::compareRuns({first}).isEmpty());
}

TEST_CASE("TrackingSystemComparison: result table has one row per tool and run, without the reference", "[unit][tool]")
{
	cx::ToolPositionStatistics pointer;
	pointer.addPosition(cx::Vector3D(10, 0, 0), 0, false);
	pointer.addPosition(cx::Vector3D(10, 0, 0), 100, true);
	cx::ToolPositionStatistics movedPointer;
	movedPointer.addPosition(cx::Vector3D(10, 3, 4), 0, false);
	cx::ToolPositionStatistics reference;
	reference.reference = true;
	reference.addPosition(cx::Vector3D(0, 0, 0), 0, false);

	cx::TrackingComparisonResult first;
	first.label = "1_igstk";
	first.statistics = {{"pointer", pointer}, {"reference", reference}};
	cx::TrackingComparisonResult second;
	second.label = "2_ndi";
	second.statistics = {{"pointer", movedPointer}, {"reference", reference}};

	std::vector<QStringList> rows = cx::TrackingSystemComparison::createResultTable({first, second});
	REQUIRE(rows.size() == 2);
	CHECK(rows[0].size() == cx::TrackingSystemComparison::getResultTableHeader().size());
	CHECK(rows[0][0] == "1_igstk");
	CHECK(rows[0][1] == "pointer");
	CHECK(rows[0][3] == "2");
	CHECK(rows[0][4] == "1");
	CHECK(rows[0][7] == "-");
	CHECK(rows[1][0] == "2_ndi");
	CHECK(rows[1][7] == "5.000");
}

TEST_CASE("TrackingSystemComparison: a run needs positions from a tool other than the reference", "[unit][tool]")
{
	cx::ToolPositionStatistics reference;
	reference.reference = true;
	reference.addPosition(cx::Vector3D(0, 0, 0), 0, false);
	cx::ToolPositionStatistics hiddenPointer;
	cx::ToolPositionStatistics pointer;
	pointer.addPosition(cx::Vector3D(1, 0, 0), 0, false);

	CHECK_FALSE(cx::TrackingSystemComparison::hasTrackedTool({}));
	CHECK_FALSE(cx::TrackingSystemComparison::hasTrackedTool({{"reference", reference}, {"pointer", hiddenPointer}}));
	CHECK(cx::TrackingSystemComparison::hasTrackedTool({{"reference", reference}, {"pointer", pointer}}));
}

TEST_CASE("TrackingSystemComparison: split command arguments", "[unit][tool]")
{
	CHECK(cx::TrackingSystemComparison::splitArguments("").isEmpty());
	CHECK(cx::TrackingSystemComparison::splitArguments("  -o  --hostname=/dev/ttyUSB0 ") == QStringList({"-o", "--hostname=/dev/ttyUSB0"}));
	CHECK(cx::TrackingSystemComparison::splitArguments("--tools=\"my tool.rom\" -v") == QStringList({"--tools=my tool.rom", "-v"}));
}

TEST_CASE("TrackingSystemComparison: expandHomeFolder replaces a leading tilde", "[unit]")
{
	CHECK(cx::TrackingSystemComparison::expandHomeFolder("~/build/server") == QDir::homePath() + "/build/server");
	CHECK(cx::TrackingSystemComparison::expandHomeFolder("~") == QDir::homePath());
	CHECK(cx::TrackingSystemComparison::expandHomeFolder("/opt/server") == "/opt/server");
	CHECK(cx::TrackingSystemComparison::expandHomeFolder("~user/server") == "~user/server");
	CHECK(cx::TrackingSystemComparison::expandHomeFolder("server") == "server");
}

TEST_CASE("TrackingSystemComparison: getWorkingFolder is the program's own folder", "[unit]")
{
	QString folder = QDir::tempPath() + "/server/bin";
	CHECK(cx::TrackingSystemComparison::getWorkingFolder(folder + "/server") == folder);
	CHECK(cx::TrackingSystemComparison::getWorkingFolder(QDir::toNativeSeparators(folder + "/server")) == folder);
	CHECK(cx::TrackingSystemComparison::getWorkingFolder("server").isEmpty());
}

} // namespace cxtest
