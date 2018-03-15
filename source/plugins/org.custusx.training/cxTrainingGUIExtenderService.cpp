/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxTrainingGUIExtenderService.h"
#include "ctkPluginContext.h"
#include "cxNeuroTrainingWidget.h"
#include "cxRegServices.h"

namespace cx
{


TrainingGUIExtenderService::TrainingGUIExtenderService(ctkPluginContext *context) :
  mContext(context)
{
}

TrainingGUIExtenderService::~TrainingGUIExtenderService()
{
}

std::vector<GUIExtenderService::CategorizedWidget> TrainingGUIExtenderService::createWidgets() const
{
	std::vector<CategorizedWidget> retval;

	RegServicesPtr services = RegServices::create(mContext);

	retval.push_back(GUIExtenderService::CategorizedWidget(
			new NeuroTrainingWidget(services, mContext),
			"Simulator", true));

	return retval;
}


} /* namespace cx */
