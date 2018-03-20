/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
										 VisServicesPtr services) :
	mServices(services),
	mOptions(options),
	mAcquisitionService(acquisitionService),
	mVisible(true)
{
	mSessionSelector = StringProperty::initialize("tracking_session",
												  "Tracking Data",
												  "Select tracker data", "", QStringList(),
												  mOptions.getElement());
	connect(mAcquisitionService.get(), &AcquisitionService::recordedSessionsChanged, this, &SelectRecordSession::recordedSessionsChanged);
	connect(mSessionSelector.get(), &StringProperty::changed, this, &SelectRecordSession::showSelectedRecordingInView);

	this->recordedSessionsChanged();
	this->updateHelpText();
}

SelectRecordSession::~SelectRecordSession()
{
	this->clearTracer();
}

void SelectRecordSession::setTool(ToolPtr tool)
{
	if (tool==mToolOverride)
		return;

	mToolOverride = tool;

	this->showSelectedRecordingInView();
}

void SelectRecordSession::recordedSessionsChanged()
{
	std::vector<RecordSessionPtr> sessions = mAcquisitionService->getSessions();
	QStringList uids;
	std::map<QString, QString> names;
	for(unsigned i=0; i<sessions.size(); ++i)
	{
		QString uid = sessions[i]->getUid();
		uids << uid;
		names[uid] = sessions[i]->getHumanDescription();
	}
	uids << "";
	names[""] = "<none>";

	mSessionSelector->setValueRange(uids);
	mSessionSelector->setDisplayNames(names);

	if(mSessionSelector->getValue().isEmpty() && !uids.isEmpty())
		mSessionSelector->setValue(uids.last());

	this->showSelectedRecordingInView();
}

void SelectRecordSession::warnIfNoTrackingDataInSession()
{
	RecordSessionPtr session = this->getSession();
	ToolPtr tool = this->getTool();
	if (session && !tool)
	{
		CX_LOG_WARNING() << QString("Could not find any tracking data for any loaded tools in session [%2]. ")
						  .arg(session.get() ? session->getHumanDescription() : "NULL");
	}
}

RecordSessionPtr SelectRecordSession::getSession()
{
	QString uid = mSessionSelector->getValue();
	if(!uid.isEmpty())
		return mAcquisitionService->getSession(uid);
	return RecordSessionPtr();
}

TimedTransformMap SelectRecordSession::getRecordedTrackerData_prMt()
{
	RecordSessionPtr session = this->getSession();
	ToolPtr tool = this->getTool();

	TimedTransformMap trackerRecordedData_prMt = RecordSession::getToolHistory_prMt(tool, session, false);

	return trackerRecordedData_prMt;
}

ToolPtr SelectRecordSession::getTool()
{
	if (mToolOverride)
		return mToolOverride;

	RecordSessionPtr session = this->getSession();
	TrackingService::ToolMap tools = mServices->tracking()->getTools();

	ToolPtr tool = this->findToolContainingMostDataForSession(tools, session);
	return tool;
}

void SelectRecordSession::setVisible(bool on)
{
	mVisible = on;
	this->showSelectedRecordingInView();
}

ToolPtr SelectRecordSession::findToolContainingMostDataForSession(std::map<QString, ToolPtr> tools, RecordSessionPtr session)
{
	std::map<int,ToolPtr> tooldata;

	for (TrackingService::ToolMap::iterator i=tools.begin(); i!=tools.end(); ++i)
	{
		if (i->second->hasType(Tool::TOOL_REFERENCE))
			continue;
		TimedTransformMap trackerRecordedData_prMt = RecordSession::getToolHistory_prMt(i->second, session, false);
		tooldata[trackerRecordedData_prMt.size()] = i->second;
	}

	if (!tooldata.empty() && (tooldata.rbegin()->first!=0))
		return tooldata.rbegin()->second;

	return ToolPtr();
}

void SelectRecordSession::updateHelpText()
{
	ToolPtr tool = this->getTool();
	QString toolname = tool ? tool->getName() : "<none>";
	mSessionSelector->setHelp(QString("Select tracker data, current data uses tool %2")
							  .arg(toolname));
}

ToolRep3DPtr SelectRecordSession::getToolRepIn3DView(ToolPtr tool)
{
	return mServices->view()->get3DReps(0, 0)->findFirst<ToolRep3D>(tool);
}

void SelectRecordSession::showSelectedRecordingInView()
{
	this->clearTracer();
	if (!mVisible)
		return;

	this->warnIfNoTrackingDataInSession();

	TimedTransformMap trackerRecordedData_prMt = this->getRecordedTrackerData_prMt();
	if (trackerRecordedData_prMt.empty())
		return;

	mCurrentTracedTool = this->getTool();
	this->updateHelpText();

	ToolRep3DPtr activeRep3D = this->getToolRepIn3DView(mCurrentTracedTool);
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
	ToolRep3DPtr activeRep3D = this->getToolRepIn3DView(mCurrentTracedTool);

	if(activeRep3D)
	{
		activeRep3D->getTracer()->clear();
	}

	mCurrentTracedTool.reset();
}

} //namespace cx
