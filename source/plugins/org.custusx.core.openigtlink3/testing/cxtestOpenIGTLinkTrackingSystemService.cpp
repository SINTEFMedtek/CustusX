/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include <memory>
#include "cxOpenIGTLinkTrackingSystemService.h"
#include "cxLogger.h"

#include "cxTrackerConfiguration.h"
#include "cxOpenIGTLinkTool.h"
#include "cxOpenIGTLinkStreamerService.h"
#include "cxOpenIGTLinkStreamer.h"
#include <QDomDocument>
#include <QFileInfo>

namespace {
typedef std::shared_ptr<class TestNetworkHandler> cxtestNetworkHandlerPtr;

class TestNetworkHandler : public cx::NetworkHandler
{
public:
	TestNetworkHandler() : cx::NetworkHandler(nullptr) {}

	double synchronizedTimestamp(double sentTimestampSec, double received_ms, QString deviceName = "device")
	{
		return NetworkHandler::synchronizedTimestamp(deviceName, sentTimestampSec, received_ms);
	}
	double getTimestampOffset(QString deviceName = "device")
	{
		return mClockSynchronizers.count(deviceName) ? mClockSynchronizers[deviceName].getOffset_ms() : 0.0;
	}
	void setSentProbeDefinitions(QString deviceName, int sent)
	{
		mDevices[deviceName].deviceName = deviceName;
		mDevices[deviceName].sentNumProbeDefinitions = sent;
	}
	int getSentProbeDefinitions(QString deviceName)
	{
		return mDevices[deviceName].sentNumProbeDefinitions;
	}
};

/** Pretends to connect to a server, which answers if mServerAvailable */
class FakeServerNetworkHandler : public TestNetworkHandler
{
public:
	bool mServerAvailable = true;
	bool mConnected = false;
	int mConnectAttempts = 0;

	igtlioSessionPointer requestConnectToServer(std::string serverHost, int serverPort, IGTLIO_SYNCHRONIZATION_TYPE sync, double timeout_s) override
	{
		Q_UNUSED(serverHost);
		Q_UNUSED(serverPort);
		Q_UNUSED(sync);
		Q_UNUSED(timeout_s);
		++mConnectAttempts;
		mConnected = mServerAvailable;
		if (mConnected)
		{
			emit connected();
		}
		return igtlioSessionPointer();
	}
	void disconnectFromServer() override
	{
		if (mConnected)
		{
			mConnected = false;
			emit disconnected();
		}
	}
	bool isConnected() const override
	{
		return mConnected;
	}
};
typedef std::shared_ptr<FakeServerNetworkHandler> FakeServerNetworkHandlerPtr;

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

typedef std::shared_ptr<OpenIGTLinkTrackingSystemServiceMoc> OpenIGTLinkTrackingSystemServiceMocPtr;

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

typedef std::shared_ptr<PositionReceiver> PositionReceiverPtr;

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

void setCxOpenIGTLinkConfiguration(OpenIGTLinkTrackingSystemServiceMocPtr trackingSystemService)
{
	// An OpenIGTLink tool configuration in CustusX's own config folder: one tool with transform id AnserNeedle1
	QStringList configurations = trackingSystemService->getConfiguration()->getAllConfigurations();
	int posToolConfigFile = 0;
	REQUIRE(findConfigFileNumberInList(configurations, QString("Bronchoscopy_Anser_one_needle"), posToolConfigFile));
	trackingSystemService->setConfigurationFile(configurations[posToolConfigFile]);
}

const QString kCxOpenIGTLinkTool = "AnserNeedle1";

} // namespace

