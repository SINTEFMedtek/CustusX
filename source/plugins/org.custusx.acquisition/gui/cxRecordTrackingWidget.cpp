/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
#include "cxRecordSessionSelector.h"
#include "cxBoolProperty.h"
#include "cxDoublePairProperty.h"


namespace cx
{

RecordTrackingWidget::RecordTrackingWidget(XmlOptionFile options,
											 AcquisitionServicePtr acquisitionService,
											 VisServicesPtr services,
											 QString category,
											 QWidget* parent,
											 bool useAdjustTracingData) :
	QWidget(parent),
	mServices(services),
	mOptions(options),
	mAcquisitionService(acquisitionService),
	mDrawAqquisitionIn3D(true),
	mUseAdjustTrackingData(useAdjustTracingData)
{
	QVBoxLayout* mVerticalLayout = new QVBoxLayout(this);

	mToolSelector = StringPropertySelectTool::New(services->tracking());

	mSelectRecordSession.reset(new SelectRecordSession(mOptions, acquisitionService, services));
	connect(mSelectRecordSession->getSessionSelector().get(), &StringProperty::changed, this, &RecordTrackingWidget::onMergeChanged);

	mMergeWithExistingSession = BoolProperty::initialize("mergerecording", "Merge",
														 "Merge new recording with selected recorded session",
														 false, QDomNode());
	connect(mMergeWithExistingSession.get(), &BoolProperty::changed, this, &RecordTrackingWidget::onMergeChanged);

	if(mUseAdjustTrackingData)
	{
		createAdjustTrackingDataTimeInterval(mOptions.getElement());
		connect(mSelectRecordSession->getSessionSelector().get(), &StringProperty::changed, this, &RecordTrackingWidget::onRecordSessionChanged);
		connect(mAdjustTrackingDataTimeInterval.get(), &DoublePairProperty::changed, this, &RecordTrackingWidget::onAdjustTrackingDataTimeIntervaChanged);
	}

	AcquisitionService::TYPES context(AcquisitionService::tTRACKING);
	mRecordSessionWidget = new RecordSessionWidget(mAcquisitionService, this, context, category);

	mVerticalLayout->setMargin(0);

	mToolSelectorWidget = sscCreateDataWidget(this, mToolSelector);
	mMergeWithExistingSessionWidget = sscCreateDataWidget(this, mMergeWithExistingSession);
	mVerticalLayout->addWidget(mToolSelectorWidget);
	mVerticalLayout->addWidget(mRecordSessionWidget);
	mVerticalLayout->addWidget(mMergeWithExistingSessionWidget);
	mVerticalLayout->addWidget(new LabeledComboBoxWidget(this, mSelectRecordSession->getSessionSelector()));
	if(mUseAdjustTrackingData)
		mVerticalLayout->addWidget(createDataWidget(mServices->view(), mServices->patient(), this, mAdjustTrackingDataTimeInterval));

	mObscuredListener.reset(new WidgetObscuredListener(this));
	connect(mObscuredListener.get(), SIGNAL(obscured(bool)), this, SLOT(obscuredSlot(bool)));
}

void RecordTrackingWidget::displayToolSelector(bool on)
{
	mToolSelectorWidget->setVisible(on);
}

StringPropertyPtr RecordTrackingWidget::getSessionSelector()
{
	return mSelectRecordSession->getSessionSelector();
}

void RecordTrackingWidget::acquisitionStarted()
{
	mRecordingTool = this->getSuitableRecordingTool();

	if(!mDrawAqquisitionIn3D)
		return;

	ToolRep3DPtr activeRep3D = this->getToolRepIn3DView();
	if (activeRep3D)
	{
		if (!mMergeWithExistingSession->getValue())
			activeRep3D->getTracer()->clear();
		activeRep3D->getTracer()->setColor(QColor("magenta"));
		activeRep3D->getTracer()->start();
	}
}

void RecordTrackingWidget::acquisitionStopped()
{
	QString newUid = mAcquisitionService->getLatestSession()->getUid();
	mSelectRecordSession->getSessionSelector()->setValue(newUid);

	mServices->patient()->autoSave();

	ToolRep3DPtr activeRep3D = this->getToolRepIn3DView();
	if (activeRep3D)
	{
		activeRep3D->getTracer()->stop();
	}
	mRecordingTool.reset();

	emit acquisitionCompleted();
}

void RecordTrackingWidget::acquisitionCancelled()
{
	ToolRep3DPtr activeRep3D = this->getToolRepIn3DView();
	if (activeRep3D)
	{
		activeRep3D->getTracer()->stop();
		activeRep3D->getTracer()->clear();
	}
	mRecordingTool.reset();
}

void RecordTrackingWidget::onMergeChanged()
{
	QString mergeSession = "";
	if (mMergeWithExistingSession->getValue())
		mergeSession = mSelectRecordSession->getSessionSelector()->getValue();

	mRecordSessionWidget->setCurrentSession(mergeSession);
}

ToolRep3DPtr RecordTrackingWidget::getToolRepIn3DView()
{
	return mServices->view()->get3DReps(0, 0)->findFirst<ToolRep3D>(mRecordingTool);
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
		connect(mServices->tracking().get(), &TrackingService::activeToolChanged, this, &RecordTrackingWidget::onToolChanged);
	}
	else
	{
		disconnect(mAcquisitionService.get(), &AcquisitionService::started, this, &RecordTrackingWidget::acquisitionStarted);
		disconnect(mAcquisitionService.get(), &AcquisitionService::acquisitionStopped, this, &RecordTrackingWidget::acquisitionStopped);
		disconnect(mAcquisitionService.get(), &AcquisitionService::cancelled, this, &RecordTrackingWidget::acquisitionCancelled);
		disconnect(mToolSelector.get(), &StringPropertySelectTool::changed, this, &RecordTrackingWidget::onToolChanged);
		disconnect(mServices->tracking().get(), &TrackingService::activeToolChanged, this, &RecordTrackingWidget::onToolChanged);
	}

