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

#ifndef CXPLAYBACKTIME_H_
#define CXPLAYBACKTIME_H_

#include "cxResourceExport.h"

#include <QObject>
#include <QDateTime>
#include <QColor>
#include <QTimer>
#include <boost/shared_ptr.hpp>

namespace cx
{

/**
 * \file
 * \addtogroup cx_resource_core_utilities
 * @{
 */

/**
 * \brief Controller for historic time, playback etc.
 *
 * \date Mar 29, 2012
 * \author Christian Askeland, SINTEF
 */
class cxResource_EXPORT PlaybackTime : public QObject
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
class cxResource_EXPORT TimelineEvent
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
