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
#include "cxReporter.h"
#include "cxTypeConversions.h"
#include "cxRegistrationManager.h"
#include "cxToolManager.h"
#include "cxDataManager.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxLandmarkRep.h"
#include "cxView.h"
#include "cxRegistrationService.h"
#include "cxVisualizationService.h"

//TODO: remove
#include "cxLogicManager.h"
#include "cxRepManager.h"

namespace cx
{
LandmarkPatientRegistrationWidget::LandmarkPatientRegistrationWidget(regServices services,
	QWidget* parent, QString objectName, QString windowTitle) :
	LandmarkRegistrationWidget(services, parent, objectName, windowTitle), mToolSampleButton(new QPushButton(
		"Sample Tool", this))
{
	mImageLandmarkSource = ImageLandmarksSource::New();
	mFixedDataAdapter.reset(new RegistrationFixedImageStringDataAdapter(services.registrationService, services.patientModelService));
	connect(services.registrationService.get(), &RegistrationService::fixedDataChanged,
			this, &LandmarkPatientRegistrationWidget::fixedDataChanged);
	connect(dataManager(), SIGNAL(rMprChanged()), this, SLOT(setModified()));

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
	mDominantToolProxy = DominantToolProxy::New(trackingService());
	connect(mDominantToolProxy.get(), SIGNAL(toolVisible(bool)), this, SLOT(updateToolSampleButton()));
	connect(mDominantToolProxy.get(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(updateToolSampleButton()));
	connect(dataManager(), SIGNAL(debugModeChanged(bool)), this, SLOT(updateToolSampleButton()));

	//layout
	mVerticalLayout->addWidget(new LabeledComboBoxWidget(this, mFixedDataAdapter));
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
	ToolPtr tool = toolManager()->getDominantTool();

	bool enabled = false;
	enabled = tool && tool->getVisible() && (!tool->hasType(Tool::TOOL_MANUAL) || dataManager()->getDebugMode()); // enable only for non-manual tools. ignore this in debug mode.
	mToolSampleButton->setEnabled(enabled);

	if (toolManager()->getDominantTool())
		mToolSampleButton->setText("Sample " + qstring_cast(tool->getName()));
	else
		mToolSampleButton->setText("No tool");
}

void LandmarkPatientRegistrationWidget::toolSampleButtonClickedSlot()
{
	ToolPtr tool = toolManager()->getDominantTool();

	if (!tool)
	{
		reportError("mToolToSample is NULL!");
		return;
	}
	//TODO What if the reference frame isnt visible?
	Transform3D lastTransform_prMt = tool->get_prMt();
	Vector3D p_pr = lastTransform_prMt.coord(Vector3D(0, 0, tool->getTooltipOffset()));

	// TODO: do we want to allow sampling points not defined in image??
	if (mActiveLandmark.isEmpty() && !dataManager()->getLandmarkProperties().empty())
		mActiveLandmark = dataManager()->getLandmarkProperties().begin()->first;

	dataManager()->getPatientLandmarks()->setLandmark(Landmark(mActiveLandmark, p_pr));
	reporter()->playSampleSound();

    this->activateLandmark(this->getNextLandmark());

	this->performRegistration(); // automatic when sampling in physical patient space (Mantis #0000674)s
}

void LandmarkPatientRegistrationWidget::showEvent(QShowEvent* event)
{
//	std::cout << "LandmarkPatientRegistrationWidget::showEvent" << std::endl;
	LandmarkRegistrationWidget::showEvent(event);
	connect(dataManager()->getPatientLandmarks().get(), SIGNAL(landmarkAdded(QString)), this, SLOT(landmarkUpdatedSlot()));
	connect(dataManager()->getPatientLandmarks().get(), SIGNAL(landmarkRemoved(QString)), this, SLOT(landmarkUpdatedSlot()));

	mServices.visualizationService->setRegistrationMode(rsPATIENT_REGISTRATED);

	LandmarkRepPtr rep = RepManager::findFirstRep<LandmarkRep>(mServices.visualizationService->get3DView(0, 0)->getReps());
	if (rep)
	{
		rep->setPrimarySource(mImageLandmarkSource);
		rep->setSecondarySource(PatientLandmarksSource::New(dataService()));
		rep->setSecondaryColor(QColor::fromRgbF(0, 0.6, 0.8));
	}
}

void LandmarkPatientRegistrationWidget::hideEvent(QHideEvent* event)
{
//	std::cout << "LandmarkPatientRegistrationWidget::hideEvent" << std::endl;
	LandmarkRegistrationWidget::hideEvent(event);
	disconnect(dataManager()->getPatientLandmarks().get(), SIGNAL(landmarkAdded(QString)), this, SLOT(landmarkUpdatedSlot()));
	disconnect(dataManager()->getPatientLandmarks().get(), SIGNAL(landmarkRemoved(QString)), this, SLOT(landmarkUpdatedSlot()));

	if(mServices.visualizationService->get3DView(0, 0))
	{
		LandmarkRepPtr rep = RepManager::findFirstRep<LandmarkRep>(mServices.visualizationService->get3DView(0, 0)->getReps());
		if (rep)
		{
			rep->setPrimarySource(LandmarksSourcePtr());
			rep->setSecondarySource(LandmarksSourcePtr());
		}
	}
	mServices.visualizationService->setRegistrationMode(rsNOT_REGISTRATED);
}

void LandmarkPatientRegistrationWidget::removeLandmarkButtonClickedSlot()
{
    QString next = this->getNextLandmark();
	dataManager()->getPatientLandmarks()->removeLandmark(mActiveLandmark);
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
	return dataManager()->getPatientLandmarks()->getLandmarks();
}

/** Return transform from target space to reference space
 *
 */
Transform3D LandmarkPatientRegistrationWidget::getTargetTransform() const
{
	Transform3D rMpr = dataManager()->get_rMpr();
	return rMpr;
}

void LandmarkPatientRegistrationWidget::setTargetLandmark(QString uid, Vector3D p_target)
{
	dataManager()->getPatientLandmarks()->setLandmark(Landmark(uid, p_target));
	reporter()->playSampleSound();
}

void LandmarkPatientRegistrationWidget::performRegistration()
{
	if (!mServices.registrationService->getFixedData())
		mServices.registrationService->setFixedData(dataManager()->getActiveImage());

	if (dataManager()->getPatientLandmarks()->getLandmarks().size() < 3)
		return;

	mServices.registrationService->doPatientRegistration();

	this->updateAvarageAccuracyLabel();
}

QString LandmarkPatientRegistrationWidget::getTargetName() const
{
	return "Patient";
}

}//namespace cx
