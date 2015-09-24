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

#include "cxPatientLandMarksWidget.h"

#include <QPushButton>
#include <QTableWidget>
#include <QLabel>

#include "cxActiveToolProxy.h"
#include "cxLandmarkListener.h"
#include "cxSettings.h"
#include "cxPatientModelService.h"
#include "cxTrackingService.h"
#include "cxRegistrationService.h"
#include "cxViewService.h"
#include "cxTypeConversions.h"
#include "cxReporter.h"
#include "cxLandmark.h"
#include "cxViewGroupData.h"
#include "cxImage.h"
#include "cxActiveData.h"

namespace cx
{

PatientLandMarksWidget::PatientLandMarksWidget(RegServices services,
	QWidget* parent, QString objectName, QString windowTitle) :
	LandmarkRegistrationWidget(services, parent, objectName, windowTitle),
	mToolSampleButton(new QPushButton("Sample Tool", this))
{
	mLandmarkListener->useI2IRegistration(false);

	connect(services.patientModelService.get(), &PatientModelService::rMprChanged, this, &PatientLandMarksWidget::setModified);

	//buttons
	mToolSampleButton->setDisabled(true);
	connect(mToolSampleButton, SIGNAL(clicked()), this, SLOT(toolSampleButtonClickedSlot()));

	mRemoveLandmarkButton = new QPushButton("Clear", this);
	mRemoveLandmarkButton->setToolTip("Clear selected landmark");
	connect(mRemoveLandmarkButton, &QPushButton::clicked, this, &PatientLandMarksWidget::removeLandmarkButtonClickedSlot);

	//toolmanager
	mActiveToolProxy = ActiveToolProxy::New(services.trackingService);
	connect(mActiveToolProxy.get(), SIGNAL(toolVisible(bool)), this, SLOT(updateToolSampleButton()));
	connect(mActiveToolProxy.get(), SIGNAL(activeToolChanged(const QString&)), this, SLOT(updateToolSampleButton()));

	connect(settings(), &Settings::valueChangedFor, this, &PatientLandMarksWidget::globalConfigurationFileChangedSlot);

	//layout
	mVerticalLayout->addWidget(mLandmarkTableWidget);
	mVerticalLayout->addWidget(mToolSampleButton);
	mVerticalLayout->addWidget(mAvarageAccuracyLabel);
	mVerticalLayout->addWidget(mRemoveLandmarkButton);

	this->updateToolSampleButton();
}

PatientLandMarksWidget::~PatientLandMarksWidget()
{
}

void PatientLandMarksWidget::globalConfigurationFileChangedSlot(QString key)
{
	if (key == "giveManualToolPhysicalProperties")
		this->updateToolSampleButton();
}

void PatientLandMarksWidget::updateToolSampleButton()
{
	ToolPtr tool = mServices.trackingService->getActiveTool();

	bool enabled = tool && tool->getVisible() && (!tool->hasType(Tool::TOOL_MANUAL) || settings()->value("giveManualToolPhysicalProperties").toBool()); // enable only for non-manual tools.
	mToolSampleButton->setEnabled(enabled);

	if (mServices.trackingService->getActiveTool())
		mToolSampleButton->setText("Sample " + qstring_cast(tool->getName()));
	else
		mToolSampleButton->setText("No tool");
}

void PatientLandMarksWidget::toolSampleButtonClickedSlot()
{
	ToolPtr tool = mServices.trackingService->getActiveTool();

	if (!tool)
	{
		reportError("mToolToSample is NULL!");
		return;
	}
	//TODO What if the reference frame isnt visible?
	Transform3D lastTransform_prMt = tool->get_prMt();
	Vector3D p_pr = lastTransform_prMt.coord(Vector3D(0, 0, tool->getTooltipOffset()));

	// TODO: do we want to allow sampling points not defined in image??
	if (mActiveLandmark.isEmpty() && !mServices.patientModelService->getLandmarkProperties().empty())
		mActiveLandmark = mServices.patientModelService->getLandmarkProperties().begin()->first;

	mServices.patientModelService->getPatientLandmarks()->setLandmark(Landmark(mActiveLandmark, p_pr));
	reporter()->playSampleSound();

	this->activateLandmark(this->getNextLandmark());

	this->performRegistration(); // automatic when sampling in physical patient space (Mantis #0000674)s
}

void PatientLandMarksWidget::showEvent(QShowEvent* event)
{
//	std::cout << "PatientLandMarksWidget::showEvent" << std::endl;
	mServices.visualizationService->getGroup(0)->setRegistrationMode(rsPATIENT_REGISTRATED);
	LandmarkRegistrationWidget::showEvent(event);
}

void PatientLandMarksWidget::hideEvent(QHideEvent* event)
{
//	std::cout << "PatientLandMarksWidget::hideEvent" << std::endl;
	mServices.visualizationService->getGroup(0)->setRegistrationMode(rsNOT_REGISTRATED);
	LandmarkRegistrationWidget::hideEvent(event);
}

void PatientLandMarksWidget::removeLandmarkButtonClickedSlot()
{
	QString next = this->getNextLandmark();
	mServices.patientModelService->getPatientLandmarks()->removeLandmark(mActiveLandmark);
	this->activateLandmark(next);
}

void PatientLandMarksWidget::cellClickedSlot(int row, int column)
{
	LandmarkRegistrationWidget::cellClickedSlot(row, column);

	mRemoveLandmarkButton->setEnabled(true);
}

void PatientLandMarksWidget::prePaintEvent()
{
	LandmarkRegistrationWidget::prePaintEvent();

	std::vector<Landmark> landmarks = this->getAllLandmarks();
	mRemoveLandmarkButton->setEnabled(!landmarks.empty() && !mActiveLandmark.isEmpty());
}

/** Return the landmarks associated with the current widget.
 */
LandmarkMap PatientLandMarksWidget::getTargetLandmarks() const
{
	return mServices.patientModelService->getPatientLandmarks()->getLandmarks();
}

/** Return transform from target space to reference space
 *
 */
Transform3D PatientLandMarksWidget::getTargetTransform() const
{
	Transform3D rMpr = mServices.patientModelService->get_rMpr();
	return rMpr;
}

void PatientLandMarksWidget::setTargetLandmark(QString uid, Vector3D p_target)
{
	mServices.patientModelService->getPatientLandmarks()->setLandmark(Landmark(uid, p_target));
	reporter()->playSampleSound();
}

void PatientLandMarksWidget::performRegistration()
{
	ActiveDataPtr activeData = mServices.patientModelService->getActiveData();
	if (!mServices.registrationService->getFixedData())
		mServices.registrationService->setFixedData(activeData->getActive<Image>());

	if (mServices.patientModelService->getPatientLandmarks()->getLandmarks().size() < 3)
		return;

	mServices.registrationService->doPatientRegistration();

	this->updateAverageAccuracyLabel();
}

QString PatientLandMarksWidget::getTargetName() const
{
	return "Patient";
}

} //cx
