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

#include "cxStateServiceBackend.h"

#include "sscDataManager.h"
#include "sscToolManager.h"
#include "cxSpaceProvider.h"
#include "cxPatientService.h"

namespace cx
{

StateServiceBackend::StateServiceBackend(DataServicePtr dataManager,
										 TrackingServicePtr toolManager,
										 VideoServicePtr videoService,
										 SpaceProviderPtr spaceProvider,
										 PatientServicePtr patientService) :
	mDataManager(dataManager),
	mToolManager(toolManager),
	mSpaceProvider(spaceProvider),
	mVideoService(videoService),
	mPatientService(patientService)
{

}

DataServicePtr StateServiceBackend::getDataManager()
{
	return mDataManager;
}

TrackingServicePtr StateServiceBackend::getToolManager()
{
	return mToolManager;
}

VideoServicePtr StateServiceBackend::getVideoService()
{
	return mVideoService;
}

SpaceProviderPtr StateServiceBackend::getSpaceProvider()
{
	return mSpaceProvider;
}

PatientServicePtr StateServiceBackend::getPatientService()
{
	return mPatientService;
}


} // namespace cx
