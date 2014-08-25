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

#include "cxtestQueuedSignalListener.h"

#include <iostream>
#include <QTimer>
#include <QEventLoop>
#include <boost/lexical_cast.hpp>
#include "cxTypeConversions.h"

namespace cxtest
{

bool waitForQueuedSignal(QObject* object, const char* signal, int maxWaitMilliSeconds, bool silentAtArrive)
{
	QueuedSignalListener listener(object, signal, maxWaitMilliSeconds);
	listener.exec();
	bool signalArrived = !listener.timedOut();
	std::string feedback = signalArrived ? " arrived." : " did NOT arrive. Timed out. ";
	if(!silentAtArrive)
		std::cout << "[QueuedSignalListener] " << signal << feedback << std::endl;
	return signalArrived;
}

QueuedSignalListener::QueuedSignalListener(QObject* object, const char* signal, int maxWaitMilliSeconds) :
		mTimedOut(false)
{
	createTimer(maxWaitMilliSeconds);
	createEventLoop(object, signal);
}

QueuedSignalListener::~QueuedSignalListener()
{
	delete mTimer;
	delete mLoop;
}

int QueuedSignalListener::exec()
{
	mTimer->start();
	int retval = mLoop->exec();
	return retval;
}

bool QueuedSignalListener::timedOut()
{
	return mTimedOut;
}

void QueuedSignalListener::quit()
{
	mTimedOut = (this->sender() == mTimer);
	mTimer->stop();
	mLoop->quit();
}

void QueuedSignalListener::createTimer(int maxWaitMilliSeconds)
{
	mTimer = new QTimer;
	mTimer->setInterval(maxWaitMilliSeconds);
	QObject::connect(mTimer, SIGNAL(timeout()), this, SLOT(quit()));
}

void QueuedSignalListener::createEventLoop(QObject* object, const char* signal)
{
	mLoop = new QEventLoop;
	QObject::connect(object, signal, this, SLOT(quit()));
}




} /* namespace cxtest */
