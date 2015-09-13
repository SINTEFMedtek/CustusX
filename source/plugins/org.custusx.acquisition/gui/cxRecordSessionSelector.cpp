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
#include "cxRecordSessionSelector.h"

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

SelectRecordSession::SelectRecordSession(XmlOptionFile options,
										 AcquisitionServicePtr acquisitionService,
										 VisServices services) :
	mServices(services),
	mOptions(options),
	mAcquisitionService(acquisitionService)
{
	mSessionSelector = StringProperty::initialize("tracking_session",
												  "Tracking Data",
												  "Select tracker data", "", QStringList(),
												  mOptions.getElement());
	connect(mAcquisitionService.get(), &AcquisitionService::recordedSessionsChanged, this, &SelectRecordSession::recordedSessionsChanged);
	connect(mSessionSelector.get(), &StringProperty::changed, this, &SelectRecordSession::showSelectedRecordingInView);

	this->recordedSessionsChanged();
}

void SelectRecordSession::setTool(ToolPtr tool)
{
	if (tool==mTool)
		return;

	this->clearTracer();

	mTool = tool;

	this->recordedSessionsChanged();
	this->showSelectedRecordingInView();
}

void SelectRecordSession::recordedSessionsChanged()
{
	if (!mTool) // optimization, see setTool()
		return;

	std::vector<RecordSessionPtr> sessions = mAcquisitionService->getSessions();
	QStringList uids;
	std::map<QString, QString> names;
	for(unsigned i=0; i<sessions.size(); ++i)
	{
		QString uid = sessions[i]->getUid();
		uids << uid;
		names[uid] = sessions[i]->getHumanDescription();
	}
	mSessionSelector->setValueRange(uids);
	mSessionSelector->setDisplayNames(names);

	if(mSessionSelector->getValue().isEmpty() && !uids.isEmpty())
		mSessionSelector->setValue(uids.last());
}


ToolRep3DPtr SelectRecordSession::getToolRepIn3DView()
{
	return mServices.visualizationService->get3DReps(0, 0)->findFirst<ToolRep3D>(mTool);
}

TimedTransformMap SelectRecordSession::getRecordedTrackerData_prMt()
{
	if(!mTool)
		return TimedTransformMap();

	RecordSessionPtr session;
	QString sessionUid = mSessionSelector->getValue();
	if(!sessionUid.isEmpty())
		session = mAcquisitionService->getSession(sessionUid);

	TimedTransformMap trackerRecordedData_prMt = RecordSession::getToolHistory_prMt(mTool, session);

	return trackerRecordedData_prMt;
}

void SelectRecordSession::showSelectedRecordingInView()
{
	TimedTransformMap trackerRecordedData_prMt = this->getRecordedTrackerData_prMt();

	ToolRep3DPtr activeRep3D = this->getToolRepIn3DView();
	if(!activeRep3D)
		return;

	if(!trackerRecordedData_prMt.empty())
	{
		activeRep3D->getTracer()->clear();
		activeRep3D->getTracer()->setColor(QColor("green"));
		activeRep3D->getTracer()->addManyPositions(trackerRecordedData_prMt);
	}
	else
	{
		activeRep3D->getTracer()->clear();
	}
}

void SelectRecordSession::clearTracer()
{
	ToolRep3DPtr activeRep3D = this->getToolRepIn3DView();
	if(activeRep3D)
	{
		activeRep3D->getTracer()->clear();
	}
}

} //namespace cx
