/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxManualImageTransformRegistrationWidget.h"
#include "cxRegistrationService.h"
#include "cxData.h"

#include "cxRegistrationProperties.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxRegistrationTransform.h"

namespace cx
{

ManualImageTransformRegistrationWidget::ManualImageTransformRegistrationWidget(RegServicesPtr services, QWidget *parent, QString objectName) :
	ManualImageRegistrationWidget(services, parent, objectName, "Manual Image Transform Registration")
{
	StringPropertyBasePtr movingImage(new StringPropertyRegistrationMovingImage(services->registration(), services->patient()));

	LabeledComboBoxWidget* moving = new LabeledComboBoxWidget(this, movingImage);

	mVerticalLayout->insertWidget(1, moving);
}

QString ManualImageTransformRegistrationWidget::getDescription()
{
	if (mConnectedMovingImage)
		return QString("<b>Position Matrix rMd for data %1</b>").arg(mConnectedMovingImage->getName());
	else
		return "<Invalid matrix>";
}

Transform3D ManualImageTransformRegistrationWidget::getMatrixFromBackend()
{
	return mConnectedMovingImage->get_rMd();
}

void ManualImageTransformRegistrationWidget::setMatrixFromWidget(Transform3D M)
{
	Transform3D rMd = mConnectedMovingImage->get_rMd();
	Transform3D rMMd = M;

	Transform3D delta_pre_rMd = rMMd * rMd.inv(); // gives delta on the r (left) side.
	mServices->registration()->addImage2ImageRegistration(delta_pre_rMd, "Manual Image rMd");
}

} //cx
