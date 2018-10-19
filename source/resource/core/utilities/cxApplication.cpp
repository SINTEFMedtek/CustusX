/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxApplication.h"
#include "cxLogger.h"
#include <QThread>
#include <QWidget>
#include <QFile>
#include <QAction>
#include <QDockWidget>

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

QWidget* getMainWindow()
{
	QWidget *mainwindow = Q_NULLPTR;
	foreach(mainwindow, QApplication::topLevelWidgets()) {
	  if(mainwindow->objectName() == "main_window")
		break;
	}
	return mainwindow;
}

template<typename T>
T findMainWindowChildWithObjectName(QString objectName)
{
	QWidget *mainwindow = getMainWindow();
	T object = mainwindow->findChild<T>(objectName);
	return object;
}

void triggerMainWindowActionWithObjectName(QString actionName)
{
	QAction* action = findMainWindowChildWithObjectName<QAction*>(actionName);
    if(action)
    {
        action->trigger();
    }
}

template
cxResource_EXPORT QWidget* findMainWindowChildWithObjectName(QString objectName);

template
cxResource_EXPORT QDockWidget* findMainWindowChildWithObjectName(QString objectName);

} // namespace cx
