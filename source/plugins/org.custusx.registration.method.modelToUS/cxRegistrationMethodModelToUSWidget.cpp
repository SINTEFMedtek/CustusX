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

#include "cxRegistrationMethodModelToUSWidget.h"
#include <ctkPluginContext.h>
#include <QLabel>
#include <QVBoxLayout>
#include "cxRegistrationService.h"
#include "cxPatientModelService.h"
#include "cxRegistrationMethodToUSPrepareWidget.h"
//#include "cxRegisterI2IWidget.h"


namespace cx
{

RegistrationMethodModelToUSWidget::RegistrationMethodModelToUSWidget(regServices services,
																	 QWidget* parent, QString objectName) :
	TabbedWidget(parent, objectName, "Model to US Registration")
{
	//mVerticalLayout->addWidget(new QLabel("Hello Plugin!"));

	RegistrationMethodToUSPrepareWidget* prepareWidget = new RegistrationMethodToUSPrepareWidget(services, NULL, "PrepareModelToUSRegistrationWidget");
	this->addTab(prepareWidget, "Prepare");

	//Try to reuse existing widget. Cannot use this one as this is only meant ot be used with vessel2vessel registration
//	Image2ImageRegistrationWidget* image2imageWidget = new Image2ImageRegistrationWidget(NULL, "Image2ImageRegistrationWidget", "Image 2 Image Registration");
//	this->addTab(new RegisterI2IWidget(services, image2imageWidget),"Register");
}

RegistrationMethodModelToUSWidget::~RegistrationMethodModelToUSWidget()
{
}

QString RegistrationMethodModelToUSWidget::defaultWhatsThis() const
{
  return "<html>"
	  "<h3>Model to US registration.</h3>"
	  "<p>Register a surface model of a vessel to a 3D/4D ultrasound volume</p>"
      "</html>";
}



} /* namespace cx */
