/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFastImageRegistrationWidget.h"


#include "cxPatientModelService.h"
#include "cxData.h"
#include "cxImage.h"
#include "cxRegistrationService.h"
#include "cxActiveData.h"

namespace cx
{

FastImageRegistrationWidget::FastImageRegistrationWidget(RegServicesPtr services, QWidget* parent, QString objectName, QString windowTitle, bool useRegistrationFixedPropertyInsteadOfActiveImage) :
	ImageLandmarksWidget(services, parent, objectName, windowTitle, useRegistrationFixedPropertyInsteadOfActiveImage)
{
}

FastImageRegistrationWidget::~FastImageRegistrationWidget()
{

}

void FastImageRegistrationWidget::performRegistration()
{
	//make sure the masterImage is set
	DataPtr fixedData = mServices->registration()->getFixedData();
	if(!fixedData)
	{
		ActiveDataPtr activeData = mServices->patient()->getActiveData();
		fixedData = activeData->getActive<Image>();
		mServices->registration()->setFixedData(fixedData);
	}

	this->updateAverageAccuracyLabel();
}

}//namespace cx
