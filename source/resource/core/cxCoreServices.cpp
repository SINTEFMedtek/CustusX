/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxCoreServices.h"

#include <ctkPluginContext.h>
#include "cxPatientModelServiceProxy.h"
#include "cxTrackingServiceProxy.h"
#include "cxVideoServiceProxy.h"
#include "cxSpaceProviderImpl.h"
#include "cxSessionStorageServiceProxy.h"
#include "cxStateServiceProxy.h"

namespace cx {

CoreServicesPtr CoreServices::create(ctkPluginContext* context)
{
	return CoreServicesPtr(new CoreServices(context));
}

CoreServices::CoreServices(ctkPluginContext* context)
{
	mPatientModelService	= PatientModelServicePtr(new PatientModelServiceProxy(context));
	mTrackingService		= TrackingServicePtr(new TrackingServiceProxy(context));
	mVideoService			= VideoServicePtr(new VideoServiceProxy(context));
	mSpaceProvider.reset(new SpaceProviderImpl(mTrackingService, mPatientModelService));
	mSessionStorageService	= SessionStorageServiceProxy::create(context);
	mStateService			= StateServiceProxy::create(context);
}

CoreServicesPtr CoreServices::getNullObjects()
{
	return CoreServicesPtr(new CoreServices());
}

CoreServices::CoreServices()
{
	mPatientModelService	= PatientModelService::getNullObject();
	mTrackingService		= TrackingService::getNullObject();
	mVideoService			= VideoService::getNullObject();
	mSpaceProvider.reset(new SpaceProviderImpl(mTrackingService, mPatientModelService));
	mSessionStorageService	= SessionStorageService::getNullObject();
	mStateService			= StateService::getNullObject();
}
} // cx
