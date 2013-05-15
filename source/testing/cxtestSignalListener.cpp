#include "cxtestSignalListener.h"

#include <iostream>
#include <QTimer>
#include <QEventLoop>

namespace cxtest
{

///< Usage:
///< QObject* object = ...;
///< waitForSignal(object, SIGNAL(newPackage()));
bool waitForSignal(QObject* object, const char* signal, int maxWaitMilliSeconds)
{
	SignalListener listener(object, signal, maxWaitMilliSeconds);

	listener.exec();
	return listener.timedOut();


//	QTimer* timer = new QTimer;
//	timer->setInterval(maxWaitMilliSeconds);
//	timer->start();
//
//	QEventLoop loop;
//	QObject::connect(timer, SIGNAL(timeout()), &loop, SLOT(quit()));
//	QObject::connect(object, signal, &loop, SLOT(quit()));
//
//	// Execute the event loop here, now we will wait here until the given signal is emitted
//	// which in turn will trigger event loop quit.
//	loop.exec();


}

SignalListener::SignalListener(QObject* object, const char* signal, int maxWaitMilliSeconds) :
		mTimedOut(false)
{
	mTimer = new QTimer;
	mTimer->setInterval(maxWaitMilliSeconds);
	QObject::connect(mTimer, SIGNAL(timeout()), this, SLOT(quit()));

	mLoop = new QEventLoop;
	QObject::connect(object, signal, this, SLOT(quit()));
//	QObject::connect(mTimer, SIGNAL(timeout()), mLoop, SLOT(quit()));
//	QObject::connect(object, signal, mLoop, SLOT(quit()));
}

SignalListener::~SignalListener()
{
	delete mTimer;
	delete mLoop;
}

int SignalListener::exec()
{
	mTimer->start();
	mLoop->exec(); // Execute the event loop here, now we will wait here until the given signal is emitted.
}

bool SignalListener::timedOut()
{
	return mTimedOut;
}

void SignalListener::quit()
{
	mTimedOut = (this->sender() == mTimer) ? false : true;
	if(this->sender() == mTimer)
		std::cout  << "timed out" << std::endl;
	else
		std::cout << "signal" << std::endl;

	mTimer->stop();
	mLoop->quit();
}

} /* namespace cxtest */
