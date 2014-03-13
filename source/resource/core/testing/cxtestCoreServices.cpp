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

#include "catch.hpp"
#include "cxDataManagerImpl.h"
#include "cxtestDummyDataManager.h"
#include "cxReporter.h"
#include "cxMessageListener.h"

namespace cx
{

TEST_CASE("Core test services correctly contructed/destructed", "[unit]")
{
	cx::MessageListenerPtr messageListener = cx::MessageListener::create();

	cxtest::TestServicesPtr services = cxtest::TestServices::create();
	services.reset();

	CHECK(!messageListener->containsErrors());

//	// [HACK] break loop by removing connection to DataFactory and SpaceProvider
//	cx::DataManagerImplPtr dataManagerImpl;
//	dataManagerImpl = boost::dynamic_pointer_cast<cx::DataManagerImpl>(services.mDataService);
//	if (dataManagerImpl)
//	{
//		dataManagerImpl->setSpaceProvider(cx::SpaceProviderPtr());
//		dataManagerImpl->setDataFactory(cx::DataFactoryPtr());
//		dataManagerImpl.reset();
//	}

//	CHECK(services.mDataFactory.unique());
//	services.mDataFactory.reset();

//	CHECK(services.mSpaceProvider.unique());
//	services.mSpaceProvider.reset();

//	CHECK(services.mDataService.unique());
//	services.mDataService.reset();

//	CHECK(services.mTrackingService.unique());
//	services.mTrackingService.reset();

//	cx::MessageManager::shutdown();
}

} // namespace cx


