/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
