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
							SpaceProviderPtr spaceProvider, FileManagerServicePtr filemanager, ctkPluginContext* context)
{
	return VideoServiceBackendPtr(new VideoServiceBackend(dataManager, trackingService, spaceProvider, filemanager, context));
}

VideoServiceBackend::VideoServiceBackend(PatientModelServicePtr dataManager,
							TrackingServicePtr trackingService,
							SpaceProviderPtr spaceProvider, FileManagerServicePtr filemanager, ctkPluginContext* context) :
	mContext(context),
	mDataManager(dataManager),
	mTrackingService(trackingService),
	mSpaceProvider(spaceProvider),
	mFileManagerService(filemanager)
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

FileManagerServicePtr VideoServiceBackend::file()
{
	return mFileManagerService;
}

} // namespace cx

