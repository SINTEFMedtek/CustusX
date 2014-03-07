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

StateServiceBackend::StateServiceBackend(DataManager* dataManager,
										 TrackingServicePtr toolManager,
										 VideoService* videoService,
										 SpaceProviderPtr spaceProvider,
										 PatientService *patientService) :
	mDataManager(dataManager),
	mToolManager(toolManager),
	mSpaceProvider(spaceProvider),
	mVideoService(videoService),
	mPatientService(patientService)
{

}

DataManager* StateServiceBackend::getDataManager()
{
	return mDataManager;
}

TrackingServicePtr StateServiceBackend::getToolManager()
{
	return mToolManager;
}

VideoService* StateServiceBackend::getVideoService()
{
	return mVideoService;
}

SpaceProviderPtr StateServiceBackend::getSpaceProvider()
{
	return mSpaceProvider;
}

PatientService* StateServiceBackend::getPatientService()
{
	return mPatientService;
}


} // namespace cx
