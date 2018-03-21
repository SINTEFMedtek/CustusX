/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxSeansVesselRegistrationWidget.h"

#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>
#include <vtkCellArray.h>
#include "cxTypeConversions.h"
#include "cxLogger.h"
#include "cxTimedAlgorithm.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxRegistrationProperties.h"
#include "vesselReg/SeansVesselReg.hxx"
#include "cxMesh.h"
#include "cxView.h"
#include "cxGeometricRep.h"
#include "cxGraphicalPrimitives.h"
#include "cxRegistrationService.h"
#include "cxViewService.h"
#include "cxPatientModelService.h"
#include "cxICPWidget.h"
#include "cxMeshInView.h"
#include "cxWidgetObscuredListener.h"
#include "cxSpaceProvider.h"
#include "cxSpaceListener.h"
#include "cxProfile.h"

namespace cx
{

SeansVesselRegistrationWidget::SeansVesselRegistrationWidget(RegServicesPtr services, QWidget* parent) :
	ICPRegistrationBaseWidget(services, parent,
						   "org_custusx_registration_method_vessel_seans_widget",
						   "Seans Vessel Registration")
{
}

void SeansVesselRegistrationWidget::setup()
{
	mSpaceListenerMoving = mServices->spaceProvider()->createListener();
	mSpaceListenerFixed = mServices->spaceProvider()->createListener();
	connect(mSpaceListenerMoving.get(), &SpaceListener::changed, this, &SeansVesselRegistrationWidget::onSpacesChanged);
	connect(mSpaceListenerFixed.get(), &SpaceListener::changed, this, &SeansVesselRegistrationWidget::onSpacesChanged);

	connect(mServices->registration().get(), &RegistrationService::fixedDataChanged,
			this, &SeansVesselRegistrationWidget::inputChanged);
	connect(mServices->registration().get(), &RegistrationService::movingDataChanged,
			this, &SeansVesselRegistrationWidget::inputChanged);

	QVBoxLayout* layout = new QVBoxLayout(this);
	QGridLayout* entryLayout = new QGridLayout;
	entryLayout->setColumnStretch(1, 1);

	mFixedImage.reset(new StringPropertyRegistrationFixedImage(mServices->registration(), mServices->patient()));
	new LabeledComboBoxWidget(this, mFixedImage, entryLayout, 0);
	mMovingImage.reset(new StringPropertyRegistrationMovingImage(mServices->registration(), mServices->patient()));
	new LabeledComboBoxWidget(this, mMovingImage, entryLayout, 1);

	layout->addLayout(entryLayout);
	layout->addWidget(mICPWidget);

	this->inputChanged();
	this->onSettingsChanged();
}

SeansVesselRegistrationWidget::~SeansVesselRegistrationWidget()
{
}

void SeansVesselRegistrationWidget::initializeRegistrator()
{
	DataPtr moving = mServices->registration()->getMovingData();
	DataPtr fixed = mServices->registration()->getFixedData();
	QString logPath = mServices->patient()->getActivePatientFolder() + "/Logs/";

	mRegistrator->initialize(moving, fixed, logPath);
}

void SeansVesselRegistrationWidget::inputChanged()
{
	if (mObscuredListener->isObscured())
		return;

	DataPtr moving = mServices->registration()->getMovingData();
	DataPtr fixed = mServices->registration()->getFixedData();

	mSpaceListenerFixed->setSpace(mServices->spaceProvider()->getD(fixed));
	mSpaceListenerMoving->setSpace(mServices->spaceProvider()->getD(moving));

	this->onSpacesChanged();
}

void SeansVesselRegistrationWidget::applyRegistration(Transform3D delta)
{
	mServices->registration()->addImage2ImageRegistration(delta, "Vessel based");
}

void SeansVesselRegistrationWidget::onShown()
{
	this->inputChanged();
}





}//namespace cx