namespace cxtest
{

TEST_CASE("OpenIGTLinkTrackingSystemService: Stays in tsNONE without an OpenIGTLink tool configuration", "[plugins][org.custusx.core.openigtlink3][unit]")
{
	OpenIGTLinkTrackingSystemServiceMocPtr trackingSystemService = OpenIGTLinkTrackingSystemServiceMocPtr(new OpenIGTLinkTrackingSystemServiceMoc());
	trackingSystemService->setConfigurationFile("");

	trackingSystemService->internalSetState(cx::Tool::tsTRACKING);
	CHECK(trackingSystemService->getState() == cx::Tool::tsNONE);
	CHECK_FALSE(trackingSystemService->isTracking());
}

TEST_CASE("OpenIGTLinkTrackingSystemService: start tracking connects to the server, and stop disconnects", "[plugins][org.custusx.core.openigtlink3][unit]")
{
	FakeServerNetworkHandlerPtr server(new FakeServerNetworkHandler());
	OpenIGTLinkTrackingSystemServiceMocPtr trackingSystemService(new OpenIGTLinkTrackingSystemServiceMoc(server));
	setCxOpenIGTLinkConfiguration(trackingSystemService);

	trackingSystemService->internalSetState(cx::Tool::tsTRACKING);
	CHECK(server->mConnectAttempts == 1);
	CHECK(server->isConnected());
	CHECK(trackingSystemService->isTracking());

	// Stop tracking from the toolbar asks for tsINITIALIZED
	trackingSystemService->internalSetState(cx::Tool::tsINITIALIZED);
	CHECK_FALSE(server->isConnected());
	CHECK(trackingSystemService->getState() == cx::Tool::tsCONFIGURED);
}

TEST_CASE("OpenIGTLinkTrackingSystemService: without an answer from the server, tracking does not start until a new start", "[plugins][org.custusx.core.openigtlink3][unit]")
{
	FakeServerNetworkHandlerPtr server(new FakeServerNetworkHandler());
	server->mServerAvailable = false;
	OpenIGTLinkTrackingSystemServiceMocPtr trackingSystemService(new OpenIGTLinkTrackingSystemServiceMoc(server));
	setCxOpenIGTLinkConfiguration(trackingSystemService);

	trackingSystemService->internalSetState(cx::Tool::tsTRACKING);
	CHECK(trackingSystemService->getState() == cx::Tool::tsCONFIGURED);

	server->mServerAvailable = true;
	trackingSystemService->internalSetState(cx::Tool::tsTRACKING);
	CHECK(server->mConnectAttempts == 2);
	CHECK(trackingSystemService->isTracking());
}

TEST_CASE("OpenIGTLinkTrackingSystemService: a video connection starts tracking, and stopping tracking keeps it", "[plugins][org.custusx.core.openigtlink3][unit]")
{
	FakeServerNetworkHandlerPtr server(new FakeServerNetworkHandler());
	OpenIGTLinkTrackingSystemServiceMocPtr trackingSystemService(new OpenIGTLinkTrackingSystemServiceMoc(server));
	setCxOpenIGTLinkConfiguration(trackingSystemService);
	double timestampMS(QDateTime::currentDateTime().toMSecsSinceEpoch());

	server->acquireConnection("video", "127.0.0.1", 18944);
	CHECK(trackingSystemService->isTracking());
	trackingSystemService->receiveTransform(kCxOpenIGTLinkTool, cx::Transform3D::Identity(), timestampMS);
	CHECK(trackingSystemService->getTool(kCxOpenIGTLinkTool)->getTimestamp() == Approx(timestampMS));

	trackingSystemService->internalSetState(cx::Tool::tsINITIALIZED);
	CHECK(server->isConnected());
	CHECK(trackingSystemService->getState() == cx::Tool::tsINITIALIZED);
	trackingSystemService->receiveTransform(kCxOpenIGTLinkTool, cx::Transform3D::Identity(), timestampMS + 100);
	CHECK(trackingSystemService->getTool(kCxOpenIGTLinkTool)->getTimestamp() == Approx(timestampMS));

	server->releaseConnection("video");
	CHECK(trackingSystemService->getState() == cx::Tool::tsCONFIGURED);
}

TEST_CASE("OpenIGTLinkTrackingSystemService: tracks the tools of an NDI configuration whose device is in use", "[plugins][org.custusx.core.openigtlink3][unit]")
{
	FakeServerNetworkHandlerPtr server(new FakeServerNetworkHandler());
	OpenIGTLinkTrackingSystemServiceMocPtr trackingSystemService(new OpenIGTLinkTrackingSystemServiceMoc(server));
	QStringList configurations = trackingSystemService->getConfiguration()->getAllConfigurations();
	int posToolConfigFile = 0;
	REQUIRE(findConfigFileNumberInList(configurations, QString("LabAuroraTools"), posToolConfigFile));
	QString ndiConfiguration = configurations[posToolConfigFile];
	trackingSystemService->setConfigurationFile(ndiConfiguration);
	double timestampMS(QDateTime::currentDateTime().toMSecsSinceEpoch());

	// Not an OpenIGTLink configuration: only tracked as a fallback
	trackingSystemService->internalSetState(cx::Tool::tsTRACKING);
	CHECK(trackingSystemService->getState() == cx::Tool::tsNONE);

	trackingSystemService->startFallbackTracking(ndiConfiguration);
	CHECK(trackingSystemService->isTracking());
	CHECK(server->mConnectAttempts == 1);
	// The Aurora sensor on port number 2 is found by the transform name port_2, the reference by port_0
	REQUIRE(trackingSystemService->getTool("port_2"));
	CHECK(trackingSystemService->getTool("port_2")->applyReference()); // the server replaces the device
	trackingSystemService->receiveTransform("port_0", cx::createTransformTranslate(cx::Vector3D(10, 0, 0)), timestampMS);
	trackingSystemService->receiveTransform("port_2", cx::createTransformTranslate(cx::Vector3D(15, 0, 0)), timestampMS);
	CHECK(trackingSystemService->getTool("port_2")->getTimestamp() == Approx(timestampMS));
	// Relative to the reference, which is the origin, as from the NDI and IGSTK tracking systems
	CHECK(cx::similar(trackingSystemService->getTool("port_0")->get_prMt(), cx::Transform3D::Identity()));
	CHECK(cx::similar(trackingSystemService->getTool("port_2")->get_prMt().translation(), cx::Vector3D(5, 0, 0)));

	// A new start ends the fallback, so that the NDI system tries its device again
	trackingSystemService->internalSetState(cx::Tool::tsTRACKING);
	CHECK(trackingSystemService->getState() == cx::Tool::tsNONE);
	CHECK_FALSE(server->isConnected());
}

TEST_CASE("NetworkHandler: the connection is shared, and closed when the last user releases it", "[plugins][org.custusx.core.openigtlink3][unit]")
{
	FakeServerNetworkHandlerPtr server(new FakeServerNetworkHandler());

	server->acquireConnection("video", "127.0.0.1", 18944);
	server->acquireConnection("tracking", "127.0.0.1", 18944);
	CHECK(server->mConnectAttempts == 1);

	server->releaseConnection("video");
	CHECK(server->isConnected());
	server->releaseConnection("tracking");
	CHECK_FALSE(server->isConnected());
}

TEST_CASE("NetworkHandler: Timestamps follow the sender, with the offset of the fastest message", "[plugins][org.custusx.core.openigtlink3][unit]")
{
	cxtestNetworkHandlerPtr networkHandler = cxtestNetworkHandlerPtr(new TestNetworkHandler());
	double start_ms = 5000000;
	double latency_ms = 2;
	for (int i = 0; i < 40; ++i)
	{
		double sentSec = 1000 + i * 0.005;
		double received_ms = start_ms + i * 5 + latency_ms + (i % 3);
		double synchedTimeMs = networkHandler->synchronizedTimestamp(sentSec, received_ms);
		INFO("iteration: " + qstring_cast(i));
		CHECK(cx::similar(synchedTimeMs, received_ms - (i % 3), 0.001));
	}
	CHECK(cx::similar(networkHandler->getTimestampOffset(), start_ms + latency_ms - 1000000, 0.001));
}

TEST_CASE("NetworkHandler: Uses the receive time when the sender time jumps back", "[plugins][org.custusx.core.openigtlink3][unit]")
{
	cxtestNetworkHandlerPtr networkHandler = cxtestNetworkHandlerPtr(new TestNetworkHandler());
	double received_ms = 5000000;
	networkHandler->synchronizedTimestamp(1000, received_ms);
	received_ms += 5;
	CHECK(cx::similar(networkHandler->synchronizedTimestamp(1000 - 3, received_ms), received_ms, 0.001));
}

TEST_CASE("NetworkHandler: Each device has its own timestamp offset", "[plugins][org.custusx.core.openigtlink3][unit]")
{
	cxtestNetworkHandlerPtr networkHandler = cxtestNetworkHandlerPtr(new TestNetworkHandler());
	double received_ms = 5000000;
	networkHandler->synchronizedTimestamp(1000, received_ms, "tracking");
	networkHandler->synchronizedTimestamp(3000, received_ms, "image");
	CHECK(cx::similar(networkHandler->getTimestampOffset("tracking"), received_ms - 1000000, 0.001));
	CHECK(cx::similar(networkHandler->getTimestampOffset("image"), received_ms - 3000000, 0.001));
}

TEST_CASE("NetworkHandler: Test timestamp synchronization reset", "[plugins][org.custusx.core.openigtlink3][unit]")
{
	cxtestNetworkHandlerPtr networkHandler = cxtestNetworkHandlerPtr(new TestNetworkHandler());
	double received_ms = 5000000;
	networkHandler->synchronizedTimestamp(1000, received_ms);
	CHECK(cx::similar(networkHandler->getTimestampOffset(), received_ms - 1000000, 0.001));

	networkHandler->clearTimestampSynchronization();
	CHECK(cx::similar(networkHandler->getTimestampOffset(), 0, 0.001));

	networkHandler->synchronizedTimestamp(3000, received_ms);
	CHECK(cx::similar(networkHandler->getTimestampOffset(), received_ms - 3000000, 0.001));
}

TEST_CASE("OpenIGTLinkTrackingSystemService: Test timestamp synchronization", "[plugins][org.custusx.core.openigtlink3][unit]")
{
	cxtestNetworkHandlerPtr networkHandler = cxtestNetworkHandlerPtr(new TestNetworkHandler());
	OpenIGTLinkTrackingSystemServiceMocPtr trackingSystemService = OpenIGTLinkTrackingSystemServiceMocPtr(new OpenIGTLinkTrackingSystemServiceMoc(networkHandler));

	double tolerance = 10;
	qint64 latestSystemTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
	double timestampSec = 1000;

	double synchedTimeMs = trackingSystemService->mTestNetworkHandler->synchronizedTimestamp(timestampSec, latestSystemTime);
	REQUIRE(cx::similar(synchedTimeMs, latestSystemTime, tolerance));

	REQUIRE_FALSE(cx::similar(0, networkHandler->getTimestampOffset(), tolerance));
	trackingSystemService->resetTimeSynchronization();
	REQUIRE(cx::similar(0, networkHandler->getTimestampOffset(), tolerance));
}

TEST_CASE("OpenIGTLinkTrackingSystemService: receiveTransform print openigtlinktransformid warning", "[plugins][org.custusx.core.openigtlink3][unit]")
{
	OpenIGTLinkTrackingSystemServiceMocPtr trackingSystemService = OpenIGTLinkTrackingSystemServiceMocPtr(new OpenIGTLinkTrackingSystemServiceMoc());
	setCxOpenIGTLinkConfiguration(trackingSystemService);
	trackingSystemService->internalSetState(cx::Tool::tsTRACKING);
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

TEST_CASE("OpenIGTLinkTrackingSystemService: Test state transitions", "[plugins][org.custusx.core.openigtlink3][unit]")
{
	OpenIGTLinkTrackingSystemServiceMocPtr trackingSystemService = OpenIGTLinkTrackingSystemServiceMocPtr(new OpenIGTLinkTrackingSystemServiceMoc());

	CHECK(trackingSystemService->getState() == cx::Tool::tsNONE);
	CHECK_FALSE(trackingSystemService->isConfigured());
	CHECK_FALSE(trackingSystemService->isInitialized());
	CHECK_FALSE(trackingSystemService->isTracking());

	QStringList configurations = trackingSystemService->getConfiguration()->getAllConfigurations();
	int posToolConfigFile = 0;
	REQUIRE(findConfigFileNumberInList(configurations, QString("PLUS"), posToolConfigFile));
	trackingSystemService->setConfigurationFile(configurations[posToolConfigFile]);

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

	trackingSystemService->internalSetState(cx::Tool::tsTRACKING);
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

	trackingSystemService->internalSetState(cx::Tool::tsTRACKING);
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

TEST_CASE("OpenIGTLinkTrackingSystemService: tool positions wait for the first reference position", "[plugins][org.custusx.core.openigtlink3][unit]")
{
	OpenIGTLinkTrackingSystemServiceMocPtr trackingSystemService = OpenIGTLinkTrackingSystemServiceMocPtr(new OpenIGTLinkTrackingSystemServiceMoc());
	cx::TrackerConfigurationPtr config = trackingSystemService->getConfiguration();
	QStringList configurations = config->getAllConfigurations();
	int posToolConfigFile = 0;
	REQUIRE(findConfigFileNumberInList(configurations, QString("BK_LabTest"), posToolConfigFile));
	QString toolConfigFile = configurations[posToolConfigFile];
	trackingSystemService->setConfigurationFile(toolConfigFile);
	cx::TrackerConfiguration::Configuration trackerConfig = config->getConfiguration(toolConfigFile);
	REQUIRE(trackerConfig.mApplyRefToTools);
	trackingSystemService->internalSetState(cx::Tool::tsTRACKING);

	std::vector<PositionReceiverPtr> positionReceivers = setupTestTools(trackerConfig, trackingSystemService);
	QString refToolDeviceName;
	QString toolDeviceName;
	PositionReceiverPtr toolPositionReceiver;
	getDeviceNames(positionReceivers, refToolDeviceName, toolDeviceName, toolPositionReceiver);
	double timestampMS(QDateTime::currentDateTime().toMSecsSinceEpoch());

	trackingSystemService->receiveTransform(toolDeviceName, cx::Transform3D::Identity(), timestampMS);
	CHECK(trackingSystemService->getTool(toolDeviceName)->getTimestamp() == 0);

	trackingSystemService->receiveTransform(refToolDeviceName, cx::Transform3D::Identity(), timestampMS);
	trackingSystemService->receiveTransform(toolDeviceName, cx::Transform3D::Identity(), timestampMS);
	CHECK(trackingSystemService->getTool(toolDeviceName)->getTimestamp() == Approx(timestampMS));

	trackingSystemService->deconfigure();
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

TEST_CASE("OpenIGTLinkStreamerService: the probe definition is sent again when the streaming starts", "[plugins][org.custusx.core.openigtlink3][unit]")
{
	FakeServerNetworkHandlerPtr connection(new FakeServerNetworkHandler());
	cx::OpenIGTLinkStreamerService service(connection);
	QDomDocument document;
	cx::StreamerPtr streamer = service.createStreamer(document.createElement("root"));

	// Images were already received over the connection used by tracking, using up the probe definitions to send
	connection->setSentProbeDefinitions("BK", 2);
	streamer->startStreaming(cx::SenderPtr());
	CHECK(connection->getSentProbeDefinitions("BK") == 0);
}

}//cxtest
