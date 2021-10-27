/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxOpenIGTLinkTrackingSystemService.h"
#include "cxLogger.h"

#include "cxTrackerConfiguration.h"
#include <QFileInfo>

namespace {
typedef boost::shared_ptr<class TestNetworkHandler> cxtestNetworkHandlerPtr;

class TestNetworkHandler : public cx::NetworkHandler
{
public:
    TestNetworkHandler() : cx::NetworkHandler(nullptr) {}

    double synchronizedTimestamp(double receivedTimestampSec)
    {
        return NetworkHandler::synchronizedTimestamp(receivedTimestampSec);
    }
    bool verifyTimestamp(double &timestampMS)
    {
        return NetworkHandler::verifyTimestamp(timestampMS);
    }
    double getTimestampOffset()
    {
        return mTimestampOffsetMS;
    }
};

class OpenIGTLinkTrackingSystemServiceMoc : public cx::OpenIGTLinkTrackingSystemService
{
public:
    cxtestNetworkHandlerPtr mTestNetworkHandler;

    OpenIGTLinkTrackingSystemServiceMoc() : cx::OpenIGTLinkTrackingSystemService(cx::NetworkHandlerPtr()) {}
    OpenIGTLinkTrackingSystemServiceMoc(cxtestNetworkHandlerPtr testNetworkHandler) :
        mTestNetworkHandler(testNetworkHandler),
        cx::OpenIGTLinkTrackingSystemService(testNetworkHandler) {}

	virtual bool isConfigured() const;
	virtual bool isInitialized() const;
	virtual bool isTracking() const;

	void internalSetState(cx::Tool::State val) {cx::OpenIGTLinkTrackingSystemService::internalSetState(val);}
    double getNetworkHandlerTimestampOffset();


public slots:
	virtual void configure() {cx::OpenIGTLinkTrackingSystemService::configure();}
	virtual void deconfigure() {cx::OpenIGTLinkTrackingSystemService::deconfigure();}

};

bool OpenIGTLinkTrackingSystemServiceMoc::isConfigured() const
{
	return cx::OpenIGTLinkTrackingSystemService::isConfigured();
}

bool OpenIGTLinkTrackingSystemServiceMoc::isInitialized() const
{
	return cx::OpenIGTLinkTrackingSystemService::isInitialized();
}

bool OpenIGTLinkTrackingSystemServiceMoc::isTracking() const
{
	return cx::OpenIGTLinkTrackingSystemService::isTracking();
}

double OpenIGTLinkTrackingSystemServiceMoc::getNetworkHandlerTimestampOffset()
{
    return mTestNetworkHandler->getTimestampOffset();
}

typedef boost::shared_ptr<OpenIGTLinkTrackingSystemServiceMoc> OpenIGTLinkTrackingSystemServiceMocPtr;

} // namespace

