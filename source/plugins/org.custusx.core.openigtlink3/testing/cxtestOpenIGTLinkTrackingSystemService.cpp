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
#include "cxOpenIGTLinkTool.h"
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
	cx::OpenIGTLinkToolPtr getTool(QString devicename) {return cx::OpenIGTLinkTrackingSystemService::getTool(devicename);}
	QStringList getTransformIdWarningPrinted() {return mTransformIdWarningPrinted;}
	bool testPrintTransformIdWarning(QString devicename) {return printTransformIdWarning(devicename);}

public slots:
	virtual void configure() {cx::OpenIGTLinkTrackingSystemService::configure();}
	virtual void deconfigure() {cx::OpenIGTLinkTrackingSystemService::deconfigure();}
	virtual void receiveTransform(QString devicename, cx::Transform3D transform, double timestampMS)
	{
		cx::OpenIGTLinkTrackingSystemService::receiveTransform(devicename, transform, timestampMS);
	}

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

class PositionReceiver : public QObject
{
public:
	PositionReceiver(QString devicename, cx::OpenIGTLinkToolPtr tool, QObject* parent = NULL) :
		QObject(parent),
		mDevicename(devicename),
		mTool(tool)
	{
		connect(tool.get(), &cx::Tool::toolTransformAndTimestamp, this, &PositionReceiver::toolTransformAndTimestampSlot);
	}
	QString mDevicename;
	cx::OpenIGTLinkToolPtr mTool;
	cx::Transform3D mPrMs;
	double mTimestampMS;

public slots:
	void toolTransformAndTimestampSlot(cx::Transform3D prMs, double timestampMS)
	{
		//CX_LOG_DEBUG() << "toolTransformAndTimestampSlot mDevicename: " << mDevicename << " timestampMS: " << timestampMS << " prMs:\n" << prMs;
		mPrMs = prMs;
		mTimestampMS = timestampMS;
	}
};

typedef boost::shared_ptr<PositionReceiver> PositionReceiverPtr;

bool findConfigFileNumberInList(QStringList configurations, QString startsWith, int& numberInList)
{
	//Find the first tool config file starting with the input string
	 numberInList = 0;
	bool foundPlusToolConfigFile = false;
	for(int i = 0; i < configurations.size(); ++i)
	{
		QFileInfo fileInfo(configurations[i]);
		if (fileInfo.fileName().startsWith(startsWith))
		{
//			CX_LOG_DEBUG() << "Found tool config file: " << configurations[i];
			numberInList = i;
			foundPlusToolConfigFile = true;
		}
		else
		{
//			CX_LOG_DEBUG() << "Tool config file (not matching input): " << configurations[i];
		}
	}
	return foundPlusToolConfigFile;
}

std::vector<PositionReceiverPtr> setupTestTools(cx::TrackerConfiguration::Configuration trackerConfig, OpenIGTLinkTrackingSystemServiceMocPtr trackingSystemService)
{
	std::vector<PositionReceiverPtr> positionReceivers;
	for(int i = 0; i < trackerConfig.mToolList.size(); ++i)
	{
		//devicename = trackerConfig.mToolList[i].mAbsoluteToolFilePath;
		QString devicename = trackerConfig.mToolList[i].mOpenIGTLinkTransformId;
		//CX_LOG_DEBUG() << trackerConfig.mToolList[i].mOpenIGTLinkTransformId << " " << trackerConfig.mToolList[i].mAbsoluteToolFilePath;
		//CHECK_FALSE(trackerConfig.mToolList[i].mApplyRefToTool);
		cx::OpenIGTLinkToolPtr tool = trackingSystemService->getTool(devicename);
		CHECK(tool);
		PositionReceiverPtr positionReceiver = PositionReceiverPtr(new PositionReceiver(devicename, tool));
		positionReceivers.push_back(positionReceiver);
	}
	return positionReceivers;
}

void getDeviceNames(std::vector<PositionReceiverPtr> positionReceivers, QString& refToolDeviceName, QString& toolDeviceName, PositionReceiverPtr& toolPositionReceiver)
{
	for(int i = 0; i < positionReceivers.size(); ++i)
	{
		if(positionReceivers[i]->mTool->isReference())
			refToolDeviceName = positionReceivers[i]->mDevicename;
		else
		{
			toolDeviceName = positionReceivers[i]->mDevicename;
			toolPositionReceiver = positionReceivers[i];
		}
	}
}

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

