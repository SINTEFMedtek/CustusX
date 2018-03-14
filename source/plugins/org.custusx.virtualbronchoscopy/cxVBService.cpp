/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxVBService.h"
#include "ctkPluginContext.h"
#include "cxVBWidget.h"
#include "cxVisServices.h"

namespace cx
{

VBGUIExtenderService::VBGUIExtenderService(VisServicesPtr services) :
	mServices(services)
{
}
std::vector<GUIExtenderService::CategorizedWidget> VBGUIExtenderService::createWidgets() const
{
	std::vector<CategorizedWidget> retval;
	retval.push_back(GUIExtenderService::CategorizedWidget(
						 new VBWidget(mServices, NULL),
						 "Utility"));

	return retval;
}

} /* namespace cx */
