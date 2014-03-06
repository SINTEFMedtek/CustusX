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
#ifndef CXSTATESERVICEBACKEND_H
#define CXSTATESERVICEBACKEND_H

#include "boost/shared_ptr.hpp"

namespace cx
{

class DataManager;
class ToolManager;
class VideoService;
typedef boost::shared_ptr<class SpaceProvider> SpaceProviderPtr;
typedef boost::shared_ptr<class StateServiceBackend> StateServiceBackendPtr;

/**
 *
 *
 * \ingroup cx
 * \date 2014-03-06
 * \author christiana
 */
class StateServiceBackend
{
public:
	StateServiceBackend(DataManager* dataManager,
								ToolManager* toolManager,
								VideoService* videoService,
								SpaceProviderPtr spaceProvider);

	DataManager* getDataManager();
	ToolManager* getToolManager();
	VideoService* getVideoService();
	SpaceProviderPtr getSpaceProvider();

private:
	DataManager* mDataManager;
	ToolManager* mToolManager;
	SpaceProviderPtr mSpaceProvider;
	VideoService* mVideoService;
};


} // namespace cx

#endif // CXSTATESERVICEBACKEND_H
