// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxPlaybackWidget.h"

#include <QPainter>
#include <QToolTip>
#include <QMouseEvent>
#include "cxToolManager.h"
#include "sscHelperWidgets.h"
#include "sscTime.h"
#include "sscMessageManager.h"
#include "sscLogger.h"
#include "sscTypeConversions.h"
#include "cxTimelineWidget.h"
#include "sscDataManager.h"
#include "sscData.h"

namespace cx
{

PlaybackWidget::PlaybackWidget(QWidget* parent) :
				BaseWidget(parent, "TimeControllerWidget", "Time Control")
{
	mOpen = false;
	this->setToolTip(this->defaultWhatsThis());

	connect(ssc::toolManager(), SIGNAL(initialized()), this, SLOT(toolManagerInitializedSlot()));

	mTimer.reset(new PlaybackTime());
	mTimer->initialize(QDateTime::currentDateTime(), 100000);
	connect(mTimer.get(), SIGNAL(changed()), SLOT(timeChangedSlot()));

	QVBoxLayout* topLayout = new QVBoxLayout(this);


	QHBoxLayout* controlButtonsLayout = new QHBoxLayout;
	topLayout->addLayout(controlButtonsLayout);

	mOpenAction = this->createAction(this,
//	                QIcon(":/icons/open_icon_library/png/64x64/actions/tool-animator.png"),
	        		QIcon(":/icons/open_icon_library/png/64x64/others/button-red.png"),
					"Open Playback", "",
	                SLOT(toggleOpenSlot()),
	                controlButtonsLayout);
//	mOpenAction->setCheckable(true);
//	mPlayAction = this->createAction(this,
//	                QIcon(""),
//					"Stop Playback", "",
//	                SLOT(closeSlot()),
//	                controlButtonsLayout);

	controlButtonsLayout->addStretch();

	mStartTimeLabel = new QLabel;
	topLayout->addWidget(mStartTimeLabel);
	mTotalLengthLabel = new QLabel;
	topLayout->addWidget(mTotalLengthLabel);
	mLabel = new QLabel;
	topLayout->addWidget(mLabel);

	mToolTimelineWidget = new TimelineWidget(this);
	connect(mToolTimelineWidget, SIGNAL(positionChanged()), this, SLOT(timeLineWidgetValueChangedSlot()));
	topLayout->addWidget(mToolTimelineWidget);

	mTimeLineSlider = new QSlider(this);
	mTimeLineSlider->setMinimumWidth(50);
	mTimeLineSlider->setOrientation(Qt::Horizontal);
	connect(mTimeLineSlider, SIGNAL(valueChanged(int)), this, SLOT(timeLineSliderValueChangedSlot(int)));
	topLayout->addWidget(mTimeLineSlider);

	// create buttons bar
	QHBoxLayout* playButtonsLayout = new QHBoxLayout;
	topLayout->addLayout(playButtonsLayout);

	this->createAction(this,
	                QIcon(":/icons/open_icon_library/png/64x64/actions/media-seek-backward-3.png"),
					"Rewind", "",
	                SLOT(rewindSlot()),
	                playButtonsLayout);
	mPlayAction = this->createAction(this,
	                QIcon(":/icons/open_icon_library/png/64x64/actions/media-playback-start-3.png"),
					"Play", "",
	                SLOT(playSlot()),
	                playButtonsLayout);
	this->createAction(this,
	                QIcon(":/icons/open_icon_library/png/64x64/actions/media-seek-forward-3.png"),
					"Forward", "",
	                SLOT(forwardSlot()),
	                playButtonsLayout);
	this->createAction(this,
	                QIcon(":/icons/open_icon_library/png/64x64/actions/media-playback-stop-3.png"),
					"Stop", "",
	                SLOT(stopSlot()),
	                playButtonsLayout);

	mSpeedAdapter = ssc::DoubleDataAdapterXml::initialize(
					"speed",
					"Speed",
					"Set speed of playback, 0 is normal speed.", 0, ssc::DoubleRange(-5,5,1),0);
	connect(mSpeedAdapter.get(), SIGNAL(changed()), this, SLOT(speedChangedSlot()));
	playButtonsLayout->addWidget(ssc::createDataWidget(this, mSpeedAdapter));

	playButtonsLayout->addStretch();

	topLayout->addStretch();
}

PlaybackWidget::~PlaybackWidget()
{
	// TODO Auto-generated destructor stub
}

QString PlaybackWidget::defaultWhatsThis() const
{
	return "";
}

void PlaybackWidget::timeLineSliderValueChangedSlot(int val)
{
	mTimer->setOffset(val);
}

void PlaybackWidget::timeLineWidgetValueChangedSlot()
{
	mTimer->setOffset(mToolTimelineWidget->getPos());
}

//mPlayAction->setIcon(QIcon(":/icons/open_icon_library/png/64x64/actions/media-playback-pause-3.png"));
//mPlayAction->setText("Pause");
//}
//else
//{
//mPlayAction->setIcon(QIcon(":/icons/open_icon_library/png/64x64/actions/media-playback-start-3.png"));
//mPlayAction->setText("Play");

void PlaybackWidget::toggleOpenSlot()
{
	if (cx::ToolManager::getInstance()->isPlaybackMode())
	{
		ToolManager::getInstance()->closePlayBackMode();
	}
	else
	{
		ToolManager::getInstance()->setPlaybackMode(mTimer);
	}
}

std::vector<TimelineWidget::TimelineEvent> PlaybackWidget::convertHistoryToEvents(ssc::ToolPtr tool)
{
	std::vector<TimelineWidget::TimelineEvent> retval;
	double zero = mTimer->getStartTime().toMSecsSinceEpoch();
	ssc::TimedTransformMapPtr history = tool->getPositionHistory();
	if (!history || history->empty())
		return retval;
	double timeout = 200;
	TimelineWidget::TimelineEvent currentEvent(tool->getName() + " visible", history->begin()->first - zero);
//	std::cout << "first event start: " << currentEvent.mDescription << " " << currentEvent.mStartTime << " " << history->size() << std::endl;

	for(ssc::TimedTransformMap::iterator iter=history->begin(); iter!=history->end(); ++iter)
	{
		double current = iter->first - zero;

		if (current - currentEvent.mEndTime > timeout)
		{
			retval.push_back(currentEvent);
			currentEvent.mStartTime = currentEvent.mEndTime = current;
		}
		else
		{
			currentEvent.mEndTime = current;
		}
	}
	if (!ssc::similar(currentEvent.mEndTime - currentEvent.mStartTime, 0))
		retval.push_back(currentEvent);

	return retval;
}

std::vector<TimelineWidget::TimelineEvent> PlaybackWidget::createEvents()
{
	// find all valid regions (i.e. time sequences with tool navigation)
	std::vector<TimelineWidget::TimelineEvent> events;
	ssc::ToolManager::ToolMapPtr tools = ssc::toolManager()->getTools();
	for (ssc::ToolManager::ToolMap::iterator iter=tools->begin(); iter!=tools->end(); ++iter)
	{
		std::vector<TimelineWidget::TimelineEvent> current = convertHistoryToEvents(iter->second);
		copy(current.begin(), current.end(), std::back_inserter(events));
	}

//	std::map<QString, ssc::DataPtr> data = ssc::dataManager()->getData();
//	for (std::map<QString, ssc::DataPtr>::iterator iter=data.begin(); iter!=data.end(); ++iter)
//	{
//		double zero = mTimer->getStartTime().toMSecsSinceEpoch();
//		QString desc("loaded " + iter->second->getName());
//		int acqTime = iter->second->getAcquisitionTime().toMSecsSinceEpoch() - zero;
//		events.push_back(TimelineWidget::TimelineEvent(desc, acqTime));
//
//		ssc::RegistrationHistoryPtr reg = iter->second->get_rMd_History();
//	}

	// test: add some single-time events:
	events.push_back(TimelineWidget::TimelineEvent("one1", 10000));
	events.push_back(TimelineWidget::TimelineEvent("one2", 20000));
	events.push_back(TimelineWidget::TimelineEvent("one3", 80000));

	return events;
}

void PlaybackWidget::toolManagerInitializedSlot()
{
	if (cx::ToolManager::getInstance()->isPlaybackMode())
	{
		mOpenAction->setText("Close Playback");
		mOpenAction->setIcon(QIcon(":/icons/open_icon_library/png/64x64/others/button-green.png"));
	}
	else
	{
		mOpenAction->setText("Open Playback");
		mOpenAction->setIcon(QIcon(":/icons/open_icon_library/png/64x64/others/button-red.png"));
		return;
	}

	std::vector<TimelineWidget::TimelineEvent> events = this->createEvents();
	mToolTimelineWidget->setRange(0, mTimer->getLength());
	mToolTimelineWidget->setEvents(events);

	QString startDate = mTimer->getStartTime().toString("yyyy-MM-dd");
	QString startTime = mTimer->getStartTime().toString("hh:mm");
	QString endTime = mTimer->getStartTime().addMSecs(mTimer->getLength()).toString("hh:mm");
	QString length = this->stripLeadingZeros(QTime(0,0,0,0).addMSecs(mTimer->getLength()).toString("hh:mm:ss"));
	mStartTimeLabel->setText(
					QString("Date:").leftJustified(15)    +"\t" + startDate+"\n" +
					QString("Time:").leftJustified(15)    +"\t" + startTime + " - " + endTime + "\n" +
					QString("Duration:").leftJustified(15)+"\t" + length);

	this->timeChangedSlot();
}

void PlaybackWidget::speedChangedSlot()
{
	double speed = mSpeedAdapter->getValue();
	speed = pow(2,speed);
	mTimer->setSpeed(speed);
}

/**strip leading zeros from a hh:mm:ss-formatted time
 *
 */
QString PlaybackWidget::stripLeadingZeros(QString time)
{
	QStringList split = time.split(":");
	bool ok = false;
	while (!split.empty() && (split.front().toInt(&ok)==0) && ok)
	{
		split.pop_front();
	}
	return split.join(":");
}

void PlaybackWidget::timeChangedSlot()
{
	QString color("green");
	int fontSize = 4;
	int offset = mTimer->getOffset(); // SmStartTime.secsTo(QDateTime::currentDateTime());
	QString format = QString("<font size=%1 color=%2><b>%3</b></font>").arg(fontSize).arg(color);

	QString currentTime = mTimer->getTime().toString("hh:mm:ss");
	QString currentOffset = this->stripLeadingZeros(QTime(0,0,0,0).addMSecs(offset).toString("hh:mm:ss.zzz"));

	mLabel->setText(format.arg("Elapsed: "+currentOffset+" \tTime: " + currentTime));

	if (mTimer->isPlaying())
	{
		mPlayAction->setIcon(QIcon(":/icons/open_icon_library/png/64x64/actions/media-playback-pause-3.png"));
		mPlayAction->setText("Pause");
	}
	else
	{
		mPlayAction->setIcon(QIcon(":/icons/open_icon_library/png/64x64/actions/media-playback-start-3.png"));
		mPlayAction->setText("Play");
	}

	mTimeLineSlider->blockSignals(true);
//	mTimeLineSlider->setDoubleRange(dRange); // in case the image is changed
	mTimeLineSlider->setRange(0, mTimer->getLength());
	mTimeLineSlider->setSingleStep(1);

	mTimeLineSlider->setValue(offset);
	mTimeLineSlider->setToolTip(QString("Current time"));
	mTimeLineSlider->blockSignals(false);

	mToolTimelineWidget->blockSignals(true);
	mToolTimelineWidget->setPos(offset);
	mToolTimelineWidget->blockSignals(false);
}

void PlaybackWidget::playSlot()
{
	if (mTimer->isPlaying())
	{
		mTimer->pause();
	}
	else
	{
		mTimer->start();
	}
}
void PlaybackWidget::pauseSlot()
{
	mTimer->pause();
}
void PlaybackWidget::stopSlot()
{
	mTimer->stop();
}
void PlaybackWidget::forwardSlot()
{
	mTimer->forward(1000);
}
void PlaybackWidget::rewindSlot()
{
	mTimer->rewind(1000);
}


} /* namespace cx */
