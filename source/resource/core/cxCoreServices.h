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
#include <boost/shared_ptr.hpp>
class ctkPluginContext;

namespace cx
{

typedef boost::shared_ptr<class PatientModelService> PatientModelServicePtr;
typedef boost::shared_ptr<class TrackingService> TrackingServicePtr;
typedef boost::shared_ptr<class VideoService> VideoServicePtr;
typedef boost::shared_ptr<class SpaceProvider> SpaceProviderPtr;
typedef boost::shared_ptr<class CoreServices> CoreServicesPtr;
typedef boost::shared_ptr<class SessionStorageService> SessionStorageServicePtr;
typedef boost::shared_ptr<class StateService> StateServicePtr;

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

protected:
	PatientModelServicePtr mPatientModelService;
	TrackingServicePtr mTrackingService;
	VideoServicePtr mVideoService;
	SpaceProviderPtr mSpaceProvider;
	SessionStorageServicePtr mSessionStorageService;
	StateServicePtr mStateService;

protected:
	CoreServices();
};

}


#endif // CXCORESERVICES_H
