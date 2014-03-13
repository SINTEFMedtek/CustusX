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

#include "cxtestDummyDataManager.h"

#include "cxDataManagerImpl.h"
//#include "cxtestSpaceProviderMock.h"
#include "cxDataFactory.h"
#include "cxDummyToolManager.h"
#include "cxSpaceProviderImpl.h"
#include "cxMessageManager.h"
#include "cxSharedPointerChecker.h"

namespace cxtest
{


TestServicesType createDummyCoreServices()
{
	cx::DataManagerImplPtr dataService = cx::DataManagerImpl::create();
	cx::TrackingServicePtr trackingService = cx::DummyToolManager::create();

	cx::SpaceProviderPtr spaceProvider;
	spaceProvider.reset(new cx::SpaceProviderImpl(trackingService, dataService));
//	cx::SpaceProviderPtr spaceProvider = cxtest::SpaceProviderMock::create();
	dataService->setSpaceProvider(spaceProvider);

	cx::DataFactoryPtr dataFactory;
	dataFactory.reset(new cx::DataFactory(dataService, spaceProvider));
	dataService->setDataFactory(dataFactory);

	TestServicesType retval;
	retval.mDataFactory = dataFactory;
	retval.mDataService = dataService;
	retval.mSpaceProvider = spaceProvider;
	retval.mTrackingService = trackingService;
	return retval;
}

void destroyDummyCoreServices(TestServicesType& services)
{
	// [HACK] break loop by removing connection to DataFactory and SpaceProvider
	cx::DataManagerImplPtr dataManagerImpl;
	dataManagerImpl = boost::dynamic_pointer_cast<cx::DataManagerImpl>(services.mDataService);
	if (dataManagerImpl)
	{
		dataManagerImpl->setSpaceProvider(cx::SpaceProviderPtr());
		dataManagerImpl->setDataFactory(cx::DataFactoryPtr());
		dataManagerImpl->clear();
		dataManagerImpl.reset();
	}

	requireUnique(services.mDataFactory, "DataFactory");
	services.mDataFactory.reset();

	requireUnique(services.mSpaceProvider, "SpaceProvider");
	services.mSpaceProvider.reset();

	requireUnique(services.mDataService, "DataService");
	services.mDataService.reset();

	requireUnique(services.mTrackingService, "TrackingService");
	services.mTrackingService.reset();

	cx::MessageManager::shutdown();
}

cx::DataServicePtr createDummyDataService()
{
	return createDummyCoreServices().mDataService;
}

TestServicesPtr TestServices::create()
{
	return TestServicesPtr(new TestServices());
}

TestServices::TestServices()
{
	cx::messageManager()->initialize();

	TestServicesType data = createDummyCoreServices();

	mDataFactory = data.mDataFactory;
	mDataService = data.mDataService;
	mSpaceProvider = data.mSpaceProvider;
	mTrackingService = data.mTrackingService;
}

TestServices::~TestServices()
{
	destroyDummyCoreServices(*this);
}

} // namespace cx
