/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXPROCESSREPORTER_H_
#define CXPROCESSREPORTER_H_

#include "cxResourceExport.h"

#include "boost/shared_ptr.hpp"
#include <QProcess>
#include <QString>

namespace cx
{

typedef boost::shared_ptr<class ProcessReporter> ProcessReporterPtr;

/**
 * Listens to events from QProcess and redirects them to the Reporter.
 *
 * \ingroup cx_resource_core_utilities
 * \date Oct 22 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class cxResource_EXPORT ProcessReporter : QObject
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
