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

#ifndef CXTESTQUEUEDSIGNALLISTENER_H_
#define CXTESTQUEUEDSIGNALLISTENER_H_

#include "cxtestutilities_export.h"
#include <QObject>

class QTimer;
class QEventLoop;

namespace cxtest
{

bool CXTESTUTILITIES_EXPORT waitForQueuedSignal(QObject* object, const char* signal, int maxWaitMilliSeconds = 100, bool silentAtArrive = false);

/**
 * \brief Object that waits for a signal to arrive from a given QObject. If
 * this takes longer than a given time, it will time out. The signal connection is of
 * type Qt::QueuedConnection.
 *
 * \author Janne Beate Bakeng, SINTEF
 * \date May 15, 2013
 */
class CXTESTUTILITIES_EXPORT QueuedSignalListener : public QObject
{
	Q_OBJECT

public:
	QueuedSignalListener(QObject* object, const char* signal, int maxWaitMilliSeconds = 100);
	virtual ~QueuedSignalListener();

	int exec(); ///< runs the eventloop that makes sure signals are sent
	bool timedOut();

private slots:
	void quit();

private:
	void createTimer(int maxWaitMilliSeconds);
	void createEventLoop(QObject* object, const char* signal);

	QTimer* mTimer; ///< used to decide how long to wait for a signal to arrive
	QEventLoop* mLoop; ///< loop that makes sure all the qt signals and slots are run

	bool mTimedOut; ///< wheather or not the last run timed out or a signal arrived
};


} /* namespace cxtest */
#endif /* CXTESTQUEUEDSIGNALLISTENER_H_ */
