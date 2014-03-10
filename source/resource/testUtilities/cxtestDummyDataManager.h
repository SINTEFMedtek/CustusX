// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#ifndef CXTESTDUMMYDATAMANAGER_H
#define CXTESTDUMMYDATAMANAGER_H

#include "cxForwardDeclarations.h"
#include <QStringList>

namespace cxtest
{

struct TestServicesType
{
	cx::DataFactoryPtr mDataFactory;
	cx::DataServicePtr mDataService;
	cx::SpaceProviderPtr mSpaceProvider;
	cx::TrackingServicePtr mTrackingService;
};

/** Create a dummy DataService that is not using a toolmanager internally
  *
  */
cx::DataServicePtr createDummyDataService();
TestServicesType createDummyCoreServices();
void destroyDummyCoreServices(TestServicesType& services);

typedef boost::shared_ptr<class TestServices> TestServicesPtr;

/** A minimal set of services for test usage.
  */
class TestServices : public TestServicesType
{
public:
	static TestServicesPtr create();
	~TestServices();

	cx::DataFactoryPtr dataFactory() { return mDataFactory; }
	cx::DataServicePtr dataService() { return mDataService; }
	cx::SpaceProviderPtr spaceProvider() { return mSpaceProvider; }
	cx::TrackingServicePtr trackingService() { return mTrackingService; }

private:
	TestServices();
};

} // namespace cx

#endif // CXTESTDUMMYDATAMANAGER_H
