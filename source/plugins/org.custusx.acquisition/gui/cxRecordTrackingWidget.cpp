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


namespace cx
{
RecordTrackingWidget::RecordTrackingWidget(XmlOptionFile options, AcquisitionServicePtr acquisitionService, VisServices services, QWidget* parent) :
	QWidget(parent),
	mServices(services),
	mOptions(options),
  mAcquisitionService(acquisitionService)
{
	QVBoxLayout* mVerticalLayout = new QVBoxLayout(this);
//	mOptions = profile()->getXmlSettings().descend("RecordTrackingWidget");

	mToolSelector = StringPropertySelectTool::New(services.getToolManager());
	//this->initializeTrackingService();

	this->initSessionSelector();

	AcquisitionService::TYPES context(AcquisitionService::tTRACKING);
	mRecordSessionWidget.reset(new RecordSessionWidget(mAcquisitionService, this, context, "Surface tracker data"));
	mRecordSessionWidget->setDescriptionVisibility(false);

	mVerticalLayout->setMargin(0);

	mVerticalLayout->addWidget(sscCreateDataWidget(this, mToolSelector));
	mVerticalLayout->addWidget(mRecordSessionWidget.get());
	mVerticalLayout->addWidget(new LabeledComboBoxWidget(this, mSessionSelector));

	mObscuredListener.reset(new WidgetObscuredListener(this));
	connect(mObscuredListener.get(), SIGNAL(obscured(bool)), this, SLOT(obscuredSlot(bool)));
}

void RecordTrackingWidget::initSessionSelector()
{
	QStringList sessionUids = getSessionList();
	QString defaultValue;
	if(!sessionUids.isEmpty())
		defaultValue = sessionUids.last();
	mSessionSelector = StringProperty::initialize("tracking_session", "Tracked Surface", "Select tracker data for registration to surface", defaultValue, sessionUids, QDomNode());

	//TODO: Let mSessionSelector display displaynames instead of uids (StringDataAdapterXml::setDisplayNames)
}

QStringList RecordTrackingWidget::getSessionList()
{
	std::vector<RecordSessionPtr> sessions = mAcquisitionService->getSessions();
	std::vector<RecordSessionPtr>::iterator it = sessions.begin();
	QStringList sessionUids;

	for(; it != sessions.end(); ++it)
	{
		QString uid = (*it)->getUid();
		sessionUids << uid;
	}

	sessionUids.sort();
	return sessionUids;
}

//void RecordTrackingWidget::initializeTrackingService()
//{
//	if(mServices.trackingService->getState() < Tool::tsCONFIGURED)
//		mServices.trackingService->setState(Tool::tsCONFIGURED);
//}

void RecordTrackingWidget::acquisitionStarted()
{
//	std::cout << "acquisitionStarted" << std::endl;

	mRecordingTool = this->getSuitableRecordingTool();

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
//	std::cout << "acquisitionStopped" << std::endl;

	QString newUid = mAcquisitionService->getLatestSession()->getUid();
	QStringList range = mSessionSelector->getValueRange();
	range << newUid;
	mSessionSelector->setValueRange(range);
	mSessionSelector->setValue(newUid);

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

void RecordTrackingWidget::recordedSessionsChanged()
{
	QStringList sessionUids = getSessionList();
	mSessionSelector->setValueRange(sessionUids);
	if(mSessionSelector->getValue().isEmpty() && !sessionUids.isEmpty())
		mSessionSelector->setValue(sessionUids.last());
}

ToolRep3DPtr RecordTrackingWidget::getToolRepIn3DView()
{
	RepContainerPtr repContainer = mServices.visualizationService->get3DReps(0, 0);
	if (repContainer)
		return repContainer->findFirst<ToolRep3D>(mRecordingTool);
	else
		return ToolRep3DPtr();
}

void RecordTrackingWidget::obscuredSlot(bool obscured)
{
	if (obscured)
		mAcquisitionService->cancelRecord();
//	std::cout << "obscuredSlot: " << obscured << std::endl;

	if (!obscured)
	{
		connect(mAcquisitionService.get(), &AcquisitionService::started, this, &RecordTrackingWidget::acquisitionStarted);
		connect(mAcquisitionService.get(), &AcquisitionService::acquisitionStopped, this, &RecordTrackingWidget::acquisitionStopped, Qt::QueuedConnection);
		connect(mAcquisitionService.get(), &AcquisitionService::cancelled, this, &RecordTrackingWidget::acquisitionCancelled);
		connect(mAcquisitionService.get(), &AcquisitionService::recordedSessionsChanged, this, &RecordTrackingWidget::recordedSessionsChanged);
	}
	else
	{
		disconnect(mAcquisitionService.get(), &AcquisitionService::started, this, &RecordTrackingWidget::acquisitionStarted);
		disconnect(mAcquisitionService.get(), &AcquisitionService::acquisitionStopped, this, &RecordTrackingWidget::acquisitionStopped);
		disconnect(mAcquisitionService.get(), &AcquisitionService::cancelled, this, &RecordTrackingWidget::acquisitionCancelled);
		disconnect(mAcquisitionService.get(), &AcquisitionService::recordedSessionsChanged, this, &RecordTrackingWidget::recordedSessionsChanged);
	}

//	ToolRep3DPtr activeRep3D = this->getToolRepIn3DView();
//	if (activeRep3D)
//	{
//		//std::cout << "Slot is cleared" << std::endl;
//		activeRep3D->getTracer()->clear();
//	}
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
	ToolPtr tool = this->getSuitableRecordingTool();

	if(!tool)
	{
		reportError("Found no tool in tracker recorder");
		return TimedTransformMap();
	}
	std::cout << "RecordTrackingWidget::getRecordedTrackerData_prMt(), Tool name: " << tool->getName() << std::endl;

	RecordSessionPtr session;
	QString sessionUid = mSessionSelector->getValue();
	if(!sessionUid.isEmpty())
		session = mAcquisitionService->getSession(sessionUid);
	if(!session)
		reportError("No session");

	TimedTransformMap trackerRecordedData_prMt = RecordSession::getToolHistory_prMt(tool, session);//input

	return trackerRecordedData_prMt;
}

void RecordTrackingWidget::ShowLastRecordingInView()
{
	TimedTransformMap trackerRecordedData_prMt = this->getRecordedTrackerData_prMt();

	ToolRep3DPtr activeRep3D = this->getToolRepIn3DView();
	if(activeRep3D)
	{
		activeRep3D->getTracer()->clear();
		activeRep3D->getTracer()->setColor(QColor("green"));
		activeRep3D->getTracer()->addManyPositions(trackerRecordedData_prMt);
	}
}

} //namespace cx
