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
	mServices->registration()->applyImage2ImageRegistration(delta, "Vessel based");
}

void SeansVesselRegistrationWidget::onShown()
{
	this->inputChanged();
}





}//namespace cx