TEST_CASE("OpenIGTLinkTrackingSystemService: receiveTransform print openigtlinktransformid warning", "[plugins][org.custusx.core.openigtlink3][unit]")
{
	OpenIGTLinkTrackingSystemServiceMocPtr trackingSystemService = OpenIGTLinkTrackingSystemServiceMocPtr(new OpenIGTLinkTrackingSystemServiceMoc());
	cx::Transform3D transform = cx::Transform3D::Identity();
	double timestampMS(QDateTime::currentDateTime().toMSecsSinceEpoch());
	QString deviceName = "testDevice";
	trackingSystemService->receiveTransform(deviceName, transform, timestampMS);
	CHECK(trackingSystemService->getTransformIdWarningPrinted().contains(deviceName));
}

TEST_CASE("OpenIGTLinkTrackingSystemService: print openigtlinktransformid warning", "[plugins][org.custusx.core.openigtlink3][unit]")
{
	OpenIGTLinkTrackingSystemServiceMocPtr trackingSystemService = OpenIGTLinkTrackingSystemServiceMocPtr(new OpenIGTLinkTrackingSystemServiceMoc());

	QString deviceName = "testDevice1";
	QString deviceName2 = "testDevice2";
	CHECK(trackingSystemService->testPrintTransformIdWarning(deviceName));
	CHECK(trackingSystemService->getTransformIdWarningPrinted().contains(deviceName));
	CHECK(trackingSystemService->testPrintTransformIdWarning(deviceName2));
	CHECK_FALSE(trackingSystemService->testPrintTransformIdWarning(deviceName));

	trackingSystemService->deconfigure();
	CHECK(trackingSystemService->testPrintTransformIdWarning(deviceName));
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
	bool foundPlusToolConfigFile = findConfigFileNumberInList(configurations, QString("PLUS"), posOfPlusConfigToolFile);

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

TEST_CASE("OpenIGTLinkTrackingSystemService: Test tool config files, and apply ref transform", "[plugins][org.custusx.core.openigtlink3][unit]")
{
	OpenIGTLinkTrackingSystemServiceMocPtr trackingSystemService = OpenIGTLinkTrackingSystemServiceMocPtr(new OpenIGTLinkTrackingSystemServiceMoc());

	cx::TrackerConfigurationPtr config = trackingSystemService->getConfiguration();
	REQUIRE(config);
	QStringList configurations = config->getAllConfigurations();

	//----- Test Plus config file where ref position should not be applied to tools -----

	int posToolConfigFile = 0;
	bool foundToolConfigFile = findConfigFileNumberInList(configurations, QString("PLUS"), posToolConfigFile);
	REQUIRE(foundToolConfigFile);
	QString toolConfigFile = configurations[posToolConfigFile];

	CX_LOG_DEBUG() << "Testing with tool config file: " << toolConfigFile;
	trackingSystemService->setConfigurationFile(toolConfigFile);

	config = trackingSystemService->getConfiguration();

	cx::TrackerConfiguration::Configuration trackerConfig = config->getConfiguration(toolConfigFile);
	//CHECK_FALSE(trackerConfig.mTrackingSystemImplementation == cx::TRACKING_SYSTEM_IMPLEMENTATION_IGTLINK);
	CHECK(trackerConfig.mTrackingSystemImplementation == cx::TRACKING_SYSTEM_IMPLEMENTATION_IGTLINK);
	CHECK_FALSE(trackerConfig.mApplyRefToTools);

	trackingSystemService->configure();
	CHECK(trackingSystemService->isConfigured());

	std::vector<PositionReceiverPtr> positionReceivers = setupTestTools(trackerConfig, trackingSystemService);

	QString refToolDeviceName;
	QString toolDeviceName;
	PositionReceiverPtr toolPositionReceiver;
	getDeviceNames(positionReceivers, refToolDeviceName, toolDeviceName, toolPositionReceiver);

	cx::Transform3D transform = cx::Transform3D::Identity();
	cx::Transform3D transform_111 =cx::createTransformTranslate(cx::Vector3D(1,1,1));
	double timestampMS(QDateTime::currentDateTime().toMSecsSinceEpoch());

	//Simulate incoming transforms
	//CX_LOG_DEBUG() << "Sent transform:\n" << transform;
	trackingSystemService->receiveTransform(refToolDeviceName, transform, timestampMS);
	trackingSystemService->receiveTransform(toolDeviceName, transform, timestampMS);
	cx::Transform3D toolTransform = toolPositionReceiver->mPrMs;

	//CX_LOG_DEBUG() << "Sent ref transform:\n" << transform_111;
	trackingSystemService->receiveTransform(refToolDeviceName, transform_111, timestampMS);
	trackingSystemService->receiveTransform(toolDeviceName, transform, timestampMS);
	cx::Transform3D toolTransform2 = toolPositionReceiver->mPrMs;

	//When trackerConfig.mToolList[i].mApplyRefToTool is false, changing ref transform should not change tool transform
	CHECK(cx::similar(toolTransform, toolTransform2));

	trackingSystemService->deconfigure();
	CHECK_FALSE(trackingSystemService->isConfigured());

	//----- Test BK config file where ref position should be applied to tools -----

	foundToolConfigFile = findConfigFileNumberInList(configurations, QString("BK_LabTest"), posToolConfigFile);
	REQUIRE(foundToolConfigFile);
	toolConfigFile = configurations[posToolConfigFile];
	CX_LOG_DEBUG() << "Testing with tool config file: " << toolConfigFile;
	trackingSystemService->setConfigurationFile(toolConfigFile);

	trackerConfig = config->getConfiguration(toolConfigFile);
	CHECK(trackerConfig.mTrackingSystemImplementation == cx::TRACKING_SYSTEM_IMPLEMENTATION_IGTLINK);
	CHECK(trackerConfig.mApplyRefToTools);

	trackingSystemService->configure();
	CHECK(trackingSystemService->isConfigured());

	positionReceivers = setupTestTools(trackerConfig, trackingSystemService);
	getDeviceNames(positionReceivers, refToolDeviceName, toolDeviceName, toolPositionReceiver);

	//Simulate incoming transforms
	//CX_LOG_DEBUG() << "Sent transform:\n" << transform;
	trackingSystemService->receiveTransform(refToolDeviceName, transform, timestampMS);
	trackingSystemService->receiveTransform(toolDeviceName, transform, timestampMS);
	toolTransform = toolPositionReceiver->mPrMs;

	//CX_LOG_DEBUG() << "Sent ref transform:\n" << transform_111;
	trackingSystemService->receiveTransform(refToolDeviceName, transform_111, timestampMS);
	trackingSystemService->receiveTransform(toolDeviceName, transform, timestampMS);
	toolTransform2 = toolPositionReceiver->mPrMs;

	//When trackerConfig.mToolList[i].mApplyRefToTool is Ttrue, changing ref transform should change tool transform
	CHECK_FALSE(cx::similar(toolTransform, toolTransform2));

	trackingSystemService->deconfigure();
	CHECK_FALSE(trackingSystemService->isConfigured());
}

TEST_CASE("OpenIGTLinkTrackingSystemService: Test tool files with OpenIGTLink id", "[plugins][org.custusx.core.openigtlink3][unit]")
{
	OpenIGTLinkTrackingSystemServiceMocPtr trackingSystemService = OpenIGTLinkTrackingSystemServiceMocPtr(new OpenIGTLinkTrackingSystemServiceMoc());

	cx::TrackerConfigurationPtr config = trackingSystemService->getConfiguration();
	REQUIRE(config);
	QStringList configurations = config->getAllConfigurations();

	int posToolConfigFile = 0;
	bool foundToolConfigFile = findConfigFileNumberInList(configurations, QString("BK_LabTest"), posToolConfigFile);
	REQUIRE(foundToolConfigFile);
	QString toolConfigFile = configurations[posToolConfigFile];

	CX_LOG_DEBUG() << "Testing with tool config file: " << toolConfigFile;
	trackingSystemService->setConfigurationFile(toolConfigFile);


	config = trackingSystemService->getConfiguration();
	cx::TrackerConfiguration::Configuration trackerConfig = config->getConfiguration(toolConfigFile);
	CHECK(trackerConfig.mTrackingSystemImplementation == cx::TRACKING_SYSTEM_IMPLEMENTATION_IGTLINK);

	//Check that tool files got OpenIGTLink id tags
	for (unsigned i = 0; i < trackerConfig.mToolList.size(); ++i)
	{
		//CX_LOG_DEBUG() << trackerConfig.mToolList[i].mAbsoluteToolFilePath;
		cx::ToolFileParser toolParser(trackerConfig.mToolList[i].mAbsoluteToolFilePath);
		cx::ToolFileParser::ToolInternalStructurePtr internalTool = toolParser.getTool();
		CHECK(internalTool->mOpenigtlinkTransformId.startsWith("BK2300-"));
		if(toolParser.getTool()->mIsProbe)
			CHECK(internalTool->mOpenigtlinkImageId == internalTool->mOpenigtlinkTransformId);
	}

	trackingSystemService->configure();
	CHECK(trackingSystemService->isConfigured());

	//Check that tool config file got OpenIGTLink id tags
	std::vector<PositionReceiverPtr> positionReceivers = setupTestTools(trackerConfig, trackingSystemService);
	QString refToolDeviceName;
	QString toolDeviceName;
	PositionReceiverPtr toolPositionReceiver;
	getDeviceNames(positionReceivers, refToolDeviceName, toolDeviceName, toolPositionReceiver);
	CHECK(refToolDeviceName.startsWith("BK2300-1"));
	CHECK(toolDeviceName.startsWith("BK2300-"));

	trackingSystemService->deconfigure();
	CHECK_FALSE(trackingSystemService->isConfigured());
}

#endif

}//cxtest
