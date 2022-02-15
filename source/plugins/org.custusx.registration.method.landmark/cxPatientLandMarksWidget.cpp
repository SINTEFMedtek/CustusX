/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxPatientLandMarksWidget.h"

#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QCheckBox>

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
#include "cxImage.h"
#include "cxActiveData.h"

namespace cx
{

PatientLandMarksWidget::PatientLandMarksWidget(RegServicesPtr services,
	QWidget* parent, QString objectName, QString windowTitle) :
	LandmarkRegistrationWidget(services, parent, objectName, windowTitle),
	mToolSampleButton(new QPushButton("Sample Tool", this))
{
	mLandmarkListener->useI2IRegistration(false);

	connect(services->patient().get(), &PatientModelService::rMprChanged, this, &PatientLandMarksWidget::setModified);

	//buttons
	mToolSampleButton->setDisabled(true);
	connect(mToolSampleButton, SIGNAL(clicked()), this, SLOT(toolSampleButtonClickedSlot()));

	mRemoveLandmarkButton = new QPushButton("Clear", this);
	mRemoveLandmarkButton->setToolTip("Clear selected landmark");
	connect(mRemoveLandmarkButton, &QPushButton::clicked, this, &PatientLandMarksWidget::removeLandmarkButtonClickedSlot);

	//toolmanager
	mActiveToolProxy = ActiveToolProxy::New(services->tracking());
	connect(mActiveToolProxy.get(), SIGNAL(toolVisible(bool)), this, SLOT(updateToolSampleButton()));
	connect(mActiveToolProxy.get(), SIGNAL(activeToolChanged(const QString&)), this, SLOT(updateToolSampleButton()));

	connect(settings(), &Settings::valueChangedFor, this, &PatientLandMarksWidget::globalConfigurationFileChangedSlot);

	//layout
	mVerticalLayout->addWidget(mLandmarkTableWidget);
	mVerticalLayout->addWidget(mToolSampleButton);
	mVerticalLayout->addWidget(mAvarageAccuracyLabel);
	mVerticalLayout->addWidget(mRemoveLandmarkButton);

	mMouseClickSample->setText("Sample with mouse clicks in anyplane view.");
	mMouseClickSample->show();
	connect(mMouseClickSample, &QCheckBox::stateChanged, this, &PatientLandMarksWidget::mouseClickSampleStateChanged);

	mVerticalLayout->addWidget(mMouseClickSample);

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
	ToolPtr tool = mServices->tracking()->getActiveTool();

	bool enabled = tool && tool->getVisible() && (!tool->hasType(Tool::TOOL_MANUAL) || settings()->value("giveManualToolPhysicalProperties").toBool()); // enable only for non-manual tools.
	mToolSampleButton->setEnabled(enabled);

	if (mServices->tracking()->getActiveTool())
		mToolSampleButton->setText("Sample " + qstring_cast(tool->getName()));
	else
		mToolSampleButton->setText("No tool");
}

void PatientLandMarksWidget::toolSampleButtonClickedSlot()
{
	ToolPtr tool = mServices->tracking()->getActiveTool();

	if (!tool)
	{
		reportError("mToolToSample is NULL!");
		return;
	}
	//TODO What if the reference frame isnt visible?
	Transform3D lastTransform_prMt = tool->get_prMt();
	Vector3D p_pr = lastTransform_prMt.coord(Vector3D(0, 0, tool->getTooltipOffset()));

	// TODO: do we want to allow sampling points not defined in image??
	if (mActiveLandmark.isEmpty() && !mServices->patient()->getLandmarkProperties().empty())
		mActiveLandmark = mServices->patient()->getLandmarkProperties().begin()->first;

	mServices->patient()->getPatientLandmarks()->setLandmark(Landmark(mActiveLandmark, p_pr));
	reporter()->playSampleSound();

	this->activateLandmark(this->getNextLandmark());

	this->performRegistration(); // automatic when sampling in physical patient space (Mantis #0000674)s
}

void PatientLandMarksWidget::showEvent(QShowEvent* event)
{
	mServices->view()->setRegistrationMode(rsPATIENT_REGISTRATED);
	LandmarkRegistrationWidget::showEvent(event);
	mMouseClickSample->setChecked(true);
}

void PatientLandMarksWidget::hideEvent(QHideEvent* event)
{
	mServices->view()->setRegistrationMode(rsNOT_REGISTRATED);
	LandmarkRegistrationWidget::hideEvent(event);
}

void PatientLandMarksWidget::removeLandmarkButtonClickedSlot()
{
	QString next = this->getNextLandmark();
	mServices->patient()->getPatientLandmarks()->removeLandmark(mActiveLandmark);
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
	return mServices->patient()->getPatientLandmarks()->getLandmarks();
}

/** Return transform from target space to reference space
 *
 */
Transform3D PatientLandMarksWidget::getTargetTransform() const
{
	Transform3D rMpr = mServices->patient()->get_rMpr();
	return rMpr;
}

void PatientLandMarksWidget::setTargetLandmark(QString uid, Vector3D p_target)
{
	mServices->patient()->getPatientLandmarks()->setLandmark(Landmark(uid, p_target));
	reporter()->playSampleSound();
}

void PatientLandMarksWidget::performRegistration()
{
	ActiveDataPtr activeData = mServices->patient()->getActiveData();
	if (!mServices->registration()->getFixedData())
		mServices->registration()->setFixedData(activeData->getActive<Image>());

	if (mServices->patient()->getPatientLandmarks()->getLandmarks().size() < 3)
		return;

	mServices->registration()->doPatientRegistration();

	this->updateAverageAccuracyLabel();
}

QString PatientLandMarksWidget::getTargetName() const
{
	return "Patient";
}

void PatientLandMarksWidget::pointSampled(Vector3D p_r)
{
	QTableWidgetItem* item = getLandmarkTableItem();
	if(!item)
	{
		CX_LOG_WARNING() << "PatientLandMarksWidget::pointSampled() Cannot get item from mLandmarkTableWidget";
		return;
	}
	QString uid = item->data(Qt::UserRole).toString();

	Transform3D rMtarget = this->getTargetTransform();
	Vector3D p_target = rMtarget.inv().coord(p_r);

	this->setTargetLandmark(uid, p_target);
	this->activateLandmark(this->getNextLandmark());
	this->performRegistration();
}
} //cx
