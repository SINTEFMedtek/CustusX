/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXCORESERVICES_H
#define CXCORESERVICES_H

#include "cxResourceExport.h"
#include <memory>
class ctkPluginContext;

namespace cx
{

typedef std::shared_ptr<class PatientModelService> PatientModelServicePtr;
typedef std::shared_ptr<class TrackingService> TrackingServicePtr;
typedef std::shared_ptr<class VideoService> VideoServicePtr;
typedef std::shared_ptr<class SpaceProvider> SpaceProviderPtr;
typedef std::shared_ptr<class CoreServices> CoreServicesPtr;
typedef std::shared_ptr<class SessionStorageService> SessionStorageServicePtr;
typedef std::shared_ptr<class StateService> StateServicePtr;
typedef std::shared_ptr<class FileManagerService> FileManagerServicePtr;

/**
 * Convenience class combining all services in resource/core.
 *
 * \ingroup cx_resource_core
 *
 * \date Nov 14 2014
 * \author Ole Vegard Solberg, SINTEF
 */
class cxResource_EXPORT CoreServices
{
public:
	static CoreServicesPtr create(ctkPluginContext* context);
	CoreServices(ctkPluginContext* context);
	static CoreServicesPtr getNullObjects();

	PatientModelServicePtr patient() { return mPatientModelService; }
	TrackingServicePtr tracking() { return mTrackingService; }
	VideoServicePtr video() { return mVideoService; }
	SpaceProviderPtr spaceProvider() { return mSpaceProvider; }
	SessionStorageServicePtr session() { return mSessionStorageService; }
	StateServicePtr state() { return mStateService; }
	FileManagerServicePtr file() {return mFileManagerService;}

protected:
	PatientModelServicePtr mPatientModelService;
	TrackingServicePtr mTrackingService;
	VideoServicePtr mVideoService;
	SpaceProviderPtr mSpaceProvider;
	SessionStorageServicePtr mSessionStorageService;
	StateServicePtr mStateService;
	FileManagerServicePtr mFileManagerService;

protected:
	CoreServices();
};

}


#endif // CXCORESERVICES_H
