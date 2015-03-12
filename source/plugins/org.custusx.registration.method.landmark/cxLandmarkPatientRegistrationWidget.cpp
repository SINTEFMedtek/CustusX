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

#include "cxLandmarkPatientRegistrationWidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLabel>
#include <QSlider>
#include <QGridLayout>
#include <QSpinBox>
#include <vtkDoubleArray.h>
#include "cxVector3D.h"
#include "cxLogger.h"
#include "cxTypeConversions.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxLandmarkRep.h"
#include "cxView.h"
#include "cxRegistrationService.h"
#include "cxViewService.h"
#include "cxPatientModelService.h"
#include "cxViewGroupData.h"
#include "cxTrackingService.h"
#include "cxRepContainer.h"
#include "cxReporter.h"
#include "cxSettings.h"

namespace cx
{
LandmarkPatientRegistrationWidget::LandmarkPatientRegistrationWidget(RegServices services,
	QWidget* parent, QString objectName, QString windowTitle) :
	LandmarkRegistrationWidget(services, parent, objectName, windowTitle), mToolSampleButton(new QPushButton(
		"Sample Tool", this))
{
	mImageLandmarkSource = ImageLandmarksSource::New();
	mFixedProperty.reset(new StringPropertyRegistrationFixedImage(services.registrationService, services.patientModelService));
	connect(services.registrationService.get(), &RegistrationService::fixedDataChanged,
			this, &LandmarkPatientRegistrationWidget::fixedDataChanged);
	connect(services.patientModelService.get(), &PatientModelService::rMprChanged, this, &LandmarkPatientRegistrationWidget::setModified);

	//buttons
	mToolSampleButton->setDisabled(true);
	connect(mToolSampleButton, SIGNAL(clicked()), this, SLOT(toolSampleButtonClickedSlot()));

	mRemoveLandmarkButton = new QPushButton("Clear", this);
	mRemoveLandmarkButton->setToolTip("Clear selected landmark");
	//  mRemoveLandmarkButton->setDisabled(true);
	connect(mRemoveLandmarkButton, SIGNAL(clicked()), this, SLOT(removeLandmarkButtonClickedSlot()));

	mRegisterButton = new QPushButton("Register", this);
	mRegisterButton->setToolTip("Perform registration");
	connect(mRegisterButton, SIGNAL(clicked()), this, SLOT(registerSlot()));

	//toolmanager
	mActiveToolProxy = ActiveToolProxy::New(services.trackingService);
	connect(mActiveToolProxy.get(), SIGNAL(toolVisible(bool)), this, SLOT(updateToolSampleButton()));
	connect(mActiveToolProxy.get(), SIGNAL(activeToolChanged(const QString&)), this, SLOT(updateToolSampleButton()));

	connect(settings(), &Settings::valueChangedFor, this, &LandmarkPatientRegistrationWidget::globalConfigurationFileChangedSlot);

	//layout
	mVerticalLayout->addWidget(new LabeledComboBoxWidget(this, mFixedProperty));
	mVerticalLayout->addWidget(mLandmarkTableWidget);
	mVerticalLayout->addWidget(mToolSampleButton);
	mVerticalLayout->addWidget(mAvarageAccuracyLabel);

	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	buttonsLayout->addWidget(mRegisterButton);
	buttonsLayout->addWidget(mRemoveLandmarkButton);
	mVerticalLayout->addLayout(buttonsLayout);

	this->updateToolSampleButton();
}

LandmarkPatientRegistrationWidget::~LandmarkPatientRegistrationWidget()
{
}

QString LandmarkPatientRegistrationWidget::defaultWhatsThis() const
{
	return "<html>"
		"<h3>Landmark based patient registration.</h3>"
		"<p>Sample points on the patient that corresponds to 3 or more landmarks already sampled in the data set. </p>"
		"<p><i>Point on the patient using a tool and click the Sample button.</i></p>"
		"<p>Landmark patient registration will move the patient into the global coordinate system (r).</p>"
		"</html>";
}

void LandmarkPatientRegistrationWidget::globalConfigurationFileChangedSlot(QString key)
{
	if (key == "giveManualToolPhysicalProperties")
		this->updateToolSampleButton();
}

void LandmarkPatientRegistrationWidget::registerSlot()
{
	this->performRegistration();
}

void LandmarkPatientRegistrationWidget::fixedDataChanged()
{
	mImageLandmarkSource->setData(mServices.registrationService->getFixedData());
}

void LandmarkPatientRegistrationWidget::updateToolSampleButton()
{
	ToolPtr tool = mServices.trackingService->getActiveTool();

	bool enabled = false;
	enabled = tool && tool->getVisible() && (!tool->hasType(Tool::TOOL_MANUAL) || settings()->value("giveManualToolPhysicalProperties").toBool()); // enable only for non-manual tools.
	mToolSampleButton->setEnabled(enabled);

	if (mServices.trackingService->getActiveTool())
		mToolSampleButton->setText("Sample " + qstring_cast(tool->getName()));
	else
		mToolSampleButton->setText("No tool");
}

void LandmarkPatientRegistrationWidget::toolSampleButtonClickedSlot()
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

void LandmarkPatientRegistrationWidget::showEvent(QShowEvent* event)
{
//	std::cout << "LandmarkPatientRegistrationWidget::showEvent" << std::endl;
	LandmarkRegistrationWidget::showEvent(event);
	connect(mServices.patientModelService->getPatientLandmarks().get(), SIGNAL(landmarkAdded(QString)), this, SLOT(landmarkUpdatedSlot()));
	connect(mServices.patientModelService->getPatientLandmarks().get(), SIGNAL(landmarkRemoved(QString)), this, SLOT(landmarkUpdatedSlot()));

	mServices.visualizationService->getGroup(0)->setRegistrationMode(rsPATIENT_REGISTRATED);

	LandmarkRepPtr rep = mServices.visualizationService->get3DReps(0, 0)->findFirst<LandmarkRep>();
	if (rep)
	{
		rep->setPrimarySource(mImageLandmarkSource);
		rep->setSecondarySource(PatientLandmarksSource::New(mServices.patientModelService));
		rep->setSecondaryColor(QColor::fromRgbF(0, 0.6, 0.8));
	}
}

void LandmarkPatientRegistrationWidget::hideEvent(QHideEvent* event)
{
//	std::cout << "LandmarkPatientRegistrationWidget::hideEvent" << std::endl;
	LandmarkRegistrationWidget::hideEvent(event);
	disconnect(mServices.patientModelService->getPatientLandmarks().get(), SIGNAL(landmarkAdded(QString)), this, SLOT(landmarkUpdatedSlot()));
	disconnect(mServices.patientModelService->getPatientLandmarks().get(), SIGNAL(landmarkRemoved(QString)), this, SLOT(landmarkUpdatedSlot()));

	if(mServices.visualizationService->get3DView(0, 0))
	{
		LandmarkRepPtr rep = mServices.visualizationService->get3DReps(0, 0)->findFirst<LandmarkRep>();
		if (rep)
		{
			rep->setPrimarySource(LandmarksSourcePtr());
			rep->setSecondarySource(LandmarksSourcePtr());
		}
	}
	mServices.visualizationService->getGroup(0)->setRegistrationMode(rsNOT_REGISTRATED);
}

void LandmarkPatientRegistrationWidget::removeLandmarkButtonClickedSlot()
{
    QString next = this->getNextLandmark();
	mServices.patientModelService->getPatientLandmarks()->removeLandmark(mActiveLandmark);
    this->activateLandmark(next);
}

void LandmarkPatientRegistrationWidget::cellClickedSlot(int row, int column)
{
	LandmarkRegistrationWidget::cellClickedSlot(row, column);

	mRemoveLandmarkButton->setEnabled(true);
}

void LandmarkPatientRegistrationWidget::prePaintEvent()
{
    LandmarkRegistrationWidget::prePaintEvent();

	std::vector<Landmark> landmarks = this->getAllLandmarks();
	mRemoveLandmarkButton->setEnabled(!landmarks.empty() && !mActiveLandmark.isEmpty());
}

/** Return the landmarks associated with the current widget.
 */
LandmarkMap LandmarkPatientRegistrationWidget::getTargetLandmarks() const
{
	return mServices.patientModelService->getPatientLandmarks()->getLandmarks();
}

/** Return transform from target space to reference space
 *
 */
Transform3D LandmarkPatientRegistrationWidget::getTargetTransform() const
{
	Transform3D rMpr = mServices.patientModelService->get_rMpr();
	return rMpr;
}

void LandmarkPatientRegistrationWidget::setTargetLandmark(QString uid, Vector3D p_target)
{
	mServices.patientModelService->getPatientLandmarks()->setLandmark(Landmark(uid, p_target));
	reporter()->playSampleSound();
}

void LandmarkPatientRegistrationWidget::performRegistration()
{
	if (!mServices.registrationService->getFixedData())
		mServices.registrationService->setFixedData(mServices.patientModelService->getActiveImage());

	if (mServices.patientModelService->getPatientLandmarks()->getLandmarks().size() < 3)
		return;

	mServices.registrationService->doPatientRegistration();

	this->updateAvarageAccuracyLabel();
}

QString LandmarkPatientRegistrationWidget::getTargetName() const
{
	return "Patient";
}

}//namespace cx
