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


#include "cxApplication.h"
#include "cxLogger.h"
#include <QThread>
#include <QWidget>
#include <QFile>
#include <QAction>

#include <iostream>

#if !defined(WIN32)
#include <langinfo.h>
#include <locale>
#endif

namespace cx
{

Application::Application(int& argc, char** argv) : QApplication(argc, argv)
{
	this->force_C_locale_decimalseparator();

	QFile stylesheet(":/cxStyleSheet.ss");
	stylesheet.open(QIODevice::ReadOnly);
	this->setStyleSheet(stylesheet.readAll());
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

	reportError(QString("Exception caught: [%1]\nin thread [%2]").arg(text).arg(threadName));
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


void Application::force_C_locale_decimalseparator()
{
#if !defined(WIN32)
	QString radixChar = nl_langinfo(RADIXCHAR);
	QString C_radixChar = ".";

	if (radixChar != C_radixChar)
	{
		QLocale::setDefault(QLocale::c());
		setlocale(LC_NUMERIC,"C");

		std::cout << QString("Detected non-standard decimal separator [%1], changing to standard [%2].")
				.arg(radixChar)
				.arg(C_radixChar)
				<< std::endl;

		QString number = QString("%1").arg(0.5);
		if (!number.contains(C_radixChar))
			std::cout << "Failed to set decimal separator." << std::endl;
	}
#endif
}

void bringWindowToFront(QWidget* window)
{
	if (!window->isVisible())
		window->show();

#ifdef __APPLE__ // needed on mac for bringing to front: does the opposite on linux
	window->activateWindow();
#endif
	window->raise();
}

void triggerMainWindowActionWithObjectName(QString actionName)
{
    QWidget *mainwindow = Q_NULLPTR;
    foreach(mainwindow, QApplication::topLevelWidgets()) {
      if(mainwindow->objectName() == "MainWindow")
        break;
    }
    QAction* action = mainwindow->findChild<QAction*>(actionName);
    if(action)
    {
        action->trigger();
    }
}

} // namespace cx
