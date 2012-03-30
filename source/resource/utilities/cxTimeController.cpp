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

#include "cxTimeController.h"
#include <iostream>

namespace cx
{

TimeController::TimeController()
{
	mStartTime = QDateTime::currentDateTime();
	mOffset = 0;
	mLength = 10000;
	mSpeed = 1.0;

	mTimer = new QTimer;
	connect(mTimer, SIGNAL(timeout()), this, SLOT(timeoutSlot()));
	mTimer->setInterval(40);
}

void TimeController::initialize(QDateTime start, int length)
{
	this->stop();
	mStartTime = start;
	mLength = length;
}

TimeController::~TimeController()
{
	delete mTimer;
}

bool TimeController::isPlaying() const
{
	return mTimer->isActive();
}

void TimeController::start()
{
	mPlayStart = QDateTime::currentDateTime();
	mLastPlayOffset = mOffset;
//	mStartTime = QDateTime::currentDateTime();
	mTimer->start();
	this->timeoutSlot();
}

void TimeController::stop()
{
	mTimer->stop();
	mOffset = 0;
	this->timeoutSlot();
}

void TimeController::pause()
{
	mOffset = this->getOffset();
	mTimer->stop();
	this->timeoutSlot();
}

int TimeController::getOffset() const
{
	if (mTimer->isActive())
	{
		// find the offset from the last start to now.
		int offset = mPlayStart.msecsTo(QDateTime::currentDateTime());
		return mLastPlayOffset + mSpeed * offset;
	}
	else
	{
		return mOffset;
	}
}

void TimeController::timeoutSlot()
{
	// find the offset from the last start to now. Use to update mOffset.
//	int offset = mPlayStart.msecsTo(QDateTime::currentDateTime());
//	mOffset = mLastPlayOffset + offset;
//	mOffset = this->getOffset(); // do we need this???

//	int secs = mOffset; // SmStartTime.secsTo(QDateTime::currentDateTime());

//	QString text = QString("<font size=%1 color=%2><b>%3 ms</b></font>").arg(mFontSize).arg(color).arg(secs);
//	std::cout << mOffset << std::endl;

	emit changed();
}


void TimeController::forward(int msecs)
{
	this->moveOffset(msecs);
}

void TimeController::rewind(int msecs)
{
	this->moveOffset(-msecs);
}

void TimeController::moveOffset(int delta)
{
	this->setOffset(this->getOffset()+delta);
}

void TimeController::setTime(QDateTime time)
{
	this->setOffset(this->getStartTime().msecsTo(time));
}

QDateTime TimeController::getTime() const
{
	return this->getStartTime().addMSecs(this->getOffset());
}

void TimeController::setOffset(int val)
{
	bool playing = this->isPlaying();
	if (playing)
		this->stop();

	mOffset = std::max(0, val);
	this->timeoutSlot();

	if (playing)
		this->start();
}

int TimeController::getLength() const
{
	return mLength;
}

QDateTime TimeController::getStartTime() const
{
	return mStartTime;
}

void TimeController::setSpeed(double val)
{
	mSpeed = val;
}

double TimeController::getSpeed() const
{
	return mSpeed;
}

void TimeController::setResolution(int val)
{
	mTimer->setInterval(val);
}

double TimeController::getResolution(int val)
{
	return mTimer->interval();
}



} /* namespace cx */
