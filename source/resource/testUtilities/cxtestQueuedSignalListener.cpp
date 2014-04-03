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
