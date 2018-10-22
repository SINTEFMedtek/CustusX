/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxtestVisServices.h"

#include "cxDataFactory.h"
#include "cxDummyToolManager.h"
#include "cxSpaceProviderImpl.h"
#include "cxReporter.h"
#include "cxSharedPointerChecker.h"
#include "cxtestPatientModelServiceMock.h"
#include "cxFileManagerService.h"


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
