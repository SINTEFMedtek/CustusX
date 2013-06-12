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

#include "cxtestSignalListener.h"

#include <iostream>
#include <QTimer>
#include <QEventLoop>
#include <boost/lexical_cast.hpp>

namespace cxtest
{

bool waitForSignal(QObject* object, const char* signal, int maxWaitMilliSeconds)
{
	SignalListener listener(object, signal, maxWaitMilliSeconds);
	listener.exec();
	bool signalArrived = !listener.timedOut();
	std::string arrived = signalArrived ? "" : "NOT ";
	std::cout << "[SIGNALLISTENER] " << signal << " did " << arrived << "arrive." << std::endl;
	return signalArrived;
}

SignalListener::SignalListener(QObject* object, const char* signal, int maxWaitMilliSeconds) :
		mTimedOut(false)
{
	createTimer(maxWaitMilliSeconds);
	createEventLoop(object, signal);
}

SignalListener::~SignalListener()
{
	delete mTimer;
	delete mLoop;
}

int SignalListener::exec()
{
	mTimer->start();
	int retval = mLoop->exec();
	return retval;
}

bool SignalListener::timedOut()
{
	return mTimedOut;
}

void SignalListener::quit()
{
	mTimedOut = (this->sender() == mTimer);
	mTimer->stop();
	mLoop->quit();
}

void SignalListener::createTimer(int maxWaitMilliSeconds)
{
	mTimer = new QTimer;
	mTimer->setInterval(maxWaitMilliSeconds);
	QObject::connect(mTimer, SIGNAL(timeout()), this, SLOT(quit()));
}

void SignalListener::createEventLoop(QObject* object, const char* signal)
{
	mLoop = new QEventLoop;
	QObject::connect(object, signal, this, SLOT(quit()));
}

} /* namespace cxtest */
