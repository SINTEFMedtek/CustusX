/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxCalibrationGUIExtenderService.h"
#include "ctkPluginContext.h"
#include "cxCalibrationMethodsWidget.h"
#include "cxVisServices.h"
#include "cxAcquisitionServiceProxy.h"

namespace cx
{

CalibrationGUIExtenderService::CalibrationGUIExtenderService(ctkPluginContext *context) :
  mContext(context)
{
}

std::vector<GUIExtenderService::CategorizedWidget> CalibrationGUIExtenderService::createWidgets() const
{
	VisServicesPtr services = VisServices::create(mContext);
	AcquisitionServicePtr acquisitionService(new AcquisitionServiceProxy(mContext));

	std::vector<CategorizedWidget> retval;

	retval.push_back(GUIExtenderService::CategorizedWidget(
			new CalibrationMethodsWidget(services, acquisitionService, NULL, "calibration_methods_widget", "Calibration Methods"),
			"Algorithms"));

	return retval;
}


} /* namespace cx */
