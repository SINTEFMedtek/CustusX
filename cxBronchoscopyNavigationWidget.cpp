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

#include "cxBronchoscopyNavigationWidget.h"
#include <QLabel>
#include <QVBoxLayout>
#include "cxMesh.h"
//#include "cxDataSelectWidget.h"
#include "cxRegistrationService.h"

namespace cx
{

BronchoscopyNavigationWidget::BronchoscopyNavigationWidget(regServices services, QWidget* parent) :
    QWidget(parent),
    mVerticalLayout(new QVBoxLayout(this))
{
	this->setObjectName("BronchoscopyNavigationWidget");
	this->setWindowTitle("BronchoscopyNavigation");
    this->setWhatsThis(this->defaultWhatsThis());

	mSelectMeshWidget = SelectMeshStringDataAdapter::New(services.patientModelService);
	mSelectMeshWidget->setValueName("Centerline: ");

	mEnableButton = new QPushButton("Enable");
	connect(mEnableButton, SIGNAL(clicked()), this, SLOT(enableSlot()));
	mEnableButton->setToolTip(this->defaultWhatsThis());

	mDisableButton = new QPushButton("Disable");
	connect(mDisableButton, SIGNAL(clicked()), this, SLOT(disableSlot()));
	mDisableButton->setToolTip(this->defaultWhatsThis());



    mVerticalLayout->addWidget(new QLabel("Hello Plugin!"));
}

BronchoscopyNavigationWidget::~BronchoscopyNavigationWidget()
{
}

void BronchoscopyNavigationWidget::enableSlot()
{

}

void BronchoscopyNavigationWidget::disableSlot()
{

}

QString BronchoscopyNavigationWidget::defaultWhatsThis() const
{
  return "<html>"
	  "<h3>BronchoscopyNavigation plugin.</h3>"
	  "<p>Locks tool position to CT centerline.</p>"
      "</html>";
}



} /* namespace cx */
