/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxConsoleWidgetCollection.h"

#include <QDockWidget>
#include <QMenu>
#include <QTimer>
//#include <QApplication>
#include <QDebug>
//#include <QDesktopWidget>
#include <QBuffer>
#include <QDataStream>
#include "cxProfile.h"
#include "cxConsoleWidget.h"
#include "cxNullDeleter.h"

namespace cx
{

ConsoleWidgetCollection::ConsoleWidgetCollection(QWidget* parent, QString objectName, QString windowTitle, XmlOptionFile options, LogPtr log) :
	QMainWindow(parent), mObjectName(objectName), mWindowTitle(windowTitle)
{
	this->setObjectName(mObjectName);
	this->setWindowTitle(mWindowTitle);
	mOptions = options;
	mLog = log;

	this->setupUI();
}

ConsoleWidgetCollection::ConsoleWidgetCollection(QWidget* parent, QString objectName, QString windowTitle) :
	QMainWindow(parent), mObjectName(objectName), mWindowTitle(windowTitle)
{
	this->setObjectName(mObjectName);
	this->setWindowTitle(mWindowTitle);
	mOptions = profile()->getXmlSettings().descend(this->objectName());
	mLog = reporter();

	this->setupUI();
}

void ConsoleWidgetCollection::setupUI()
{
	this->setFocusPolicy(Qt::StrongFocus); // needed for help system: focus is used to display help text

	this->setDockNestingEnabled(true);
	this->setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);

	int consoleCount = this->option("consoleCount").readVariant(-1).toInt();

	for (int i=0; i<consoleCount; ++i)
	{
		this->addConsole();
	}

	// Restore saved window states
	// Must be done after all DockWidgets are created

	this->restoreState(this->option("windowState").readVariant().toByteArray());

	// default: add two consoles
	if (mDockWidgets.empty())
	{
		this->onNewConsole();
		this->onNewConsole();
	}
}

XmlOptionItem ConsoleWidgetCollection::option(QString name)
{
	return XmlOptionItem(name, mOptions.getElement());
}

ConsoleWidgetCollection::~ConsoleWidgetCollection()
{
	this->option("windowState").writeVariant(this->saveState());
	this->option("consoleCount").writeVariant(QVariant::fromValue<int>(mDockWidgets.size()));
}

void ConsoleWidgetCollection::onDockWidgetVisibilityChanged(bool val)
{
	if (!this->isVisible())
	{
//		std::cout << "ignore remove - parent hidden" << std::endl;
		return;
	}

	QTimer::singleShot(0, this, SLOT(checkVisibility()));
}

void ConsoleWidgetCollection::checkVisibility()
{
	this->removeHiddenConsoles();
}

void ConsoleWidgetCollection::removeHiddenConsoles()
{
	for (int i=0; i<mDockWidgets.size(); )
	{
		if (!mDockWidgets[i]->isVisible())
			this->deleteDockWidget(mDockWidgets[i]);
		else
			++i;
	}
}

QDockWidget* ConsoleWidgetCollection::addAsDockWidget(QWidget* widget)
{
	QDockWidget* dockWidget = this->createDockWidget(widget);

	mDockWidgets.push_back(dockWidget);

	connect(dockWidget, &QDockWidget::visibilityChanged,
			this, &ConsoleWidgetCollection::onDockWidgetVisibilityChanged);

	this->addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
	return dockWidget;
}

QDockWidget* ConsoleWidgetCollection::createDockWidget(QWidget* widget)
{
		QDockWidget* dockWidget = new QDockWidget(widget->windowTitle(), this);
		connect(widget, &QWidget::windowTitleChanged, this, &ConsoleWidgetCollection::onConsoleWindowTitleChanged);
		dockWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
		dockWidget->setObjectName(widget->objectName() + "DockWidget");
		dockWidget->setWidget(widget);
		return dockWidget;
}

void ConsoleWidgetCollection::onConsoleWindowTitleChanged(const QString & title)
{
	QWidget* widget = dynamic_cast<QWidget*>(sender());
	QDockWidget* dockWidget = dynamic_cast<QDockWidget*>(sender()->parent());
	if (!widget || !dockWidget)
		return;
	dockWidget->setWindowTitle(widget->windowTitle());
}

void ConsoleWidgetCollection::deleteDockWidget(QDockWidget* dockWidget)
{
	disconnect(dockWidget, &QDockWidget::visibilityChanged,
			this, &ConsoleWidgetCollection::onDockWidgetVisibilityChanged);

	this->removeDockWidget(dockWidget);
	mDockWidgets.removeAll(dockWidget);
	dockWidget->deleteLater();

	for (int i=0; i<mDockWidgets.size(); ++i)
	{
		QString uid = QString("ConsoleWidget%1").arg(i);
		QString dock_uid = uid + "DockWidget";
		mDockWidgets[i]->setObjectName(dock_uid);
		mDockWidgets[i]->widget()->setObjectName(uid);
	}
}

ConsoleWidget* ConsoleWidgetCollection::addConsole()
{
	QString uid = QString("ConsoleWidget%1").arg(mDockWidgets.size());
	ConsoleWidget* console = new ConsoleWidget(this, uid, "Console", mOptions.descend(uid), mLog);
	this->addAsDockWidget(console);
	return console;
}

void ConsoleWidgetCollection::onNewConsole()
{
	ConsoleWidget* console = this->addConsole();
	console->setDetails(true);
}

QMenu* ConsoleWidgetCollection::createPopupMenu()
{
	this->removeHiddenConsoles();

	QMenu* popupMenu = new QMenu(this);

	QAction* addConsoleAction = new QAction("New Console", this);
	connect(addConsoleAction, &QAction::triggered, this, &ConsoleWidgetCollection::onNewConsole);
	popupMenu->addAction(addConsoleAction);

	for (int i=0; i<mDockWidgets.size(); ++i)
	{
		popupMenu->addAction(mDockWidgets[i]->toggleViewAction());
	}

	return popupMenu;
}



} //namespace cx

