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

#include "cxTimeControllerWidget.h"

namespace cx
{

TimeControllerWidget::TimeControllerWidget(QWidget* parent) :
				BaseWidget(parent, "TimeControllerWidget", "Time Control")
{
	this->setToolTip(this->defaultWhatsThis());

	mTimer.reset(new TimeController());
	mTimer->initialize(QDateTime::currentDateTime(), 100000);
	connect(mTimer.get(), SIGNAL(changed()), SLOT(timeChangedSlot()));

	QVBoxLayout* topLayout = new QVBoxLayout(this);

	mLabel = new QLabel;
	topLayout->addWidget(mLabel);

	// create buttons bar
	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	topLayout->addLayout(buttonsLayout);

	this->createAction(this,
	                QIcon(":/icons/open_icon_library/png/64x64/actions/media-seek-backward-3.png"),
					"Rewind", "",
	                SLOT(rewindSlot()),
	                buttonsLayout);
	mPlayAction = this->createAction(this,
	                QIcon(":/icons/open_icon_library/png/64x64/actions/media-playback-start-3.png"),
					"Play", "",
	                SLOT(playSlot()),
	                buttonsLayout);
//	this->createAction(this,
//	                QIcon(":/icons/open_icon_library/png/64x64/actions/media-playback-pause-3.png"),
//					"Pause", "",
//	                SLOT(pauseSlot()),
//	                buttonsLayout);
	this->createAction(this,
	                QIcon(":/icons/open_icon_library/png/64x64/actions/media-seek-forward-3.png"),
					"Forward", "",
	                SLOT(forwardSlot()),
	                buttonsLayout);
	this->createAction(this,
	                QIcon(":/icons/open_icon_library/png/64x64/actions/media-playback-stop-3.png"),
					"Stop", "",
	                SLOT(stopSlot()),
	                buttonsLayout);

	buttonsLayout->addStretch();

	topLayout->addStretch();
}

TimeControllerWidget::~TimeControllerWidget()
{
	// TODO Auto-generated destructor stub
}

QString TimeControllerWidget::defaultWhatsThis() const
{
	return "";
}

void TimeControllerWidget::timeChangedSlot()
{
	QString color("green");
	int fontSize = 14;
	int secs = mTimer->getOffset(); // SmStartTime.secsTo(QDateTime::currentDateTime());
	mLabel->setText(QString("<font size=%1 color=%2><b>%3 ms</b></font>").arg(fontSize).arg(color).arg(secs));

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

}

void TimeControllerWidget::playSlot()
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
void TimeControllerWidget::pauseSlot()
{
	mTimer->pause();
}
void TimeControllerWidget::stopSlot()
{
	mTimer->stop();
}
void TimeControllerWidget::forwardSlot()
{
	mTimer->forward(1000);
}
void TimeControllerWidget::rewindSlot()
{
	mTimer->rewind(1000);
}

} /* namespace cx */
