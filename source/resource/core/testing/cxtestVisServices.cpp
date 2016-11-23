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

#include "cxtestVisServices.h"

#include "cxDataFactory.h"
#include "cxDummyToolManager.h"
#include "cxSpaceProviderImpl.h"
#include "cxReporter.h"
#include "cxSharedPointerChecker.h"
#include "cxtestPatientModelServiceMock.h"


/*
namespace
{
ViewServiceMocWithRenderWindowFactory::ViewServiceMocWithRenderWindowFactory()
{
	mRenderWindowFactory = cx::RenderWindowFactoryPtr(new cx::RenderWindowFactory());
}

vtkRenderWindowPtr ViewServiceMocWithRenderWindowFactory::getRenderWindow(QString uid, bool offScreenRendering)
{
	return mRenderWindowFactory->getRenderWindow(uid, offScreenRendering);
}

vtkRenderWindowPtr ViewServiceMocWithRenderWindowFactory::getSharedRenderWindow() const
{
	return mRenderWindowFactory->getSharedRenderWindow();
}

cx::RenderWindowFactoryPtr ViewServiceMocWithRenderWindowFactory::getRenderWindowFactory() const
{
	return mRenderWindowFactory;
}

///--------------------------------------------------------


}
*/

namespace cxtest
{


//TestServicesType createDummyCoreServices()
//{
//	TestServicesType retval;
//	retval.mPatientModelService.reset(new PatientModelServiceMock());
//	cx::TrackingServicePtr trackingService = cx::DummyToolManager::create();

//	cx::SpaceProviderPtr spaceProvider;
//	spaceProvider.reset(new cx::SpaceProviderImpl(trackingService, retval.mPatientModelService));
//	retval.mSpaceProvider = spaceProvider;
//	retval.mTrackingService = trackingService;
//	return retval;
//}

TestVisServicesPtr TestVisServices::create()
{
	return TestVisServicesPtr(new TestVisServices());
}

TestVisServices::TestVisServices() :
	cx::VisServices()
{
	cx::reporter()->initialize();
	this->mPatientModelService.reset(new PatientModelServiceMock());
	cx::TrackingServicePtr trackingService = cx::DummyToolManager::create();

	cx::SpaceProviderPtr spaceProvider;
	spaceProvider.reset(new cx::SpaceProviderImpl(trackingService, this->mPatientModelService));
	this->mSpaceProvider = spaceProvider;
	this->mTrackingService = trackingService;
	//this->mViewService = ::ViewServiceMocWithRenderWindowFactoryPtr(new ::ViewServiceMocWithRenderWindowFactory());
}

TestVisServices::~TestVisServices()
{
	cx::Reporter::shutdown();
}

//void destroyDummyCoreServices(TestServicesType& services)
//{
//	requireUnique(services.mSpaceProvider, "SpaceProvider");
//	services.mSpaceProvider.reset();

//	requireUnique(services.mPatientModelService, "PatientModelService");
//	services.mPatientModelService.reset();

//	requireUnique(services.mTrackingService, "TrackingService");
//	services.mTrackingService.reset();

//	cx::Reporter::shutdown();
//}

//TestServicesPtr TestServices::create()
//{
//	return TestServicesPtr(new TestServices());
//}

//TestServices::TestServices()
//{
//	cx::reporter()->initialize();

//	TestServicesType data = createDummyCoreServices();

//	mPatientModelService = data.mPatientModelService;
//	mSpaceProvider = data.mSpaceProvider;
//	mTrackingService = data.mTrackingService;
//}

//TestServices::~TestServices()
//{
//	destroyDummyCoreServices(*this);
//}


} // namespace cx
