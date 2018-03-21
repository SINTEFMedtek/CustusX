/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include <cxRegisterI2IWidget.h>

#include "cxImageTF3D.h"
#include "cxImageLUT2D.h"
#include "cxTypeConversions.h"
#include "cxImage.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxFrameTreeWidget.h"
//#include "cxDataInterface.h"
#include "cxSeansVesselRegistrationWidget.h"

namespace cx
{


RegisterI2IWidget::RegisterI2IWidget(RegServicesPtr services, QWidget* parent) :
		RegistrationBaseWidget(services, parent, "org_custusx_registration_method_vessel_register_widget", "Register Image2Image"),
		mSeansVesselRegsitrationWidget(new SeansVesselRegistrationWidget(services, this))
{
  QVBoxLayout* topLayout = new QVBoxLayout(this);
  QGridLayout* layout = new QGridLayout();
  topLayout->addLayout(layout);

  layout->addWidget(mSeansVesselRegsitrationWidget);
  layout->addWidget(new QLabel("Parent frame tree status:"), 3, 0);
  layout->addWidget(new FrameTreeWidget(services->patient(), this), 4, 0);
}

RegisterI2IWidget::~RegisterI2IWidget()
{}

}
