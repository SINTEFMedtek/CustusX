/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXCONSOLEWIDGETCOLLECTION_H
#define CXCONSOLEWIDGETCOLLECTION_H

#include <QMainWindow>
#include "cxXmlOptionItem.h"
#include <boost/shared_ptr.hpp>
class QMenu;

#include "cxResourceWidgetsExport.h"

namespace cx
{
typedef boost::shared_ptr<class Log> LogPtr;

class cxResourceWidgets_EXPORT ConsoleWidgetCollection : public QMainWindow
{
	Q_OBJECT
public:
	ConsoleWidgetCollection(QWidget* parent, QString objectName, QString windowTitle, XmlOptionFile options, LogPtr log);
	ConsoleWidgetCollection(QWidget* parent, QString objectName, QString windowTitle);
	virtual ~ConsoleWidgetCollection();
private slots:
	void onDockWidgetVisibilityChanged(bool val);
	void onNewConsole();
	void checkVisibility();
	void onConsoleWindowTitleChanged(const QString & title);
protected:
	virtual QMenu* createPopupMenu();
private:
	QDockWidget* addAsDockWidget(QWidget* widget);
	QDockWidget* createDockWidget(QWidget* widget);
	void deleteDockWidget(QDockWidget* dockWidget);
	void removeHiddenConsoles();
	class ConsoleWidget* addConsole();

	QString mObjectName;
	QString mWindowTitle;
	XmlOptionFile mOptions;
	LogPtr mLog;
	XmlOptionItem option(QString name);
	void setupUI();

	QList<QDockWidget*> mDockWidgets;
};

} //namespace cx


#endif // CXCONSOLEWIDGETCOLLECTION_H
