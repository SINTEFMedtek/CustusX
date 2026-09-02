/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxLiverGUIExtenderService.h"
#include "ctkPluginContext.h"
#include "cxLiverWidget.h"
#include "cxVisServices.h"

namespace cx
{

LiverGUIExtenderService::LiverGUIExtenderService(ctkPluginContext *context) :
	mContext(context)
{
}

std::vector<GUIExtenderService::CategorizedWidget> LiverGUIExtenderService::createWidgets() const
{
	std::vector<CategorizedWidget> retval;

	VisServicesPtr services = VisServices::create(mContext);

	retval.push_back(GUIExtenderService::CategorizedWidget(
			new LiverWidget(services),
			"Liver"));

	return retval;
}

} /* namespace cx */
