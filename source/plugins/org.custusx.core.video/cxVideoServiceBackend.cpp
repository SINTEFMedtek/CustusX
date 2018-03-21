/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxVideoServiceBackend.h"

#include <ctkPluginFramework.h>
#include "cxTrackingService.h"
#include "cxSpaceProvider.h"

namespace cx
{

VideoServiceBackendPtr VideoServiceBackend::create(PatientModelServicePtr dataManager,
							TrackingServicePtr trackingService,
							SpaceProviderPtr spaceProvider,
												   ctkPluginContext* context)
{
	return VideoServiceBackendPtr(new VideoServiceBackend(dataManager, trackingService, spaceProvider, context));
}

VideoServiceBackend::VideoServiceBackend(PatientModelServicePtr dataManager,
							TrackingServicePtr trackingService,
							SpaceProviderPtr spaceProvider,
										 ctkPluginContext* context) :
	mDataManager(dataManager),
	mTrackingService(trackingService),
	mSpaceProvider(spaceProvider),
	mContext(context)
{

}

PatientModelServicePtr VideoServiceBackend::getDataManager()
{
	return mDataManager;
}

TrackingServicePtr VideoServiceBackend::tracking()
{
	return mTrackingService;
}

SpaceProviderPtr VideoServiceBackend::getSpaceProvider()
{
	return mSpaceProvider;
}

} // namespace cx

