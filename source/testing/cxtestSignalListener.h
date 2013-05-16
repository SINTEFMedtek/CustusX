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
	SignalListener(QObject* object, const char* signal, int maxWaitMilliSeconds = 5000);
	virtual ~SignalListener();

	int exec(); ///< runs the eventloop that
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
