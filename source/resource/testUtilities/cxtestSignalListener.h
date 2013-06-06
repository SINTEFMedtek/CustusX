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

#ifndef CXTESTSIGNALLISTENER_H_
#define CXTESTSIGNALLISTENER_H_

#include <QObject>

class QTimer;
class QEventLoop;

namespace cxtest
{

bool waitForSignal(QObject* object, const char* signal, int maxWaitMilliSeconds = 100);

/**
 * \brief Object that waits for a signal to arrive from a given QObject. If
 * this takes longer than a given time, it will time out.
 *
 * \author Janne Beate Bakeng, SINTEF
 * \date May 15, 2013
 */
class SignalListener : public QObject
{
	Q_OBJECT

public:
	SignalListener(QObject* object, const char* signal, int maxWaitMilliSeconds = 100);
	virtual ~SignalListener();

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
#endif /* CXTESTSIGNALLISTENER_H_ */
