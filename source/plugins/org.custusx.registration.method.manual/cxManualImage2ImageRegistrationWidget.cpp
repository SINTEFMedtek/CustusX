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

#include "cxManualImage2ImageRegistrationWidget.h"
#include "cxRegistrationService.h"
#include "cxData.h"
#include "cxRegistrationProperties.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxRegistrationService.h"

namespace cx
{

ManualImage2ImageRegistrationWidget::ManualImage2ImageRegistrationWidget(RegServicesPtr services, QWidget *parent, QString objectName) :
	ManualImageRegistrationWidget(services, parent, objectName, "Manual Image to Image Registration")
{
	StringPropertyBasePtr fixedImage(new StringPropertyRegistrationFixedImage(services->registration(), services->patient()));
	StringPropertyBasePtr movingImage(new StringPropertyRegistrationMovingImage(services->registration(), services->patient()));

	LabeledComboBoxWidget* fixed = new LabeledComboBoxWidget(this, fixedImage);
	LabeledComboBoxWidget* moving = new LabeledComboBoxWidget(this, movingImage);

	mVerticalLayout->insertWidget(0, fixed);
	mVerticalLayout->insertWidget(1, moving);
}

QString ManualImage2ImageRegistrationWidget::getDescription()
{
	if (this->isValid())
		return QString("<b>Matrix fMm from moving to fixed image</b>");
	else
		return "<Invalid matrix>";
}

bool ManualImage2ImageRegistrationWidget::isValid() const
{
	return mServices->registration()->getMovingData() && mServices->registration()->getFixedData();
}

Transform3D ManualImage2ImageRegistrationWidget::getMatrixFromBackend()
{
	if (!this->isValid())
		return Transform3D::Identity();

	Transform3D rMm = mServices->registration()->getMovingData()->get_rMd();
	Transform3D rMf = mServices->registration()->getFixedData()->get_rMd();
	Transform3D fMm = rMf.inv() * rMm;
	return fMm;
}

void ManualImage2ImageRegistrationWidget::setMatrixFromWidget(Transform3D M)
{
	if (!this->isValid())
		return;

	Transform3D rMm = mServices->registration()->getMovingData()->get_rMd();
	Transform3D rMf = mServices->registration()->getFixedData()->get_rMd();
	Transform3D fQm = M; // the modified fMm matrix

	// start with
	//   new registration = new registration
	//                fQm = fMr * rMm'
	//                fQm = fMr * delta * rMm
	Transform3D delta = rMf * fQm * rMm.inv();

	mServices->registration()->addImage2ImageRegistration(delta, "Manual Image");
}
} // cx
