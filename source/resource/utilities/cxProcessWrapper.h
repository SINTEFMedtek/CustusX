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

namespace cx
{

/**
* \file
* \addtogroup cxResourceUtilities
* @{
*/

typedef boost::shared_ptr<class ProcessWrapper> ProcessWrapperPtr;

/**Wraps a QProcess and performs some common operations.
 *
 *  \date Oct 19, 2012
 *  \author christiana
 */
class ProcessWrapper : public QObject
{
	Q_OBJECT
public:
	explicit ProcessWrapper(QString name = "executable", QObject* parent = NULL);
	virtual ~ProcessWrapper();

	QProcess* getProcess();
	void launch(QString executable, QStringList arguments = QStringList());


private slots:
	void serverProcessReadyRead();
	void serverProcessStateChanged(QProcess::ProcessState newState);
	void serverProcessError(QProcess::ProcessError error);

private:
	QProcess* mProcess;
	QString mName;
};

}

#endif /* CXPROCESSWRAPPER_H_ */
