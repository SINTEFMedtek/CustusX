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
#include <cxRegisterI2IWidget.h>

#include "cxImageTF3D.h"
#include "cxImageLUT2D.h"
#include "cxTypeConversions.h"
#include "cxImage.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxFrameTreeWidget.h"
#include "cxDataInterface.h"
#include "cxSeansVesselRegistrationWidget.h"

namespace cx
{


RegisterI2IWidget::RegisterI2IWidget(RegServices services, QWidget* parent) :
		RegistrationBaseWidget(services, parent, "RegisterI2IWidget", "Register Image2Image"),
		mSeansVesselRegsitrationWidget(new SeansVesselRegistrationWidget(services, this))
{
//  connect(registrationManager(), SIGNAL(fixedDataChanged(QString)), this, SLOT(fixedImageSlot(QString)));
//  connect(registrationManager(), SIGNAL(movingDataChanged(QString)), this, SLOT(movingImageSlot(QString)));

  QVBoxLayout* topLayout = new QVBoxLayout(this);
  QGridLayout* layout = new QGridLayout();
  topLayout->addLayout(layout);

  layout->addWidget(mSeansVesselRegsitrationWidget);
  layout->addWidget(new QLabel("Parent frame tree status:"), 3, 0);
  layout->addWidget(new FrameTreeWidget(this), 4, 0);
}

RegisterI2IWidget::~RegisterI2IWidget()
{}

QString RegisterI2IWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Registration of vessel segments to eachother.</h3>"
      "<p><i>Press the button to perform vessel based registration between image 1 and image 2s centerlines.</i></p>"
      "</html>";
}
//
//void RegisterI2IWidget::fixedImageSlot(QString uid)
//{
//  mSeansVesselRegsitrationWidget->fixedImageSlot(uid);
//}
//
//void RegisterI2IWidget::movingImageSlot(QString uid)
//{
//  mSeansVesselRegsitrationWidget->movingImageSlot(uid);
//}

}
