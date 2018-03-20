/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

    Transform3D d_M_d = Transform3D(mBronchoscopyRegistration->runBronchoscopyRegistrationImage2Image(centerlineFixed, centerlineMoving));

    DataPtr fixedData = mSelectMeshFixedWidget->getData();
    mServices->registration()->setFixedData(fixedData);
    DataPtr movingData = mSelectMeshMovingWidget->getData();
    mServices->registration()->setMovingData(movingData);

    Transform3D delta_pre_rMd = rMdFixed * d_M_d * rMdMoving.inv();//output
    mServices->registration()->addImage2ImageRegistration(delta_pre_rMd, "Bronchoscopy: centerline to centerline");
}

void BronchoscopyImage2ImageRegistrationWidget::clearDataOnNewPatient()
{
    mMeshFixed.reset();
    mMeshMoving.reset();
}

} //namespace cx
