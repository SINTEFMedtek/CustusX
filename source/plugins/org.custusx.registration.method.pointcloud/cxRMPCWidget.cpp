/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxRMPCWidget.h"

#include "cxPatientModelService.h"
#include "cxRegistrationService.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxWidgetObscuredListener.h"
#include "cxRegistrationProperties.h"
#include "vesselReg/SeansVesselReg.hxx"
#include "cxICPWidget.h"
#include "cxSpaceListener.h"
#include "cxSpaceProvider.h"

namespace cx
{
RMPCWidget::RMPCWidget(RegServicesPtr services, QWidget* parent) :
	ICPRegistrationBaseWidget(services, parent, "org_custusx_registration_method_pointcloud_widget",
						   "Point Cloud Registration")
{
}

void RMPCWidget::setup()
{
	mSpaceListenerMoving = mServices->spaceProvider()->createListener();
	mSpaceListenerFixed = mServices->spaceProvider()->createListener();
	mSpaceListenerMoving->setSpace(mServices->spaceProvider()->getPr());
	connect(mSpaceListenerMoving.get(), &SpaceListener::changed, this, &RMPCWidget::onSpacesChanged);
	connect(mSpaceListenerFixed.get(), &SpaceListener::changed, this, &RMPCWidget::onSpacesChanged);

	mFixedImage.reset(new StringPropertyRegistrationFixedImage(mServices->registration(), mServices->patient()));
	mMovingImage.reset(new StringPropertyRegistrationMovingImage(mServices->registration(), mServices->patient()));

	connect(mServices->registration().get(), &RegistrationService::fixedDataChanged,
			this, &RMPCWidget::inputChanged);
	connect(mServices->registration().get(), &RegistrationService::movingDataChanged,
			this, &RMPCWidget::inputChanged);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->addWidget(new LabeledComboBoxWidget(this, mFixedImage));
	layout->addWidget(new LabeledComboBoxWidget(this, mMovingImage));

	layout->addWidget(mICPWidget);
	layout->addStretch();

	this->inputChanged();
	this->onSettingsChanged();
}


void RMPCWidget::initializeRegistrator()
{
	DataPtr fixed = mServices->registration()->getFixedData();
	DataPtr moving = mServices->registration()->getMovingData();
	QString logPath = mServices->patient()->getActivePatientFolder() + "/Logs/";

	mRegistrator->initialize(moving, fixed, logPath);
}

void RMPCWidget::inputChanged()
{
	if (mObscuredListener->isObscured())
		return;

	DataPtr fixed = mServices->registration()->getFixedData();
	mSpaceListenerFixed->setSpace(mServices->spaceProvider()->getD(fixed));

	this->onSpacesChanged();
}

void RMPCWidget::applyRegistration(Transform3D delta)
{
	Transform3D rMpr = mServices->patient()->get_rMpr();
	Transform3D new_rMpr = delta*rMpr;//output
	mServices->registration()->addPatientRegistration(new_rMpr, "I2P Surface to Surface");

	mServices->registration()->addImage2ImageRegistration(delta, "I2P Surface to Surface - correction");

}

void RMPCWidget::onShown()
{
	this->inputChanged();
}


} //namespace cx
