// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include "sscApplication.h"
#include "sscMessageManager.h"
#include <QThread>

namespace ssc
{

Application::Application(int& argc, char** argv) : QApplication(argc, argv)
{
}

void Application::reportException(QString text)
{
	QThread* main = this->thread();
	QThread* current = QThread::currentThread();
	QString threadName = current->objectName();
	if (threadName.isEmpty())
	{
		if (main==current)
			threadName = "MAIN";
		else
			threadName = "SECONDARY";
	}

	ssc::messageManager()->sendError(QString("Exception caught: [%1]\nin thread [%2]").arg(text).arg(threadName));
}

bool Application::notify(QObject *rec, QEvent *ev)
{
	try
	{
		return QApplication::notify(rec, ev);
	}
	catch(std::exception& e)
	{
		this->reportException(e.what());
		exit(0);
	}
	catch( ... )
	{
		this->reportException("Unknown");
		exit(0);
	}
	return false;
}

} // namespace ssc
