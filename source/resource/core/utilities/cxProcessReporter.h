#ifndef CXPROCESSREPORTER_H_
#define CXPROCESSREPORTER_H_

#include "boost/shared_ptr.hpp"
#include <QProcess>
#include <QString>

namespace cx
{

typedef boost::shared_ptr<class ProcessReporter> ProcessReporterPtr;

/**
 * Listens to events from QProcess and redirects them to the messagemanager.
 *
 * \date Oct 22 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class ProcessReporter : QObject
{
	Q_OBJECT

public:
	ProcessReporter(QProcess* process, QString name);
	~ProcessReporter();

private slots:
	void processReadyRead();
	void processStateChanged(QProcess::ProcessState newState);
	void processError(QProcess::ProcessError error);
	void processFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
	QProcess* mProcess;

	QString mName;
};
;

} /* namespace cx */
#endif /* CXPROCESSREPORTER_H_ */
