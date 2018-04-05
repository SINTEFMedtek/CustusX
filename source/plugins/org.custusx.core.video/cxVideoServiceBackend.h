/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVIDEOSERVICEBACKEND_H
#define CXVIDEOSERVICEBACKEND_H

#include "org_custusx_core_video_Export.h"
#include <QSharedPointer>
#include "boost/shared_ptr.hpp"
#include "cxForwardDeclarations.h"
class ctkPluginFramework;
class ctkPluginContext;

namespace cx
{
typedef boost::shared_ptr<class SpaceProvider> SpaceProviderPtr;

typedef boost::shared_ptr<class VideoServiceBackend> VideoServiceBackendPtr;
/**
 *
 *
 * \ingroup org_custusx_core_video
 * \date 25.02.2014, 2014
 * \author christiana
 */
class org_custusx_core_video_EXPORT VideoServiceBackend
{
public:
	static VideoServiceBackendPtr create(PatientModelServicePtr dataManager,
								TrackingServicePtr trackingService,
								SpaceProviderPtr spaceProvider,
								FileManagerServicePtr filemanager,
								ctkPluginContext* context);
	VideoServiceBackend(PatientModelServicePtr dataManager,
								TrackingServicePtr trackingService,
								SpaceProviderPtr spaceProvider,
								FileManagerServicePtr filemanager,
								ctkPluginContext* context);

	PatientModelServicePtr getDataManager();
	TrackingServicePtr tracking();
	SpaceProviderPtr getSpaceProvider();
	FileManagerServicePtr file();
	ctkPluginContext* mContext;

private:
	PatientModelServicePtr mDataManager;
	TrackingServicePtr mTrackingService;
	SpaceProviderPtr mSpaceProvider;
	FileManagerServicePtr mFileManagerService;
};

} // namespace cx


#endif // CXVIDEOSERVICEBACKEND_H