namespace cxtest
{

TEST_CASE("OpenIGTLinkTrackingSystemService: Test state transitions", "[plugins][org.custusx.core.openigtlink3][unit]")
{
	OpenIGTLinkTrackingSystemServiceMocPtr trackingSystemService = OpenIGTLinkTrackingSystemServiceMocPtr(new OpenIGTLinkTrackingSystemServiceMoc());

	CHECK(trackingSystemService->getState() == cx::Tool::tsNONE);
	CHECK_FALSE(trackingSystemService->isConfigured());
	CHECK_FALSE(trackingSystemService->isInitialized());
	CHECK_FALSE(trackingSystemService->isTracking());

	// Set up

	trackingSystemService->internalSetState(cx::Tool::tsCONFIGURED);
	CHECK(trackingSystemService->isConfigured());
	CHECK_FALSE(trackingSystemService->isInitialized());
	CHECK_FALSE(trackingSystemService->isTracking());

	trackingSystemService->internalSetState(cx::Tool::tsINITIALIZED);
	CHECK(trackingSystemService->isConfigured());
	CHECK(trackingSystemService->isInitialized());
	CHECK_FALSE(trackingSystemService->isTracking());

	trackingSystemService->internalSetState(cx::Tool::tsTRACKING);
	CHECK(trackingSystemService->isConfigured());
	CHECK(trackingSystemService->isInitialized());
	CHECK(trackingSystemService->isTracking());

	//Take down

	trackingSystemService->internalSetState(cx::Tool::tsINITIALIZED);
	CHECK(trackingSystemService->isConfigured());
	CHECK(trackingSystemService->isInitialized());
	CHECK_FALSE(trackingSystemService->isTracking());

	trackingSystemService->internalSetState(cx::Tool::tsCONFIGURED);
	CHECK(trackingSystemService->isConfigured());
	CHECK_FALSE(trackingSystemService->isInitialized());
	CHECK_FALSE(trackingSystemService->isTracking());

	trackingSystemService->internalSetState(cx::Tool::tsNONE);
	CHECK_FALSE(trackingSystemService->isConfigured());
	CHECK_FALSE(trackingSystemService->isInitialized());
	CHECK_FALSE(trackingSystemService->isTracking());
}


TEST_CASE("NetworkHandler: Test timestamp verification", "[plugins][org.custusx.core.openigtlink3][unit]")
{
    cxtestNetworkHandlerPtr networkHandler = cxtestNetworkHandlerPtr(new TestNetworkHandler());

	qint64 latestSystemTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
	double timestamp = latestSystemTime;

	REQUIRE(networkHandler->verifyTimestamp(timestamp));
	timestamp += 500;
	REQUIRE(networkHandler->verifyTimestamp(timestamp));
	timestamp += 1000;
	REQUIRE_FALSE(networkHandler->verifyTimestamp(timestamp));
	timestamp -= 3000;
	REQUIRE_FALSE(networkHandler->verifyTimestamp(timestamp));
}

TEST_CASE("NetworkHandler: Test timestamp synchronization", "[plugins][org.custusx.core.openigtlink3][unit]")
{
    cxtestNetworkHandlerPtr networkHandler = cxtestNetworkHandlerPtr(new TestNetworkHandler());

	double tolerance = 10;
	qint64 latestSystemTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
	double synchValue = 1000;
	double timestampSec = synchValue;

	double synchedTimeMs = networkHandler->synchronizedTimestamp(timestampSec);
	REQUIRE(cx::similar(synchedTimeMs, latestSystemTime, tolerance));

	double timeIncrementSec = 0.005;

	for(int i = 1; i < 40; ++i)
	{
		timestampSec += timeIncrementSec;
		synchedTimeMs = networkHandler->synchronizedTimestamp(timestampSec);
		{
			double targetTime = latestSystemTime + timeIncrementSec*1000*i;
			INFO("iteration: "+qstring_cast(i)+" "+qstring_cast(synchedTimeMs)+" == "+qstring_cast(targetTime));
			REQUIRE(cx::similar(synchedTimeMs, targetTime, tolerance));
		}
	}

	//Adding a difference from system time above 1 sec will cause NetworkHandler::verifyTimestamp to change timestamp to system time
	timestampSec += 1;
	latestSystemTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
	synchedTimeMs = networkHandler->synchronizedTimestamp(timestampSec);
	INFO(qstring_cast(synchedTimeMs)+" == "+qstring_cast(latestSystemTime));
	REQUIRE(cx::similar(synchedTimeMs, latestSystemTime, tolerance));

	timestampSec -= 3;
	latestSystemTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
	synchedTimeMs = networkHandler->synchronizedTimestamp(timestampSec);
	REQUIRE(cx::similar(synchedTimeMs, latestSystemTime, tolerance));
}

TEST_CASE("NetworkHandler: Test timestamp synchronization reset", "[plugins][org.custusx.core.openigtlink3][unit]")
{
    cxtestNetworkHandlerPtr networkHandler = cxtestNetworkHandlerPtr(new TestNetworkHandler());

	double tolerance = 10;
    qint64 latestSystemTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
	double timestampSec = 1000;

	double synchedTimeMs = networkHandler->synchronizedTimestamp(timestampSec);
    REQUIRE(cx::similar(synchedTimeMs, latestSystemTime, tolerance));


    double offset = latestSystemTime - timestampSec*1000;
    {
        INFO(qstring_cast(offset)+"=="+qstring_cast(networkHandler->getTimestampOffset()));
        REQUIRE(cx::similar(offset, networkHandler->getTimestampOffset(), tolerance));
    }

    networkHandler->clearTimestampSynchronization();
    REQUIRE_FALSE(cx::similar(offset, networkHandler->getTimestampOffset(), tolerance));
    REQUIRE(cx::similar(0, networkHandler->getTimestampOffset(), tolerance));


    timestampSec = 3000;
    latestSystemTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    synchedTimeMs = networkHandler->synchronizedTimestamp(timestampSec);
    REQUIRE(cx::similar(synchedTimeMs, latestSystemTime, tolerance));

    offset = latestSystemTime - timestampSec*1000;
    REQUIRE(cx::similar(offset, networkHandler->getTimestampOffset(), tolerance));
}

TEST_CASE("OpenIGTLinkTrackingSystemService: Test timestamp synchronization", "[plugins][org.custusx.core.openigtlink3][unit]")
{
    cxtestNetworkHandlerPtr networkHandler = cxtestNetworkHandlerPtr(new TestNetworkHandler());
    OpenIGTLinkTrackingSystemServiceMocPtr trackingSystemService = OpenIGTLinkTrackingSystemServiceMocPtr(new OpenIGTLinkTrackingSystemServiceMoc(networkHandler));

    double tolerance = 10;
    qint64 latestSystemTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    double timestampSec = 1000;

    double synchedTimeMs = trackingSystemService->mTestNetworkHandler->synchronizedTimestamp(timestampSec);
    REQUIRE(cx::similar(synchedTimeMs, latestSystemTime, tolerance));

    REQUIRE_FALSE(cx::similar(0, networkHandler->getTimestampOffset(), tolerance));
    trackingSystemService->resetTimeSynchronization();
    REQUIRE(cx::similar(0, networkHandler->getTimestampOffset(), tolerance));
}

#ifdef CX_CUSTUS_SINTEF
TEST_CASE("OpenIGTLinkTrackingSystemService: Test configure state using tool config file for Plus", "[plugins][org.custusx.core.openigtlink3][integration]")
{
	OpenIGTLinkTrackingSystemServiceMocPtr trackingSystemService = OpenIGTLinkTrackingSystemServiceMocPtr(new OpenIGTLinkTrackingSystemServiceMoc());

	cx::TrackerConfigurationPtr config = trackingSystemService->getConfiguration();
	REQUIRE(config);
	QStringList configurations = config->getAllConfigurations();

	REQUIRE(configurations.size() > 1); //Test require that at least one tool configuration is present. Normally there should be many

	//Use one of the tool config files with Plus tools
	int posOfPlusConfigToolFile = 0;
	bool foundPlusToolConfigFile = false;
	for(int i = 0; i < configurations.size(); ++i)
	{
		QFileInfo fileInfo(configurations[i]);
		if (fileInfo.fileName().startsWith("PLUS"))
		{
//			CX_LOG_DEBUG() << "Found Plus tool config file: " << configurations[i];
			posOfPlusConfigToolFile = i;
			foundPlusToolConfigFile = true;
		}
//		else
//			CX_LOG_DEBUG() << "Tool config file (not Plus): " << configurations[i];
	}

	REQUIRE(foundPlusToolConfigFile);// Test require that at leat one tool configuration using Plus is present.
	QString toolConfigFile = configurations[posOfPlusConfigToolFile];
	CX_LOG_DEBUG() << "Using Plus tool config file: " << toolConfigFile;
	trackingSystemService->setConfigurationFile(toolConfigFile);

	//Test real configure/deconfigure functions
	trackingSystemService->configure();
	CHECK(trackingSystemService->isConfigured());//Wait for stateChanged?

	trackingSystemService->deconfigure();
	CHECK_FALSE(trackingSystemService->isConfigured());//Wait for stateChanged?
}
#endif

}//cxtest
