/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxTrackingShapeGUIExtenderService.h"
#include "ctkPluginContext.h"
#include "cxShapeSensorWidget.h"
#include "cxVisServices.h"

namespace cx
{

TrackingShapeGUIExtenderService::TrackingShapeGUIExtenderService(ctkPluginContext *context) :
  mContext(context)
{
}

std::vector<GUIExtenderService::CategorizedWidget> TrackingShapeGUIExtenderService::createWidgets() const
{
	std::vector<CategorizedWidget> retval;

	VisServicesPtr services = VisServices::create(mContext);

	retval.push_back(GUIExtenderService::CategorizedWidget(
			new ShapeSensorWidget(services),
			"Utility"));

	return retval;
}


} /* namespace cx */
