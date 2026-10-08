/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include <QDateTime>
#include <QDir>
#include <QEventLoop>
#include <QTimer>
#include "cxDataLocations.h"
#include "cxDefinitions.h"
#include "cxLogicManager.h"
#include "cxProfile.h"
#include "cxTrackingService.h"
#include "cxTrackingSystemComparison.h"
#include "cxTime.h"

namespace cxtest
{

namespace
{

QString getEnvironment(const char* name, QString defaultValue = QString())
{
	QString value = qEnvironmentVariable(name);
	return value.isEmpty() ? defaultValue : value;
}

QStringList getSystems(QString defaultSystems)
{
	QStringList retval;
	for (const QString& system : getEnvironment("CX_TRACKING_TEST_SYSTEMS", defaultSystems).split(","))
	{
		if (!system.trimmed().isEmpty())
		{
			retval << system.trimmed();
		}
	}
	return retval;
}

/** Settings from the CX_TRACKING_TEST_* environment variables. */
cx::TrackingComparisonSettings createSettings(QString testName)
{
	cx::TrackingComparisonSettings retval;
	retval.toolConfigFile = getEnvironment("CX_TRACKING_TEST_CONFIG");
	QString folder = getEnvironment("CX_TRACKING_TEST_OUTPUT", QDir::homePath() + "/cx_tracking_tests");
	retval.outputFolder = folder + "/" + testName + "_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
	retval.recordDuration_ms = getEnvironment("CX_TRACKING_TEST_DURATION_S", "10").toInt() * 1000;
	retval.openIGTLinkApplyReference = getEnvironment("CX_TRACKING_TEST_IGTL_APPLY_REF", "1") != "0";
	retval.openIGTLinkHost = getEnvironment("CX_TRACKING_TEST_IGTL_HOST", "127.0.0.1");
	retval.openIGTLinkPort = getEnvironment("CX_TRACKING_TEST_IGTL_PORT", "18944").toInt();
	return retval;
}

cx::TrackingComparisonRun createRun(QString implementation)
{
	cx::TrackingComparisonRun retval;
	retval.implementation = implementation;
	if (implementation == cx::TRACKING_SYSTEM_IMPLEMENTATION_IGTLINK)
	{
		retval.command = getEnvironment("CX_TRACKING_TEST_IGTL_COMMAND");
		retval.arguments = cx::TrackingSystemComparison::splitArguments(getEnvironment("CX_TRACKING_TEST_IGTL_ARGS"));
	}
	return retval;
}

int getMaxDuration_ms(const cx::TrackingComparisonSettings& settings)
{
	int perRun = settings.commandStartupDelay_ms + settings.startTimeout_ms + settings.settleTime_ms
			+ settings.recordDuration_ms + settings.pauseBetweenRuns_ms + 20000;
	return static_cast<int>(settings.runs.size()) * perRun;
}

/** Run the comparison with the LogicManager's services, return true if all runs got positions. */
bool runComparison(cx::TrackingComparisonSettings settings, double* maxMeanDifference_mm = nullptr, int* comparedTools = nullptr)
{
	INFO("Set CX_TRACKING_TEST_CONFIG to the tool configuration file");
	REQUIRE(QFile::exists(settings.toolConfigFile));

	cx::TrackingSystemComparison comparison(cx::logicManager()->getTrackingService());
	QEventLoop loop;
	QObject::connect(&comparison, &cx::TrackingSystemComparison::finished, &loop, &QEventLoop::quit);
	QTimer::singleShot(getMaxDuration_ms(settings), &loop, &QEventLoop::quit);
	comparison.start(settings);
	loop.exec();
	comparison.cancel();

	if (maxMeanDifference_mm)
	{
		*maxMeanDifference_mm = comparison.getMaxMeanDifference_mm();
	}
	if (comparedTools)
	{
		*comparedTools = comparison.getComparedToolCount();
	}
	INFO(comparison.getSummary().toStdString());
	WARN("Results in " << settings.outputFolder.toStdString() << ":\n" << comparison.getSummary().toStdString());
	return comparison.wasSuccessful();
}

void wait_ms(int duration_ms)
{
	QEventLoop loop;
	QTimer::singleShot(duration_ms, &loop, &QEventLoop::quit);
	loop.exec();
}

bool waitForTrackingState(cx::Tool::State state, int timeout_ms)
{
	double deadline = cx::getMilliSecondsSinceEpoch() + timeout_ms;
	while (cx::logicManager()->getTrackingService()->getState() != state && cx::getMilliSecondsSinceEpoch() < deadline)
	{
		wait_ms(100);
	}
	return cx::logicManager()->getTrackingService()->getState() == state;
}

} // namespace

TEST_CASE("Tracking hardware: compare tracking systems on the same tools", "[manual][hide][tracking_hardware]")
{
	cx::DataLocations::setTestMode();
	cx::LogicManager::initialize();

	cx::TrackingComparisonSettings settings = createSettings("compare");
	for (const QString& system : getSystems("igstk,ndi,openigtlink"))
	{
		settings.runs.push_back(createRun(system));
	}
	double maxDifference_mm = 0.0;
	int comparedTools = 0;
	CHECK(runComparison(settings, &maxDifference_mm, &comparedTools));
	if (settings.runs.size() > 1)
	{
		CHECK(comparedTools > 0);
	}
	double limit_mm = getEnvironment("CX_TRACKING_TEST_MAX_DIFF_MM", "1.0").toDouble();
	INFO("The tools must be kept still during the comparison");
	CHECK(maxDifference_mm <= limit_mm);

	cx::LogicManager::shutdown();
}

TEST_CASE("Tracking hardware: start and stop tracking several times", "[manual][hide][tracking_hardware]")
{
	cx::DataLocations::setTestMode();
	cx::LogicManager::initialize();

	cx::TrackingComparisonSettings settings = createSettings("restart");
	settings.recordDuration_ms = 3000;
	int repeats = getEnvironment("CX_TRACKING_TEST_REPEATS", "5").toInt();
	for (const QString& system : getSystems("ndi"))
	{
		for (int i = 0; i < repeats; ++i)
		{
			settings.runs.push_back(createRun(system));
		}
	}
	CHECK(runComparison(settings));

	cx::LogicManager::shutdown();
}

TEST_CASE("Tracking hardware: long run", "[manual][hide][tracking_hardware]")
{
	cx::DataLocations::setTestMode();
	cx::LogicManager::initialize();

	cx::TrackingComparisonSettings settings = createSettings("long_run");
	settings.recordDuration_ms = getEnvironment("CX_TRACKING_TEST_LONG_DURATION_S", "3600").toInt() * 1000;
	for (const QString& system : getSystems("ndi"))
	{
		settings.runs.push_back(createRun(system));
	}
	CHECK(runComparison(settings));

	cx::LogicManager::shutdown();
}

TEST_CASE("Tracking hardware: shut down while tracking, then track again", "[manual][hide][tracking_hardware]")
{
	QString config = getEnvironment("CX_TRACKING_TEST_CONFIG");
	INFO("Set CX_TRACKING_TEST_CONFIG to the tool configuration file (NDI or IGSTK)");
	REQUIRE(QFile::exists(config));

	for (int i = 0; i < 2; ++i)
	{
		cx::DataLocations::setTestMode();
		cx::LogicManager::initialize();
		cx::profile()->setToolConfigFilePath(config);
		cx::logicManager()->getTrackingService()->setState(cx::Tool::tsTRACKING);
		CHECK(waitForTrackingState(cx::Tool::tsTRACKING, 30000));
		wait_ms(3000);

		double start = cx::getMilliSecondsSinceEpoch();
		cx::LogicManager::shutdown();
		WARN("Shutdown " << i + 1 << " while tracking took " << cx::getMilliSecondsSinceEpoch() - start << " ms");
	}
}

} // namespace cxtest
