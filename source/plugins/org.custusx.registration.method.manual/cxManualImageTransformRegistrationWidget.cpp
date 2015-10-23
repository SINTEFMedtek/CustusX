/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
	 this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
	 this list of conditions and the following disclaimer in the documentation
	 and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
	 may be used to endorse or promote products derived from this software
	 without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
	mServices->registration()->applyImage2ImageRegistration(delta_pre_rMd, "Manual Image rMd");
}

} //cx
