	/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
