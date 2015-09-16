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

namespace cx
{

SeansVesselRegistrationWidget::SeansVesselRegistrationWidget(RegServices services, QWidget* parent) :
	RegistrationBaseWidget(services, parent,
						   "org_custusx_registration_method_vessel_seans_widget",
						   "Seans Vessel Registration")
{
	mRegistrator.reset(new SeansVesselReg());

	mSpaceListenerMoving = mServices.spaceProvider->createListener();
	mSpaceListenerFixed = mServices.spaceProvider->createListener();
	connect(mSpaceListenerMoving.get(), &SpaceListener::changed, this, &SeansVesselRegistrationWidget::onSpacesChanged);
	connect(mSpaceListenerFixed.get(), &SpaceListener::changed, this, &SeansVesselRegistrationWidget::onSpacesChanged);

	connect(mServices.registrationService.get(), &RegistrationService::fixedDataChanged,
			this, &SeansVesselRegistrationWidget::inputChanged);
	connect(mServices.registrationService.get(), &RegistrationService::movingDataChanged,
			this, &SeansVesselRegistrationWidget::inputChanged);

	mLTSRatio = DoubleProperty::initialize("LTSRatio", "LTS Ratio",
										   "Fraction of points in the lesser point set to use during each iteration.",
										   80, DoubleRange(20,100,1), 0);
	connect(mLTSRatio.get(), &DoubleProperty::changed, this, &SeansVesselRegistrationWidget::onSettingsChanged);

	mAutoLTS = BoolProperty::initialize("autoLTS","Auto LTS",
										"Ignore LTS, instead attempt to find optimal value",
									   false);
	connect(mAutoLTS.get(), &DoubleProperty::changed, this, &SeansVesselRegistrationWidget::onSettingsChanged);

	mLinear = BoolProperty::initialize("linear","Linear",
									   "Use only linear iteration",
									   true);
	connect(mLinear.get(), &DoubleProperty::changed, this, &SeansVesselRegistrationWidget::onSettingsChanged);

	mDisplayProgress = BoolProperty::initialize("progress","Display Progress",
												"Display metric and difference lines between point sets",
									   true);
	connect(mDisplayProgress.get(), &DoubleProperty::changed, this, &SeansVesselRegistrationWidget::onDisplayProgressChanged);

	mOneStep = BoolProperty::initialize("onestep","One Step",
										"Registration is done one iteration at a time.",
									   false);
	connect(mOneStep.get(), &DoubleProperty::changed, this, &SeansVesselRegistrationWidget::onSettingsChanged);

	std::vector<PropertyPtr> properties;
	properties.push_back(mLTSRatio);
	properties.push_back(mAutoLTS);
	properties.push_back(mLinear);
	properties.push_back(mDisplayProgress);
	properties.push_back(mOneStep);
	mICPWidget = new ICPWidget(this);
	mICPWidget->setSettings(properties);
	connect(mICPWidget, &ICPWidget::requestRegister, this, &SeansVesselRegistrationWidget::registerSlot);

	QGridLayout* layout = new QGridLayout(this);
	QGridLayout* entryLayout = new QGridLayout;
	entryLayout->setColumnStretch(1, 1);

	mFixedImage.reset(new StringPropertyRegistrationFixedImage(services.registrationService, services.patientModelService));
	new LabeledComboBoxWidget(this, mFixedImage, entryLayout, 0);
	mMovingImage.reset(new StringPropertyRegistrationMovingImage(services.registrationService, services.patientModelService));
	new LabeledComboBoxWidget(this, mMovingImage, entryLayout, 1);

	layout->addLayout(entryLayout, 0, 0, 2, 2);
	layout->addWidget(mICPWidget, 2, 0, 2, 2);

	mObscuredListener.reset(new WidgetObscuredListener(this));
	connect(mObscuredListener.get(), SIGNAL(obscured(bool)), this, SLOT(obscuredSlot(bool)));

	this->inputChanged();
	this->onSettingsChanged();
}

SeansVesselRegistrationWidget::~SeansVesselRegistrationWidget()
{
}

void SeansVesselRegistrationWidget::obscuredSlot(bool obscured)
{
	if (obscured)
	{
		mMeshInView.reset();
	}
	else
	{
		this->onSettingsChanged();
		this->inputChanged();
	}
}

void SeansVesselRegistrationWidget::onSpacesChanged()
{
	if (mObscuredListener->isObscured())
		return;

	DataPtr moving = mServices.registrationService->getMovingData();
	DataPtr fixed = mServices.registrationService->getFixedData();
	QString logPath = mServices.patientModelService->getActivePatientFolder() + "/Logs/";

	mRegistrator->initialize(moving, fixed, logPath);

	mICPWidget->enableRegistration(mRegistrator->isValid());
	this->updateDifferenceLines();
	mICPWidget->setRMS(mRegistrator->getResultMetric());
}

void SeansVesselRegistrationWidget::inputChanged()
{
	if (mObscuredListener->isObscured())
		return;

	DataPtr moving = mServices.registrationService->getMovingData();
	DataPtr fixed = mServices.registrationService->getFixedData();

	mSpaceListenerFixed->setSpace(mServices.spaceProvider->getD(fixed));
	mSpaceListenerMoving->setSpace(mServices.spaceProvider->getD(moving));

	this->onSpacesChanged();
}

void SeansVesselRegistrationWidget::onSettingsChanged()
{
	if (mObscuredListener->isObscured())
		return;
	mRegistrator->mt_auto_lts = mAutoLTS->getValue();
	mRegistrator->mt_ltsRatio = mLTSRatio->getValue();
	mRegistrator->mt_doOnlyLinear = mLinear->getValue();
}

void SeansVesselRegistrationWidget::registerSlot()
{
//	int lts_ratio = mLTSRatioSpinBox->value();
//	double stop_delta = 0.001; //TODO, add user interface
//	double lambda = 0; //TODO, add user interface
//	double sigma = 1.0; //TODO, add user interface
//	bool lin_flag = mLinearCheckBox->isChecked(); //TODO, add user interface
//	int sample = 1; //TODO, add user interface
//	int single_point_thre = 1; //TODO, add user interface
//	bool verbose = 1; //TODO, add user interface

	mRegistrator->notifyPreRegistrationWarnings();

	if (mRegistrator->mt_auto_lts)
	{
		reportDebug("Using automatic lts_ratio");
	}
	else
	{
		reportDebug("Using lts_ratio: " + qstring_cast(mRegistrator->mt_ltsRatio));
	}

	bool success = false;
	if (mOneStep->getValue())
		success = mRegistrator->performOneRegistration();
	else
		success = mRegistrator->execute();

	if (!success)
	{
		reportWarning("Vessel registration failed.");
		return;
	}

	Transform3D linearTransform = mRegistrator->getLinearResult();
	std::cout << "v2v linear result:\n" << linearTransform << std::endl;
	//std::cout << "v2v inverted linear result:\n" << linearTransform.inverse() << std::endl;

	mRegistrator->checkQuality(linearTransform);

	// The registration is performed in space r. Thus, given an old data position rMd, we find the
	// new one as rM'd = Q * rMd, where Q is the inverted registration output.
	// Delta is thus equal to Q:
	Transform3D delta = linearTransform.inv();
	//std::cout << "delta:\n" << delta << std::endl;
	mServices.registrationService->applyImage2ImageRegistration(delta, "Vessel based");
}


void SeansVesselRegistrationWidget::updateDifferenceLines()
{
	if (!mMeshInView)
		mMeshInView.reset(new MeshInView(mServices.visualizationService));

	bool show = mDisplayProgress->getValue() && mRegistrator->isValid();
	if (show)
		mMeshInView->show(mRegistrator->getDifferenceLines());
	else
		mMeshInView->hide();
}

void SeansVesselRegistrationWidget::onDisplayProgressChanged()
{
	this->updateDifferenceLines();
}


}//namespace cx
