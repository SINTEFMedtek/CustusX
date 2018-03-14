/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxLogConsole.h"

#include <QDesktopWidget>
#include <QApplication>
#include <QFileDialog>
#include <QAction>
#include <QMenu>
#include <QMenuBar>

#include "cxConsoleWidgetCollection.h"
#include "cxDataLocations.h"
#include "cxLogFileWatcher.h"

namespace cx
{

LogConsole::LogConsole()
{
	QString optionsFile = cx::DataLocations::getPersistentWritablePath()+"/logconsole.xml";
	mOptions = cx::XmlOptionFile(optionsFile);
	CX_LOG_INFO() << "Options file: " << optionsFile;
	mLog = cx::LogFileWatcher::create();
	CX_LOG_INFO() << "Log folder: " << mLog->getLoggingFolder();

	this->createMenus();

	this->updateWindowTitle();

	cx::ConsoleWidgetCollection* consoles = new cx::ConsoleWidgetCollection(NULL, "ConsoleWidgets", "Consoles", mOptions, mLog);
	this->setCentralWidget(consoles);

	this->initializeGeometry();
}

void LogConsole::initializeGeometry()
{
	if (!this->restoreGeometry(this->getGeometryOption().readVariant().toByteArray()))
		this->setDefaultGeometry();
}

void LogConsole::setDefaultGeometry()
{
	QDesktopWidget* desktop = dynamic_cast<QApplication*>(QApplication::instance())->desktop();
	QRect screen = desktop->screenGeometry(desktop->primaryScreen());
	screen.adjust(screen.width()*0.5, screen.height()*0.25, 0, 0);
	this->setGeometry(screen);
}

XmlOptionItem LogConsole::getGeometryOption()
{
	return XmlOptionItem("geometry", mOptions.getElement());
}

LogConsole::~LogConsole()
{
	this->getGeometryOption().writeVariant(this->saveGeometry());

	mOptions.save();
}

void LogConsole::createMenus()
{
	//data
	QAction* selectFolderAction = new QAction(QIcon(":/icons/open_icon_library/document-import-2.png"), tr("&Select folder"), this);
	selectFolderAction->setShortcut(tr("Ctrl+L"));
	selectFolderAction->setStatusTip(tr("Select folder"));
	connect(selectFolderAction, &QAction::triggered, this, &LogConsole::onSelectFolder);

	QMenu* fileMenu = new QMenu(tr("File"), this);
	fileMenu->addAction(selectFolderAction);
	this->menuBar()->addMenu(fileMenu);
}

void LogConsole::updateWindowTitle()
{
	QString title = QString("Log Console [%1]").arg(mLog->getLoggingFolder());
	this->setWindowTitle(title);
}

void LogConsole::onSelectFolder()
{
	QString folder = mLog->getLoggingFolder() + "/..";

	folder = QFileDialog::getExistingDirectory(this, "Open log folder", folder);
	if (folder.isEmpty())
	{
		return;
	}

	mLog->setLoggingFolder(folder);
	this->updateWindowTitle();
}


} // namespace cx
