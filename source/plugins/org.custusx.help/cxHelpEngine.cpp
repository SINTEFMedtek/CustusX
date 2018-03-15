/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxHelpEngine.h"

#include <QHelpEngine>
#include "cxDataLocations.h"
#include <iostream>
#include "cxTypeConversions.h"
#include <QFileInfo>
#include <QApplication>
#include <QWidget>
#include "cxLogger.h"
#include "cxProfile.h"
#include <QDir>
#include <QTimer>

//#define DEBUG_HELP_SYSTEM // turn on to output help system focus information

namespace cx
{

HelpEngine::HelpEngine()
{
	QDir().mkpath(profile()->getPath()); // otherwise setupData() fails sometimes
	QString helpFile = profile()->getPath() + "/cx_user_doc.qhc";
	helpEngine = new QHelpEngine(helpFile, NULL);

	this->setupDataWithWarning();
	this->setupDocFile();
	this->setupDataWithWarning();

	connect(qApp, SIGNAL(focusObjectChanged(QObject*)), this, SLOT(focusObjectChanged(QObject*)));
	connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(focusChanged(QWidget*, QWidget*)));

	QTimer::singleShot(100, this, SLOT(setInitialPage()));
}

HelpEngine::~HelpEngine()
{
	delete helpEngine;
}

void HelpEngine::setupDocFile()
{
	QString docFile = DataLocations::getDocPath()+"/cx_user_doc.qch";
	if(!QFile(docFile).exists())
		reportWarning(QString("HelpEngine: Cannot find docFile: %1").arg(docFile));
	helpEngine->unregisterDocumentation(helpEngine->namespaceName(docFile));
	if(!helpEngine->registerDocumentation(docFile))
		reportWarning(QString("HelpEngine: Documentation registration failed: %1").arg(helpEngine->error()));
}

void HelpEngine::setupDataWithWarning()
{
	bool success = helpEngine->setupData();
	// had problems here before mkdir was called on the qhc path
	if (!success)
		CX_LOG_WARNING() << QString("Help engine setup failed with error [%1]").arg(helpEngine->error());
}

void HelpEngine::setInitialPage()
{
	emit keywordActivated("user_doc_overview");
}

void HelpEngine::focusChanged(QWidget * old, QWidget * now)
{
	if (!now)
		return;
}

void HelpEngine::focusObjectChanged(QObject* newFocus)
{
	if (!newFocus)
		return;
#ifdef DEBUG_HELP_SYSTEM
	CX_LOG_CHANNEL_INFO("HELP_DB") << QString("**Focus on [%1]: %2").arg(newFocus->objectName()).arg(dynamic_cast<QWidget*>(newFocus)->windowTitle());
#endif
	QString keyword = this->findBestMatchingKeyword(newFocus);
	if (!keyword.isEmpty())
	{
#ifdef DEBUG_HELP_SYSTEM
		CX_LOG_CHANNEL_INFO("HELP_DB") << QString("    Found keyword [%1]").arg(keyword);
#endif
		emit keywordActivated(keyword);
	}
}

bool HelpEngine::isBreakChar(QChar c) const
{
	return c.isDigit() || c.isUpper();
}

bool HelpEngine::isBreakChar(QString text, int index) const
{
	if (!this->isBreakChar(text[index]))
		return false;

	bool prev = true;
	if (index>0)
		prev = this->isBreakChar(text[index-1]);

	bool next = true;
	if (index+1<text.size())
		next = this->isBreakChar(text[index+1]);

	if (!prev || !next)
		return true;

	return false;
}

QString HelpEngine::findBestMatchingKeyword(QObject* object)
{
	while (object)
	{
		QString id = object->objectName();
#ifdef DEBUG_HELP_SYSTEM
		CX_LOG_CHANNEL_DEBUG("HELP_DB") << QString("    examining [%1](%2)")
										   .arg(id)
										   .arg(object->metaObject()->className());
#endif
		if (id.contains("help_widget"))
			return "";

		QMap<QString, QUrl> links = this->engine()->linksForIdentifier(id);
		if (!links.empty())
		{
			return id;
		}

		object = object->parent();
	}
	return "";
}


}
