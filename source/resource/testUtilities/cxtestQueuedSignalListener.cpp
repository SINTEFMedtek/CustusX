/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
