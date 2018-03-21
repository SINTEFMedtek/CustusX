/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRegistrationMethodManualImageToPatientService.h"
#include "cxManualPatientRegistrationWidget.h"

namespace cx
{

RegistrationMethodManualImageToPatientService::RegistrationMethodManualImageToPatientService(RegServicesPtr services) :
	RegistrationMethodManualService(services)
{
}

QWidget* RegistrationMethodManualImageToPatientService::createWidget()
{
	QWidget *retval = new ManualPatientRegistrationWidget(mServices, NULL, this->getWidgetName());
	return retval;
}

} //cx
