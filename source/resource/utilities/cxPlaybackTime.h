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

#ifndef CXPLAYBACKTIME_H_
#define CXPLAYBACKTIME_H_

#include <QObject>
#include <QDateTime>
#include <QColor>
#include <QTimer>
#include <boost/shared_ptr.hpp>

namespace cx
{

/**
 * \file
 * \addtogroup cxResourceUtilities
 * @{
 */

/**
 * \brief Controller for historic time, playback etc.
 *
 * \date Mar 29, 2012
 * \author Christian Askeland, SINTEF
 */
class PlaybackTime : public QObject
{
	Q_OBJECT
public:
	PlaybackTime();
	virtual ~PlaybackTime();

	void initialize(QDateTime start, qint64 length);

	void forward(qint64 msecs); ///< jump forward in ms
	void rewind(qint64 msecs); ///< jump backward in ms
	bool isPlaying() const;

	void setTime(QDateTime time);
	QDateTime getTime() const;
	void setOffset(qint64 val); ///< set time as an offset from start
	void moveOffset(qint64 delta); ///< change the offset with an amount
	qint64 getOffset() const;
	qint64 getLength() const; ///< length of recording in ms
	QDateTime getStartTime() const;
	void setSpeed(double val); ///< set speed as a ratio of real time. 1 is real time, less is slower, more is faster.
	double getSpeed() const;
	void setResolution(qint64 val); ///< set resolution in ms (signals are emitted with this spacing)
	double getResolution();

public slots:
	void start(); ///< start playing.
	void stop(); ///< stop playing and reset to start
	void pause(); ///< pause playing

signals:
	void changed();
private slots:
	void timeoutSlot();

private:
	QDateTime mStartTime;
//	QDateTime mTime;
	qint64 mOffset; ///< current offset from the start time.
	qint64 mLength;
	QTimer* mTimer;
	double mSpeed;

	QDateTime mPlayStart; ///< real time when play was last started.
	qint64 mLastPlayOffset; ///< offset when play was last started.

};

typedef boost::shared_ptr<PlaybackTime> PlaybackTimePtr;

/**\brief Description of one event in time.
 *
 * \sa PlaybackTime
 */
class TimelineEvent
{
public:
	TimelineEvent() : mStartTime(0), mEndTime(0), mColor("black") {}
	TimelineEvent(QString description, double start, double end) : mDescription(description), mStartTime(start), mEndTime(end) {}
	TimelineEvent(QString description, double time) : mDescription(description), mStartTime(time), mEndTime(time) {}
	QString mUid;
	QString mDescription;
	double mStartTime;
	double mEndTime;
	QColor mColor;
	QString mGroup;
	bool isInside(double time, double tol_ms=0) const;
	bool isSingular() const;
	bool isOverlap(const TimelineEvent& rhs) const;
	bool operator<(const TimelineEvent& rhs) const;
};


} /* namespace cx */
#endif /* CXPLAYBACKTIME_H_ */
