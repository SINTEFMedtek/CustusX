/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/
#ifndef CXLEGACYSINGLETONS_H
#define CXLEGACYSINGLETONS_H

#include "cxLogicManagerExport.h"

#include "boost/shared_ptr.hpp"

namespace cx
{
class ViewManager;
typedef boost::shared_ptr<class SpaceProvider> SpaceProviderPtr;
typedef boost::shared_ptr<class TrackingService> TrackingServicePtr;
typedef boost::shared_ptr<class PatientModelService> PatientModelServicePtr;
typedef boost::shared_ptr<class VideoService> VideoServicePtr;
typedef boost::shared_ptr<class StateService> StateServicePtr;
typedef boost::shared_ptr<class ViewService> ViewServicePtr;
typedef boost::shared_ptr<class AcquisitionService> AcquisitionServicePtr;
typedef boost::shared_ptr<class SessionStorageService> SessionStorageServicePtr;

/** 
 *
 *
 * \ingroup cx
 * \date 23.02.2014, 2014
 * \author christiana
 */

// quick access to core services stored in the logic manager:
cxLogicManager_EXPORT SpaceProviderPtr spaceProvider();
cxLogicManager_EXPORT TrackingServicePtr trackingService();
cxLogicManager_EXPORT PatientModelServicePtr patientService();
cxLogicManager_EXPORT VideoServicePtr videoService();
cxLogicManager_EXPORT StateServicePtr stateService();
cxLogicManager_EXPORT ViewServicePtr viewService();
cxLogicManager_EXPORT AcquisitionServicePtr acquisitionService();
cxLogicManager_EXPORT SessionStorageServicePtr sessionStorageService();

} // namespace cx


#endif // CXLEGACYSINGLETONS_H
