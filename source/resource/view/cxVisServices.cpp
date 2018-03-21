/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxVisServices.h"

#include "cxViewServiceProxy.h"

namespace cx {

VisServicesPtr VisServices::create(ctkPluginContext* context)
{
	return VisServicesPtr(new VisServices(context));
}

VisServices::VisServices(ctkPluginContext* context) :
	CoreServices(context)
{
	mViewService = ViewServicePtr(new ViewServiceProxy(context));
}

VisServicesPtr VisServices::getNullObjects()
{
	return VisServicesPtr(new VisServices());
}

VisServices::VisServices() :
	CoreServices()
{
	mViewService = cx::ViewService::getNullObject();
}
} // cx
