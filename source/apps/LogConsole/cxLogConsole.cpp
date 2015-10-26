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
