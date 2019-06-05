/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxWebServerGUIExtenderService.h"
#include "ctkPluginContext.h"
#include "cxWebServerWidget.h"
#include "cxVisServices.h"
#include "cxLogger.h"

namespace cx
{

WebServerGUIExtenderService::WebServerGUIExtenderService(ctkPluginContext *context)
{
	mVisServices = VisServices::create(context);
}

std::vector<GUIExtenderService::CategorizedWidget> WebServerGUIExtenderService::createWidgets() const
{
	std::vector<CategorizedWidget> retval;

	retval.push_back(GUIExtenderService::CategorizedWidget(
			new WebServerWidget(mVisServices),
			"Utility"));

	return retval;
}

} /* namespace cx */
