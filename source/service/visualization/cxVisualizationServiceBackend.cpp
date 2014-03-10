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

#include "cxVisualizationServiceBackend.h"

#include "sscDataManager.h"
#include "sscToolManager.h"
#include "cxSpaceProvider.h"

namespace cx
{

VisualizationServiceBackend::VisualizationServiceBackend(DataServicePtr dataManager,
														 TrackingServicePtr toolManager,
														 VideoServicePtr videoService,
														 SpaceProviderPtr spaceProvider) :
	mDataManager(dataManager),
	mToolManager(toolManager),
	mSpaceProvider(spaceProvider),
  mVideoService(videoService)
{

}

DataServicePtr VisualizationServiceBackend::getDataManager()
{
	return mDataManager;
}

TrackingServicePtr VisualizationServiceBackend::getToolManager()
{
	return mToolManager;
}

VideoServicePtr VisualizationServiceBackend::getVideoService()
{
	return mVideoService;
}

SpaceProviderPtr VisualizationServiceBackend::getSpaceProvider()
{
	return mSpaceProvider;
}


} // namespace cx

