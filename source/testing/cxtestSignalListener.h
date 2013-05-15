#ifndef CXTESTSIGNALLISTENER_H_
#define CXTESTSIGNALLISTENER_H_

#include <QObject>

class QTimer;
class QEventLoop;

namespace cxtest
{

bool waitForSignal(QObject* object, const char* signal, int maxWaitMilliSeconds = 5000);

class SignalListener : public QObject
{
	Q_OBJECT

public:
	SignalListener(QObject* object, const char* signal, int maxWaitMilliSeconds = 5000);
	virtual ~SignalListener();

	int exec();
	bool timedOut();

private slots:
	void quit();

private:
	QTimer* mTimer;
	QEventLoop* mLoop;

	bool mTimedOut;
};


} /* namespace cxtest */
#endif /* CXTESTSIGNALLISTENER_H_ */
