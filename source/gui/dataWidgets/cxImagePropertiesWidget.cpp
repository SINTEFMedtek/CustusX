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

#include "cxImagePropertiesWidget.h"

#include <QComboBox>
#include <QVBoxLayout>
#include "cxImage.h"
#include "cxPatientModelService.h"

namespace cx
{

ImagePropertiesWidget::ImagePropertiesWidget(PatientModelServicePtr patientModelService, QWidget* parent) :
		BaseWidget(parent, "ImagePropertiesWidget", "Image Properties"),
		mPatientModelService(patientModelService)
{
	mInterpolationType = new QComboBox(this);
	mInterpolationType->insertItem(0, "Nearest");
	mInterpolationType->insertItem(1, "Linear");
	mInterpolationType->insertItem(2, "Cubic");
	mInterpolationType->setToolTip("Change VTK interpolation type");

	connect(mInterpolationType, SIGNAL(currentIndexChanged(int)), this, SLOT(interpolationTypeChanged(int)));

	mActiveImageProxy = ActiveImageProxy::New(patientModelService);
	connect(mActiveImageProxy.get(), &ActiveImageProxy::activeImageChanged, this, &ImagePropertiesWidget::activeImageChangedSlot);

	QLabel* interpolationTypeLabel = new QLabel("Volume interpolation type", this);

	QHBoxLayout* layout =  new QHBoxLayout(this);
	layout->addWidget(interpolationTypeLabel);
	layout->addWidget(mInterpolationType);
}

void ImagePropertiesWidget::interpolationTypeChanged(int index)
{
	ImagePtr image = mPatientModelService->getActiveImage();
	if (image)
		image->setInterpolationType(index);
}

void ImagePropertiesWidget::activeImageChangedSlot()
{
	ImagePtr activeImage = mPatientModelService->getActiveImage();
	if (activeImage)
	{
		mInterpolationType->setCurrentIndex(activeImage->getInterpolationType());
	}
}

QString ImagePropertiesWidget::defaultWhatsThis() const
{
	return "<html>"
		"<h3>Image properties</h3>"
		"<p>"
		"Set image (volume) properties."
		"</p>"
		"<p><i></i></p>"
		"</html>";
}

}//end namespace cx
