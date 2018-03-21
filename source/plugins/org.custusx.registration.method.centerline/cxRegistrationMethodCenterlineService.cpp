/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRegistrationMethodCenterlineService.h"
#include "cxCenterlineRegistrationWidget.h"
#include "cxTabbedWidget.h"
#include "cxCenterlinePointsWidget.h"

namespace cx
{

QWidget *RegistrationMethodCenterlineImageToPatientService::createWidget()
{
    TabbedWidget* tabWidget = new TabbedWidget(NULL, this->getWidgetName(), "Centerline Registration");

    CenterlineRegistrationWidget* centerlineRegistrationWidget = new CenterlineRegistrationWidget(mServices, NULL);

    CenterlinePointsWidget* centerlinePointsWidget = new CenterlinePointsWidget(mServices, tabWidget, "org_custusx_registration_method_centerline_landmarks_widget", "Image Registration");

    tabWidget->addTab(centerlinePointsWidget, "Prepare");
    tabWidget->addTab(centerlineRegistrationWidget, "Register");
    tabWidget->setDefaultWidget(centerlineRegistrationWidget);

    return tabWidget;
}

} /* namespace cx */
