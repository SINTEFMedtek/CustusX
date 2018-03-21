/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRegistrationGUIExtenderService.h"
#include "ctkPluginContext.h"
#include "cxRegistrationWidget.h"
#include "cxWirePhantomWidget.h"

namespace cx
{


RegistrationGUIExtenderService::RegistrationGUIExtenderService(ctkPluginContext *context) :
  mContext(context)
{
}

RegistrationGUIExtenderService::~RegistrationGUIExtenderService()
{
}

std::vector<GUIExtenderService::CategorizedWidget> RegistrationGUIExtenderService::createWidgets() const
{
	std::vector<CategorizedWidget> retval;

	retval.push_back(GUIExtenderService::CategorizedWidget(
										 new RegistrationWidget(mContext), "Algorithms"));

	retval.push_back(GUIExtenderService::CategorizedWidget(
										 new WirePhantomWidget(mContext), "Utility"));

	return retval;
}

} /* namespace cx */
