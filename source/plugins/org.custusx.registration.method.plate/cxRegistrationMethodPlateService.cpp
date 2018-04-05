/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRegistrationMethodPlateService.h"
#include "cxPlateRegistrationWidget.h"
#include "cxPlateImageRegistrationWidget.h"
#include "cxTabbedWidget.h"

namespace cx
{

QWidget *RegistrationMethodPlateImageToPatientService::createWidget()
{
	TabbedWidget* topWidget = new TabbedWidget(NULL, this->getWidgetName(), "Plate");
	topWidget->setToolTip("Plate Registration");

	PlateImageRegistrationWidget* imageLandmarks = new PlateImageRegistrationWidget(mServices, topWidget);
	PlateRegistrationWidget* referenceLandmarks = new PlateRegistrationWidget(mServices, topWidget);

	topWidget->addTab(imageLandmarks, "Image landmarks");
	topWidget->addTab(referenceLandmarks, "Reference landmarks");

	return topWidget;
}

} /* namespace cx */
