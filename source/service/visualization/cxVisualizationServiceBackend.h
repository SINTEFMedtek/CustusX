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
#ifndef CXVISUALIZATIONSERVICEBACKEND_H
#define CXVISUALIZATIONSERVICEBACKEND_H

#include "boost/shared_ptr.hpp"
#include "cxForwardDeclarations.h"

namespace cx
{
class DataManager;
class ToolManager;
class VideoService;
typedef boost::shared_ptr<class SpaceProvider> SpaceProviderPtr;

typedef boost::shared_ptr<class VisualizationServiceBackend> VisualizationServiceBackendPtr;
/**
 *
 *
 * \ingroup cx
 * \date 2014-02-23
 * \author christiana
 */
class VisualizationServiceBackend
{
public:
	VisualizationServiceBackend(DataServicePtr dataManager,
								TrackingServicePtr toolManager,
								VideoServicePtr videoService,
								SpaceProviderPtr spaceProvider);

	DataServicePtr getDataManager();
	TrackingServicePtr getToolManager();
	VideoServicePtr getVideoService();
	SpaceProviderPtr getSpaceProvider();

private:
	DataServicePtr mDataManager;
	TrackingServicePtr mToolManager;
	SpaceProviderPtr mSpaceProvider;
	VideoServicePtr mVideoService;
};


} // namespace cx



#endif // CXVISUALIZATIONSERVICEBACKEND_H