	mSelectRecordSession->setVisible(!obscured);
}

void RecordTrackingWidget::onToolChanged()
{
	mSelectRecordSession->setTool(mToolSelector->getTool());
}


ToolPtr RecordTrackingWidget::getSuitableRecordingTool()
{
	ToolPtr retval = mToolSelector->getTool();
	if(!retval)
	{
		retval = mServices->tracking()->getActiveTool();
		if(retval)
			mToolSelector->setValue(retval->getUid());
	}
		return retval;
}

void RecordTrackingWidget::useBaseToolIfAvailable(bool useBaseTool)
//used in bronchoscopy navigation to get raw tool data without projection to centerline.
{
	ToolPtr tool = mToolSelector->getTool();
	if (!tool)
		return;
	mSelectRecordSession->setTool(tool);
	if(useBaseTool)
	{
		ToolPtr baseTool = tool->getBaseTool();
		if(baseTool)
			mSelectRecordSession->setTool(baseTool);
	}
}

TimedTransformMap RecordTrackingWidget::getRecordedTrackerData_prMt()
{
	return mSelectRecordSession->getRecordedTrackerData_prMt();
}

TimedTransformMap RecordTrackingWidget::getRecordedTrackerData_prMs()
{
	TimedTransformMap RecordedTrackerData_prMt =  this->getRecordedTrackerData_prMt();
	ToolPtr tool = mToolSelector->getTool();
	if(!tool)
		return RecordedTrackerData_prMt;
	Transform3D tMs = tool->getCalibration_sMt().inverse();

	TimedTransformMap RecordedTrackerData_prMs = RecordedTrackerData_prMt;
	for(TimedTransformMap::iterator iter=RecordedTrackerData_prMs.begin(); iter!=RecordedTrackerData_prMs.end(); ++iter)
		iter->second = iter->second*tMs;

	return RecordedTrackerData_prMs;
}

void RecordTrackingWidget::hideMergeWithExistingSession()
{
	if(!mMergeWithExistingSessionWidget->isHidden())
		mMergeWithExistingSessionWidget->hide();
}

void RecordTrackingWidget::drawAcquisitionIn3D(bool draw)
{
	mDrawAqquisitionIn3D = draw;
}

void RecordTrackingWidget::createAdjustTrackingDataTimeInterval(QDomElement root)
{
	mAdjustTrackingDataTimeInterval = DoublePairProperty::initialize("Adjust tracking data", "",
																																							"Adjust start and stop time for tracking data", DoubleRange(0, 0, 0.5), 1,
																				root);
}

void RecordTrackingWidget::onRecordSessionChanged()
{
	int secondsDiff = 0;
	if(mSelectRecordSession)
	{
		RecordSessionPtr recordedSession = mSelectRecordSession->getSession();
		if(recordedSession)
			if (recordedSession->getIntervalCount()>0)
			{
				mStartStopTimeBeforeAdjustment  = recordedSession->getInterval(0);
				secondsDiff = std::floor(mStartStopTimeBeforeAdjustment.first.msecsTo(mStartStopTimeBeforeAdjustment.second) / 1000);
			}
	}

	if(mAdjustTrackingDataTimeInterval)
	{
		mAdjustTrackingDataTimeInterval->setValueRange(DoubleRange(-ADJUST_TRACKING_DATA_OFFSET, secondsDiff + ADJUST_TRACKING_DATA_OFFSET, 0.5)); //Also possible to increase length with up to 10s before and after
		mAdjustTrackingDataTimeInterval->setValue(Eigen::Vector2d(0, secondsDiff));
	}
}

void RecordTrackingWidget::onAdjustTrackingDataTimeIntervaChanged()
{
	if(!mAdjustTrackingDataTimeInterval)
		return;

	Eigen::Vector2d timeInterval = mAdjustTrackingDataTimeInterval->getValue();
	DoubleRange range = mAdjustTrackingDataTimeInterval->getValueRange();

	RecordSessionPtr recordedSession = mSelectRecordSession->getSession();
	if(recordedSession)
	{
		if (recordedSession->getIntervalCount()>0)
		{
			std::pair<QDateTime, QDateTime> startAndStopTime = recordedSession->getInterval(0);
			startAndStopTime.first = mStartStopTimeBeforeAdjustment.first.addMSecs((timeInterval(0))*1000);
			startAndStopTime.second = mStartStopTimeBeforeAdjustment.second.addMSecs((timeInterval(1) - (range.max()-ADJUST_TRACKING_DATA_OFFSET))*1000);
			recordedSession->setInterval(0, startAndStopTime);
			mSelectRecordSession->setVisible(true);
		}
	}

}

} //namespace cx
