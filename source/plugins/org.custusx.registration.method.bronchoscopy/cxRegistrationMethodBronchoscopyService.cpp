/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRegistrationMethodBronchoscopyService.h"
#include "cxBronchoscopyRegistrationWidget.h"
#include "cxBronchoscopyImage2ImageRegistrationWidget.h"

namespace cx
{

QWidget *RegistrationMethodBronchoscopyImageToPatientService::createWidget()
{
	BronchoscopyRegistrationWidget* bronchoscopyRegistrationWidget = new BronchoscopyRegistrationWidget(mServices, NULL);
	return bronchoscopyRegistrationWidget;
}

QWidget *RegistrationMethodBronchoscopyImageToImageService::createWidget()
{
	BronchoscopyImage2ImageRegistrationWidget* bronchoscopyImage2ImageRegistrationWidget = new BronchoscopyImage2ImageRegistrationWidget(mServices, NULL);
	return bronchoscopyImage2ImageRegistrationWidget;
}

} /* namespace cx */
