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

#include "cxBronchoscopyNavigationPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxBronchoscopyNavigationGUIExtenderService.h"
#include "cxRegisteredService.h"
#include "cxTrackingSystemBronchoscopyService.h"
#include "cxVisServices.h"

namespace cx
{

BronchoscopyNavigationPluginActivator::BronchoscopyNavigationPluginActivator()
{
}

BronchoscopyNavigationPluginActivator::~BronchoscopyNavigationPluginActivator()
{}

void BronchoscopyNavigationPluginActivator::start(ctkPluginContext* context)
{
	VisServicesPtr services = VisServices::create(context);

	BronchoscopyNavigationGUIExtenderService *bronchoscopyNavigationService = new BronchoscopyNavigationGUIExtenderService(services);
	mRegistration = RegisteredServicePtr(new RegisteredService(context, bronchoscopyNavigationService, GUIExtenderService_iid));

	BronchoscopePositionProjectionPtr projectionCenterlinePtr = BronchoscopePositionProjectionPtr(new BronchoscopePositionProjection());

//	TrackingSystemBronchoscopyService* trackingSystemBronchoscopyService = new TrackingSystemBronchoscopyService(services->tracking(), projectionCenterlinePtr);
//	mRegistrationTracking = RegisteredService::create<TrackingSystemBronchoscopyService>(context,trackingSystemBronchoscopyService, TrackingSystemService_iid);

}

void BronchoscopyNavigationPluginActivator::stop(ctkPluginContext* context)
{
	mRegistration.reset();
//	mRegistrationTracking.reset();
	Q_UNUSED(context);
}

} // namespace cx



