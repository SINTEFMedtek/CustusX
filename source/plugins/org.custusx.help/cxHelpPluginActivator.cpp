/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxHelpPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxHelpGUIExtenderService.h"
#include "cxRegisteredService.h"
#include "cxHelpEngine.h"

namespace cx
{

HelpPluginActivator::HelpPluginActivator()
{
//	std::cout << "Created HelpPluginActivator" << std::endl;
}

HelpPluginActivator::~HelpPluginActivator()
{}

void HelpPluginActivator::start(ctkPluginContext* context)
{
	mEngine.reset(new HelpEngine);

	HelpGUIExtenderService* guiExtender = new HelpGUIExtenderService(context, mEngine);
	mGUIExtender = RegisteredService::create<HelpGUIExtenderService>(context, guiExtender, GUIExtenderService_iid);
}

void HelpPluginActivator::stop(ctkPluginContext* context)
{
	mGUIExtender.reset();
	mEngine.reset();
    Q_UNUSED(context);
}

} // namespace cx



