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

#include "cxPlaybackTime.h"
#include <iostream>
#include "sscTime.h"
#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "sscVector3D.h"

namespace cx
{

PlaybackTime::PlaybackTime()
{
	mStartTime = QDateTime::currentDateTime();
	mOffset = 0;
	mLength = 10000;
	mSpeed = 1.0;

	mTimer = new QTimer;
	connect(mTimer, SIGNAL(timeout()), this, SLOT(timeoutSlot()));
	mTimer->setInterval(40);
}

void PlaybackTime::initialize(QDateTime start, qint64 length)
{
	this->stop();
	mStartTime = start;
	mLength = length;

//	ssc::messageManager()->sendInfo(QString("Initialized PlaybackTime with start time [%1] and end time [%2]")
//					.arg(mStartTime.toString(ssc::timestampMilliSecondsFormatNice()))
//					.arg(mStartTime.addMSecs(mLength).toString(ssc::timestampMilliSecondsFormatNice())));
}

PlaybackTime::~PlaybackTime()
{
	delete mTimer;
}

bool PlaybackTime::isPlaying() const
{
	return mTimer->isActive();
}

void PlaybackTime::start()
{
	mPlayStart = QDateTime::currentDateTime();
	mLastPlayOffset = mOffset;
//	mStartTime = QDateTime::currentDateTime();
	mTimer->start();
	this->timeoutSlot();
}

void PlaybackTime::stop()
{
	mTimer->stop();
	mOffset = 0;
	this->timeoutSlot();
}

void PlaybackTime::pause()
{
	mOffset = this->getOffset();
	mTimer->stop();
	this->timeoutSlot();
}

qint64 PlaybackTime::getOffset() const
{
	if (mTimer->isActive())
	{
		// find the offset from the last start to now.
		qint64 offset = mPlayStart.msecsTo(QDateTime::currentDateTime());
		return mLastPlayOffset + mSpeed * offset;
	}
	else
	{
		return mOffset;
	}
}

void PlaybackTime::timeoutSlot()
{
	// find the offset from the last start to now. Use to update mOffset.
//	int offset = mPlayStart.msecsTo(QDateTime::currentDateTime());
//	mOffset = mLastPlayOffset + offset;
//	mOffset = this->getOffset(); // do we need this???

//	int secs = mOffset; // SmStartTime.secsTo(QDateTime::currentDateTime());

//	QString text = QString("<font size=%1 color=%2><b>%3 ms</b></font>").arg(mFontSize).arg(color).arg(secs);
//	std::cout << mOffset << std::endl;

	if (this->getOffset() > mLength)
	{
		// similar to pause(), except dont call timeout recusively
		mOffset = mLength;
		mTimer->stop();
	}
	else
		emit changed();
}


void PlaybackTime::forward(qint64 msecs)
{
	this->moveOffset(msecs);
}

void PlaybackTime::rewind(qint64 msecs)
{
	this->moveOffset(-msecs);
}

void PlaybackTime::moveOffset(qint64 delta)
{
	this->setOffset(this->getOffset()+delta);
}

void PlaybackTime::setTime(QDateTime time)
{
	this->setOffset(this->getStartTime().msecsTo(time));
}

QDateTime PlaybackTime::getTime() const
{
//	std::cout << "gettime " << this->getStartTime().addMSecs(this->getOffset()).toString(ssc::timestampMilliSecondsFormatNice()) << std::endl;

	return this->getStartTime().addMSecs(this->getOffset());
}

/**Sentry object for stopping play during object lifetime
 *
 */
struct TemporaryPausePlay
{
	TemporaryPausePlay(PlaybackTime* base) : mBase(base)
	{
		mPlaying = mBase->isPlaying();
		if (mPlaying)
			mBase->pause();
	}
	~TemporaryPausePlay()
	{
		if (mPlaying)
			mBase->start();
	}
	bool mPlaying;
	PlaybackTime* mBase;
};

void PlaybackTime::setOffset(qint64 val)
{
	if (val==mOffset)
		return;

	TemporaryPausePlay sentry(this);
//	bool playing = this->isPlaying();
//	if (playing)
//		this->stop();

	mOffset = std::max<quint64>(0, val);
	this->timeoutSlot();

//	if (playing)
//		this->start();
}

qint64 PlaybackTime::getLength() const
{
	return mLength;
}

QDateTime PlaybackTime::getStartTime() const
{
	return mStartTime;
}

void PlaybackTime::setSpeed(double val)
{
	TemporaryPausePlay sentry(this);
	mSpeed = val;
}

double PlaybackTime::getSpeed() const
{
	return mSpeed;
}

void PlaybackTime::setResolution(qint64 val)
{
	mTimer->setInterval(val);
}

double PlaybackTime::getResolution()
{
	return mTimer->interval();
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


/** Return whether time is inside event.
 *  The minimum length of the event if set to tol_ms.
 */
bool TimelineEvent::isInside(double time, double tol_ms) const
{
	double w = mEndTime - mStartTime;
	double m = mStartTime + w/2;
	return fabs(time - m) < std::max(w, tol_ms)/2;
}
bool TimelineEvent::isSingular() const { return ssc::similar(mEndTime,mStartTime); }
bool TimelineEvent::isOverlap(const TimelineEvent& rhs) const
{
	double w0 = mEndTime - mStartTime;
	double m0 = mStartTime + w0/2;
	double w1 = rhs.mEndTime - rhs.mStartTime;
	double m1 = rhs.mStartTime + w1/2;
	return fabs(m1-m0) < (w1+w0)/2;
}
bool TimelineEvent::operator<(const TimelineEvent& rhs) const
{
	return mStartTime < rhs.mStartTime;
}


} /* namespace cx */
