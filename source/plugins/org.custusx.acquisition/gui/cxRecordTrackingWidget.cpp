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
#include "cxRecordTrackingWidget.h"

#include "cxRecordSessionWidget.h"
#include "cxRecordSession.h"
#include "cxToolRep3D.h"
#include "cxToolTracer.h"
#include "cxLogger.h"
#include "cxPatientModelService.h"
#include "cxViewService.h"
#include "cxStringProperty.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxTrackingService.h"
#include "cxProfile.h"
#include "cxHelperWidgets.h"
#include "cxRepContainer.h"
#include "cxWidgetObscuredListener.h"
#include "cxStringPropertySelectTool.h"
#include "cxAcquisitionService.h"
#include "cxSelectRecordSessionWidget.h"


namespace cx
{

RecordTrackingWidget::RecordTrackingWidget(XmlOptionFile options,
										   AcquisitionServicePtr acquisitionService,
										   VisServices services,
										   QString category,
										   QWidget* parent) :
	QWidget(parent),
	mServices(services),
	mOptions(options),
	mAcquisitionService(acquisitionService)
{
	QVBoxLayout* mVerticalLayout = new QVBoxLayout(this);

	mToolSelector = StringPropertySelectTool::New(services.getToolManager());

	mSelectRecordSession = new SelectRecordSession(mOptions, acquisitionService, services);

	AcquisitionService::TYPES context(AcquisitionService::tTRACKING);
	mRecordSessionWidget = new RecordSessionWidget(mAcquisitionService, this, context, category);

	mVerticalLayout->setMargin(0);

	mVerticalLayout->addWidget(sscCreateDataWidget(this, mToolSelector));
	mVerticalLayout->addWidget(mRecordSessionWidget);
	mVerticalLayout->addWidget(new LabeledComboBoxWidget(this, mSelectRecordSession->getSessionSelector()));

	mObscuredListener.reset(new WidgetObscuredListener(this));
	connect(mObscuredListener.get(), SIGNAL(obscured(bool)), this, SLOT(obscuredSlot(bool)));
}

StringPropertyPtr RecordTrackingWidget::getSessionSelector()
{
	return mSelectRecordSession->getSessionSelector();
}

void RecordTrackingWidget::acquisitionStarted()
{
//	mRecordingTool = this->getSuitableRecordingTool();

	ToolRep3DPtr activeRep3D = this->getToolRepIn3DView();
	if (activeRep3D)
	{
		activeRep3D->getTracer()->clear();
		activeRep3D->getTracer()->setColor(QColor("magenta"));
		activeRep3D->getTracer()->start();
	}
}

void RecordTrackingWidget::acquisitionStopped()
{
	QString newUid = mAcquisitionService->getLatestSession()->getUid();
	mSelectRecordSession->getSessionSelector()->setValue(newUid);

	mServices.patientModelService->autoSave();

	ToolRep3DPtr activeRep3D = this->getToolRepIn3DView();
	if (activeRep3D)
	{
		activeRep3D->getTracer()->stop();
	}

//	this->saveSessions();
}

void RecordTrackingWidget::acquisitionCancelled()
{
	ToolRep3DPtr activeRep3D = this->getToolRepIn3DView();
	if (activeRep3D)
	{
		activeRep3D->getTracer()->stop();
		activeRep3D->getTracer()->clear();
	}
}

ToolRep3DPtr RecordTrackingWidget::getToolRepIn3DView()
{
	RepContainerPtr repContainer = mServices.visualizationService->get3DReps(0, 0);
	ToolPtr tool = this->getSuitableRecordingTool();
	if (repContainer)
		return repContainer->findFirst<ToolRep3D>(tool);
	else
		return ToolRep3DPtr();
}

void RecordTrackingWidget::obscuredSlot(bool obscured)
{
	if (obscured)
		mAcquisitionService->cancelRecord();

	if (!obscured)
	{
		connect(mAcquisitionService.get(), &AcquisitionService::started, this, &RecordTrackingWidget::acquisitionStarted);
		connect(mAcquisitionService.get(), &AcquisitionService::acquisitionStopped, this, &RecordTrackingWidget::acquisitionStopped, Qt::QueuedConnection);
		connect(mAcquisitionService.get(), &AcquisitionService::cancelled, this, &RecordTrackingWidget::acquisitionCancelled);
		connect(mToolSelector.get(), &StringPropertySelectTool::changed, this, &RecordTrackingWidget::onToolChanged);
		mSelectRecordSession->setTool(this->getSuitableRecordingTool());
	}
	else
	{
		disconnect(mAcquisitionService.get(), &AcquisitionService::started, this, &RecordTrackingWidget::acquisitionStarted);
		disconnect(mAcquisitionService.get(), &AcquisitionService::acquisitionStopped, this, &RecordTrackingWidget::acquisitionStopped);
		disconnect(mAcquisitionService.get(), &AcquisitionService::cancelled, this, &RecordTrackingWidget::acquisitionCancelled);
		disconnect(mToolSelector.get(), &StringPropertySelectTool::changed, this, &RecordTrackingWidget::onToolChanged);
		mSelectRecordSession->setTool(ToolPtr());
	}
}

void RecordTrackingWidget::onToolChanged()
{
	mSelectRecordSession->setTool(this->getSuitableRecordingTool());
}


ToolPtr RecordTrackingWidget::getSuitableRecordingTool()
{
	ToolPtr retval = mToolSelector->getTool();
	if(!retval)
		retval = mServices.trackingService->getActiveTool();
	return retval;
}

TimedTransformMap RecordTrackingWidget::getRecordedTrackerData_prMt()
{
	return mSelectRecordSession->getRecordedTrackerData_prMt();
}


} //namespace cx
