/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRegistrationHistoryGUIExtenderService.h"
#include "ctkPluginContext.h"
//#include "cxRegistrationWidget.h"
#include "cxRegistrationHistoryWidget.h"

namespace cx
{


RegistrationHistoryGUIExtenderService::RegistrationHistoryGUIExtenderService(RegServicesPtr services) :
	mServices(services)
{
}

RegistrationHistoryGUIExtenderService::~RegistrationHistoryGUIExtenderService()
{
}

std::vector<GUIExtenderService::CategorizedWidget> RegistrationHistoryGUIExtenderService::createWidgets() const
{
	std::vector<CategorizedWidget> retval;

	retval.push_back(GUIExtenderService::CategorizedWidget(
						 new RegistrationHistoryWidget(mServices, NULL), "Browsing"));

	return retval;
}

std::vector<QToolBar *> RegistrationHistoryGUIExtenderService::createToolBars() const
{
	QToolBar* registrationHistoryToolBar = new QToolBar("Registration History");
	registrationHistoryToolBar->setObjectName("RegistrationHistoryToolBar");
	registrationHistoryToolBar->addWidget(new RegistrationHistoryWidget(mServices, registrationHistoryToolBar, true));

	std::vector<QToolBar*> retval;
	retval.push_back(registrationHistoryToolBar);
	return retval;
}

} /* namespace cx */
