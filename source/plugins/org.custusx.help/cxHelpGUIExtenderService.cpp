/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxHelpGUIExtenderService.h"
#include "ctkPluginContext.h"
#include "cxHelpWidget.h"

namespace cx
{


HelpGUIExtenderService::HelpGUIExtenderService(ctkPluginContext *context, HelpEnginePtr engine) :
  mContext(context),
  mEngine(engine)
{
}

HelpGUIExtenderService::~HelpGUIExtenderService()
{
}

std::vector<GUIExtenderService::CategorizedWidget> HelpGUIExtenderService::createWidgets() const
{
	std::vector<CategorizedWidget> retval;

	retval.push_back(GUIExtenderService::CategorizedWidget(
			new HelpWidget(mEngine),
			"Browsing"));

	return retval;
}


} /* namespace cx */
