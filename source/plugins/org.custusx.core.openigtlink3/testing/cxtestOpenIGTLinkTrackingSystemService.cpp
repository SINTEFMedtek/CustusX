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

class OpenIGTLinkTrackingSystemServiceMoc : public cx::OpenIGTLinkTrackingSystemService
{
public:
	OpenIGTLinkTrackingSystemServiceMoc() : cx::OpenIGTLinkTrackingSystemService(cx::NetworkHandlerPtr()) {}

	virtual bool isConfigured() const;
	virtual bool isInitialized() const;
	virtual bool isTracking() const;

	void internalSetState(cx::Tool::State val) {cx::OpenIGTLinkTrackingSystemService::internalSetState(val);}

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

typedef boost::shared_ptr<OpenIGTLinkTrackingSystemServiceMoc> OpenIGTLinkTrackingSystemServiceMocPtr;


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

#ifdef CX_CUSTUS_SINTEF
TEST_CASE("OpenIGTLinkTrackingSystemService: Test configure state using tool config file for PLUS", "[plugins][org.custusx.core.openigtlink3][integration]")
{
	OpenIGTLinkTrackingSystemServiceMocPtr trackingSystemService = OpenIGTLinkTrackingSystemServiceMocPtr(new OpenIGTLinkTrackingSystemServiceMoc());

	cx::TrackerConfigurationPtr config = trackingSystemService->getConfiguration();
	REQUIRE(config);
	QStringList configurations = config->getAllConfigurations();

	REQUIRE(configurations.size() > 1); //Test require that at least one tool configuration is present. Normally there should be many

	//Use one of the tool config files with PLUS tools
	int posOfPlusConfigToolFile = 0;
	bool foundPlusToolConfigFile = false;
	for(int i = 0; i < configurations.size(); ++i)
	{
		QFileInfo fileInfo(configurations[i]);
		if (fileInfo.fileName().startsWith("PLUS"))
		{
//			CX_LOG_DEBUG() << "Found PLUS tool config file: " << configurations[i];
			posOfPlusConfigToolFile = i;
			foundPlusToolConfigFile = true;
		}
//		else
//			CX_LOG_DEBUG() << "Tool config file (not PLUS): " << configurations[i];
	}

	REQUIRE(foundPlusToolConfigFile);// Test require that at leat one tool configuration using PLUS is present.
	QString toolConfigFile = configurations[posOfPlusConfigToolFile];
	CX_LOG_DEBUG() << "Using PLUS tool config file: " << toolConfigFile;
	trackingSystemService->setConfigurationFile(toolConfigFile);

	//Test real configure/deconfigure functions
	trackingSystemService->configure();
	CHECK(trackingSystemService->isConfigured());//Wait for stateChanged?

	trackingSystemService->deconfigure();
	CHECK_FALSE(trackingSystemService->isConfigured());//Wait for stateChanged?
}
#endif

}//cxtest
