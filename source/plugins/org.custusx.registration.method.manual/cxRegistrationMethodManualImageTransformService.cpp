/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRegistrationMethodManualImageTransformService.h"
#include "cxManualImageTransformRegistrationWidget.h"

namespace cx
{

RegistrationMethodManualImageTransformService::RegistrationMethodManualImageTransformService(RegServicesPtr services) :
	RegistrationMethodManualService(services)
{
}

QWidget* RegistrationMethodManualImageTransformService::createWidget()
{
	QWidget *retval = new ManualImageTransformRegistrationWidget(mServices, NULL, this->getWidgetName());
	return retval;
}

} //cx
