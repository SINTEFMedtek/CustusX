/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#include "cxtestDummyDataManager.h"

#include "cxDataManagerImpl.h"
//#include "cxtestSpaceProviderMock.h"
#include "cxDataFactory.h"
#include "cxDummyToolManager.h"
#include "cxSpaceProviderImpl.h"
#include "cxReporter.h"
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

	cx::Reporter::shutdown();
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
	cx::reporter()->initialize();

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
