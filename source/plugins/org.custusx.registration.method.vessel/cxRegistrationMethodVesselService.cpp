/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRegistrationMethodVesselService.h"
#include "cxRegisterI2IWidget.h"
#include "cxPrepareVesselsWidget.h"
#include "cxTabbedWidget.h"

namespace cx
{

QWidget *RegistrationMethodVesselImageToImageService::createWidget()
{	
	TabbedWidget* topWidget = new TabbedWidget(NULL, this->getWidgetName(), "Image 2 Image Registration");

	RegisterI2IWidget* registerWidget = new RegisterI2IWidget(mServices, topWidget);
	PrepareVesselsWidget* prepareRegistrationWidget = new PrepareVesselsWidget(mServices, topWidget);

	topWidget->addTab(prepareRegistrationWidget, "Prepare"); //should be application specific
	topWidget->addTab(registerWidget, "Register");

	return topWidget;
}

} /* namespace cx */
