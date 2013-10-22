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

#ifndef CXPROCESSWRAPPER_H_
#define CXPROCESSWRAPPER_H_

#include <QObject>
#include "boost/shared_ptr.hpp"
#include <QStringList>
#include <QString>
#include <QProcess>
#include "cxProcessReporter.h"

namespace cx
{

/**
* \file
* \addtogroup cxResourceUtilities
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
class ProcessWrapper : public QObject
{
	Q_OBJECT
public:
	explicit ProcessWrapper(QString name = "executable", QObject* parent = NULL);
	virtual ~ProcessWrapper();

	QProcess* getProcess();

	void launchWithRelativePath(QString executable, QStringList arguments = QStringList());
	void launch(QString executable, QStringList argument = QStringList());

	bool isRunning();

public slots:
	void requestTerminateSlot();

private:
	QString getExecutableInBundlesAbsolutePath(QString exeInBundle);
	void internalLaunch(QString executable, QStringList arguments);

	QProcess* mProcess;
	ProcessReporterPtr mReporter;
	QString mName;
	QString mLastExecutablePath; //the path to the last executable that was launched
};
}

#endif /* CXPROCESSWRAPPER_H_ */
