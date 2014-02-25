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

#ifndef CXVIDEOSERVICEBACKEND_H
#define CXVIDEOSERVICEBACKEND_H

#include "boost/shared_ptr.hpp"

namespace cx
{
class DataManager;
class ToolManager;
typedef boost::shared_ptr<class SpaceProvider> SpaceProviderPtr;

typedef boost::shared_ptr<class VideoServiceBackend> VideoServiceBackendPtr;
/**
 *
 *
 * \ingroup cx
 * \date 25.02.2014, 2014
 * \author christiana
 */
class VideoServiceBackend
{
public:
	static VideoServiceBackendPtr create(DataManager* dataManager,
								ToolManager* toolManager,
								SpaceProviderPtr spaceProvider);
	VideoServiceBackend(DataManager* dataManager,
								ToolManager* toolManager,
								SpaceProviderPtr spaceProvider);

	DataManager* getDataManager();
	ToolManager* getToolManager();
	SpaceProviderPtr getSpaceProvider();

private:
	DataManager* mDataManager;
	ToolManager* mToolManager;
	SpaceProviderPtr mSpaceProvider;
};

} // namespace cx


#endif // CXVIDEOSERVICEBACKEND_H
