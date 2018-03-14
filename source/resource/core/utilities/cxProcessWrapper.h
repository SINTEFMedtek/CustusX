	/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXPROCESSWRAPPER_H_
#define CXPROCESSWRAPPER_H_

#include "cxResourceExport.h"

#include <QObject>
#include "boost/shared_ptr.hpp"
#include <QStringList>
#include <QString>
#include <QProcess>
#include <QPointer>
#include "cxProcessReporter.h"

namespace cx
{

/**
* \file
* \addtogroup cx_resource_core_utilities
* @{
*/

typedef boost::shared_ptr<class ProcessWrapper> ProcessWrapperPtr;

/**
 * Wraps a QProcess and supplies a interface that integrates nicely with the rest of CustusX.
 *
 *  \date Oct 19, 2012
 *  \author Christian Askeland, SINTEF
 *  \author Janne Beate Bakeng, SINTEF
 */
class cxResource_EXPORT ProcessWrapper : public QObject
{
	Q_OBJECT
public:
	explicit ProcessWrapper(QString name = "executable", QObject* parent = NULL);
	virtual ~ProcessWrapper();

	QProcess *getProcess();

	void launchWithRelativePath(QString executable, QStringList arguments = QStringList());
	void launch(QString executable, QStringList argument = QStringList());

	bool isRunning();

	qint64 write(const char * data);
	bool waitForStarted(int msecs = 30000);
	bool waitForFinished(int msecs = 30000);

signals:
	void stateChanged();

private:
	QString getExecutableInBundlesAbsolutePath(QString exeInBundle);
	void internalLaunch(QString executable, QStringList arguments);

	QPointer<QProcess> mProcess;
	ProcessReporterPtr mReporter;
	QString mName;
	QString mLastExecutablePath; //the path to the last executable that was launched
};
}

#endif /* CXPROCESSWRAPPER_H_ */
