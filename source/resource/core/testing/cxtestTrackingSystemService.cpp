/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxTrackingSystemService.h"

class TrackingSystemServiceMoc : public cx::TrackingSystemService
{
public:
	TrackingSystemServiceMoc():
		initializeCalled(false),
		configureCalled(false)
	{}

	virtual QString getUid() const {return QString();}
	virtual void setState(const cx::Tool::State val) { this->internalSetState(val); }
	virtual std::vector<cx::ToolPtr> getTools() { return std::vector<cx::ToolPtr>();}
	virtual cx::TrackerConfigurationPtr getConfiguration() {return cx::TrackerConfigurationPtr();}
	virtual cx::ToolPtr getReference() {return cx::ToolPtr();}

	QString getLoggingFolder() { return mLoggingFolder;}
	QString getConfigurationFilePath() { return mConfigurationFilePath;}

	virtual bool isConfigured() const;
	virtual bool isInitialized() const;
	virtual bool isTracking() const;

	virtual void startTracking()
	{
		cx::TrackingSystemService::startTracking();
	}
	virtual void initialize()
	{
		cx::TrackingSystemService::initialize();
		initializeCalled = true;
	}

	virtual void configure()
	{
		cx::TrackingSystemService::configure();
		configureCalled = true;
	}

	bool initializeCalled;
	bool configureCalled;
};

bool TrackingSystemServiceMoc::isConfigured() const
{
	return cx::TrackingSystemService::isConfigured();
}

bool TrackingSystemServiceMoc::isInitialized() const
{
	return cx::TrackingSystemService::isInitialized();
}

bool TrackingSystemServiceMoc::isTracking() const
{
	return cx::TrackingSystemService::isTracking();
}

typedef boost::shared_ptr<TrackingSystemServiceMoc> TrackingSystemServiceMocPtr;

namespace cxtest
{
TEST_CASE("TrackingSystemService: Init", "[unit]")
{
	TrackingSystemServiceMocPtr trackingSystemService = TrackingSystemServiceMocPtr(new TrackingSystemServiceMoc());
	CHECK(trackingSystemService->getState() == cx::Tool::tsNONE);
	CHECK_FALSE(trackingSystemService->isConfigured());
	CHECK_FALSE(trackingSystemService->isInitialized());
	CHECK_FALSE(trackingSystemService->isTracking());

	QString testString("test");
	trackingSystemService->setLoggingFolder(testString);
	CHECK(testString == trackingSystemService->getLoggingFolder());

	trackingSystemService->setConfigurationFile(testString);
	CHECK(testString == trackingSystemService->getConfigurationFilePath());
}

TEST_CASE("TrackingSystemService: Convenience state transitions", "[unit]")
{
	TrackingSystemServiceMocPtr trackingSystemService = TrackingSystemServiceMocPtr(new TrackingSystemServiceMoc());

	CHECK_FALSE(trackingSystemService->configureCalled);
	CHECK_FALSE(trackingSystemService->initializeCalled);

	trackingSystemService->initialize();
	CHECK(trackingSystemService->configureCalled);

	trackingSystemService->configureCalled = false;
	trackingSystemService->startTracking();
	CHECK(trackingSystemService->configureCalled);
	CHECK(trackingSystemService->initializeCalled);
}

}//cxtest


