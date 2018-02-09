/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2017, SINTEF Department of Medical Technology
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

#include "cxBronchoscopyImage2ImageRegistrationWidget.h"
#include <vtkPolyData.h>
#include "cxTransform3D.h"
#include "cxDataSelectWidget.h"
#include "cxMesh.h"
#include "cxSelectDataStringProperty.h"
#include "cxView.h"
#include "cxBronchoscopyRegistration.h"
#include "cxLogger.h"
#include "cxTypeConversions.h"
#include "cxPatientModelService.h"
#include "cxRegistrationService.h"
#include "cxViewService.h"
#include "cxStringProperty.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxDoubleProperty.h"
#include "cxProfile.h"
#include "cxHelperWidgets.h"
#include "cxBoolProperty.h"
#include "cxWidgetObscuredListener.h"
#include "cxViewGroupData.h"
#include "cxHelperWidgets.h"
#include "cxRegServices.h"

namespace cx
{
BronchoscopyImage2ImageRegistrationWidget::BronchoscopyImage2ImageRegistrationWidget(RegServicesPtr services, QWidget* parent) :
    RegistrationBaseWidget(services, parent, "org_custusx_registration_method_bronchoscopy_image2image_widget",
                           "Bronchoscopy Registration"),
    mBronchoscopyRegistration(new BronchoscopyRegistration())
{
    mVerticalLayout = new QVBoxLayout(this);
}

void BronchoscopyImage2ImageRegistrationWidget::prePaintEvent()
{
    this->setup();
}

void BronchoscopyImage2ImageRegistrationWidget::setup()
{
    mOptions = profile()->getXmlSettings().descend("bronchoscopyregistrationimage2imagewidget");

    mSelectMeshFixedWidget = StringPropertySelectMesh::New(mServices->patient());
    mSelectMeshFixedWidget->setValueName("Centerline fixed image: ");

    mSelectMeshMovingWidget = StringPropertySelectMesh::New(mServices->patient());
    mSelectMeshMovingWidget->setValueName("Centerline moving image: ");

    connect(mServices->patient().get(),&PatientModelService::patientChanged,this,&BronchoscopyImage2ImageRegistrationWidget::clearDataOnNewPatient);

    mRegisterButton = new QPushButton("Register");
    connect(mRegisterButton, SIGNAL(clicked()), this, SLOT(registerSlot()));
    mRegisterButton->setToolTip(this->defaultWhatsThis());

    mVerticalLayout->setMargin(0);
    mVerticalLayout->addWidget(new DataSelectWidget(mServices->view(), mServices->patient(), this, mSelectMeshFixedWidget));
    mVerticalLayout->addWidget(new DataSelectWidget(mServices->view(), mServices->patient(), this, mSelectMeshMovingWidget));

    mVerticalLayout->addWidget(mRegisterButton);

    mVerticalLayout->addStretch();
}

QString BronchoscopyImage2ImageRegistrationWidget::defaultWhatsThis() const
{
    return QString("Registration of CT images based on airway centerlines");
}

void BronchoscopyImage2ImageRegistrationWidget::registerSlot()
{

    if(!mSelectMeshFixedWidget->getMesh())
    {
        reportError("Fixed centerline missing");
        return;
    }
    if(!mSelectMeshMovingWidget->getMesh())
    {
        reportError("Moving centerline missing");
        return;
    }

    vtkPolyDataPtr centerlineFixed = mSelectMeshFixedWidget->getMesh()->getVtkPolyData();//input
    Transform3D rMdFixed = mSelectMeshFixedWidget->getMesh()->get_rMd();

    vtkPolyDataPtr centerlineMoving = mSelectMeshMovingWidget->getMesh()->getVtkPolyData();//input
    Transform3D rMdMoving = mSelectMeshMovingWidget->getMesh()->get_rMd();

    Transform3D centerlineFixed_M_centerlineMoving = Transform3D(mBronchoscopyRegistration->runBronchoscopyRegistrationImage2Image(centerlineFixed, centerlineMoving));

    DataPtr fixedData = mSelectMeshFixedWidget->getData();
    mServices->registration()->setFixedData(fixedData);
    DataPtr movingData = mSelectMeshMovingWidget->getData();
    mServices->registration()->setMovingData(movingData);

    Transform3D dMd = rMdFixed * centerlineFixed_M_centerlineMoving * rMdMoving.inv();//output
    mServices->registration()->addImage2ImageRegistration(dMd, "Bronchoscopy: centerline to centerline");
}

void BronchoscopyImage2ImageRegistrationWidget::clearDataOnNewPatient()
{
    mMeshFixed.reset();
    mMeshMoving.reset();
}

} //namespace cx
