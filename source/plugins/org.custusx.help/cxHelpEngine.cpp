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
#include <QDockWidget>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMouseEvent>
#include <QQueue>
#include <QStatusBar>
#include <QTabBar>
#include <QTabWidget>
#include <QToolBar>

//#define DEBUG_HELP_SYSTEM // turn on to output help system focus information

namespace cx
{

HelpEngine::HelpEngine()
{
	QDir().mkpath(profile()->getPath()); // otherwise setupData() fails sometimes
	QString helpFile = profile()->getPath() + "/cx_user_doc.qhc";
	helpEngine = new QHelpEngine(helpFile, NULL);

	if (!helpEngine->setupData())
	{
		// A collection file left over from a previously installed version can
		// reference a cx_user_doc.qch that no longer exists (each Windows
		// install lives in its own version-named folder), leaving the
		// collection file unusable ("Cannot register index tables...").
		// Recreate it from scratch instead of leaving help broken all session.
		reportWarning(QString("HelpEngine: could not open %1 (%2), recreating it")
					  .arg(helpFile).arg(helpEngine->error()));
		delete helpEngine;
		QFile::remove(helpFile);
		helpEngine = new QHelpEngine(helpFile, NULL);
	}

	this->setupDataWithWarning();
	this->setupDocFile();
	this->setupDataWithWarning();

	connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(focusChanged(QWidget*, QWidget*)));
	qApp->installEventFilter(this);

	QTimer::singleShot(100, this, SLOT(setInitialPage()));
}

HelpEngine::~HelpEngine()
{
	if (qApp)
	{
		qApp->removeEventFilter(this);
	}
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
	if (mCurrentKeyword.isEmpty())
	{
		this->activateKeyword("user_doc_overview");
	}
}

QString HelpEngine::getCurrentKeyword() const
{
	return mCurrentKeyword;
}

void HelpEngine::activateKeyword(QString keyword)
{
	// The same keyword again would reload the page and lose the scroll position
	if (!keyword.isEmpty() && keyword != mCurrentKeyword)
	{
#ifdef DEBUG_HELP_SYSTEM
		CX_LOG_CHANNEL_INFO("HELP_DB") << QString("    Found keyword [%1]").arg(keyword);
#endif
		mCurrentKeyword = keyword;
		emit keywordActivated(keyword);
	}
}

bool HelpEngine::hasKeyword(QString id) const
{
	if (!mHasKeywordCache.contains(id))
	{
		mHasKeywordCache[id] = !id.isEmpty() && !helpEngine->linksForKeyword(id).empty();
	}
	return mHasKeywordCache[id];
}

bool HelpEngine::eventFilter(QObject* watched, QEvent* event)
{
	QWidget* widget = qobject_cast<QWidget*>(watched);
	if (widget && event->type() == QEvent::MouseButtonPress)
	{
		this->listenToDockWidgetActivation(widget);
	}
	// A click is sent to the parents too if not accepted: only use the widget under the mouse
	if (widget && event->type() == QEvent::MouseButtonRelease
		&& widget == QApplication::widgetAt(static_cast<QMouseEvent*>(event)->globalPos()))
	{
		this->activateKeyword(findKeywordForClick(widget, [this](QString id) { return this->hasKeyword(id); }));
	}
	return QObject::eventFilter(watched, event);
}

void HelpEngine::listenToDockWidgetActivation(QWidget* widget)
{
	QMainWindow* mainWindow = qobject_cast<QMainWindow*>(widget->window());
	if (mainWindow && !mListenedMainWindows.contains(mainWindow))
	{
		mListenedMainWindows << mainWindow;
		connect(mainWindow, &QMainWindow::tabifiedDockWidgetActivated, this, &HelpEngine::dockWidgetActivated);
	}
}

void HelpEngine::dockWidgetActivated(QDockWidget* dockWidget)
{
	this->activateKeyword(findKeywordInside(dockWidget, [this](QString id) { return this->hasKeyword(id); }));
}

void HelpEngine::focusChanged(QWidget* old, QWidget* now)
{
	// Ignore focus coming back to the application, and focus leaving a help widget that was hidden
	bool leavingHiddenHelp = old && isInsideHelpWidget(old) && !old->isVisible();
	if (old && now && !leavingHiddenHelp && !isInsideBar(now))
	{
#ifdef DEBUG_HELP_SYSTEM
		CX_LOG_CHANNEL_INFO("HELP_DB") << QString("**Focus on [%1]: %2").arg(now->objectName()).arg(now->windowTitle());
#endif
		this->activateKeyword(findKeywordUpwards(now, [this](QString id) { return this->hasKeyword(id); }));
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

bool HelpEngine::isHelpWidget(QObject* object)
{
	return object->objectName().contains("help_widget");
}

bool HelpEngine::isInsideHelpWidget(QObject* object)
{
	bool retval = false;
	for (; object && !retval; object = object->parent())
	{
		retval = isHelpWidget(object);
	}
	return retval;
}

QString HelpEngine::findKeywordUpwards(QObject* object, HasKeywordFunction hasKeyword)
{
	QString retval;
	if (!isInsideHelpWidget(object))
	{
		for (; object && retval.isEmpty(); object = object->parent())
		{
#ifdef DEBUG_HELP_SYSTEM
			CX_LOG_CHANNEL_DEBUG("HELP_DB") << QString("    examining [%1](%2)")
											   .arg(object->objectName())
											   .arg(object->metaObject()->className());
#endif
			if (hasKeyword(object->objectName()))
			{
				retval = object->objectName();
			}
		}
	}
	return retval;
}

QString HelpEngine::findKeywordInside(QWidget* root, HasKeywordFunction hasKeyword)
{
	QString retval;
	if (root && !isInsideHelpWidget(root))
	{
		retval = findKeywordInChildren(root, hasKeyword);
		if (retval.isEmpty())
		{
			retval = findKeywordUpwards(root->parent(), hasKeyword);
		}
	}
	return retval;
}

QString HelpEngine::findKeywordInChildren(QWidget* root, HasKeywordFunction hasKeyword)
{
	QString retval;
	QQueue<QWidget*> queue;
	queue.enqueue(root);
	while (!queue.isEmpty() && retval.isEmpty())
	{
		QWidget* widget = queue.dequeue();
		if (hasKeyword(widget->objectName()))
		{
			retval = widget->objectName();
		}
		foreach (QWidget* child, widget->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly))
		{
			// skip the pages of a tab or stacked widget that are not shown
			if (!child->isHidden() && !child->isWindow() && !isHelpWidget(child))
			{
				queue.enqueue(child);
			}
		}
	}
	return retval;
}

QString HelpEngine::findKeywordForClick(QWidget* widget, HasKeywordFunction hasKeyword)
{
	QString retval;
	QTabBar* tabBar = qobject_cast<QTabBar*>(widget);
	QTabWidget* tabWidget = tabBar ? qobject_cast<QTabWidget*>(tabBar->parentWidget()) : NULL;
	QMainWindow* dockTabsOwner = tabBar ? qobject_cast<QMainWindow*>(tabBar->parentWidget()) : NULL;
	if (isInsideBar(widget))
	{
		retval = "";
	}
	else if (tabWidget && tabWidget->currentWidget())
	{
		retval = findKeywordInside(tabWidget->currentWidget(), hasKeyword);
	}
	else if (dockTabsOwner && tabBar->currentIndex() >= 0)
	{
		QPoint tabBarCenter = tabBar->mapTo(dockTabsOwner, tabBar->rect().center());
		retval = findKeywordInside(findDockWidget(dockTabsOwner, tabBar->tabText(tabBar->currentIndex()), tabBarCenter), hasKeyword);
	}
	else if (qobject_cast<QDockWidget*>(widget))
	{
		retval = findKeywordInside(widget, hasKeyword);
	}
	else
	{
		retval = findKeywordUpwards(widget, hasKeyword);
	}
	return retval;
}

bool HelpEngine::isInsideBar(QObject* object)
{
	bool retval = false;
	for (; object && !retval; object = object->parent())
	{
		retval = qobject_cast<QToolBar*>(object) || qobject_cast<QMenuBar*>(object)
				|| qobject_cast<QMenu*>(object) || qobject_cast<QStatusBar*>(object);
	}
	return retval;
}

QDockWidget* HelpEngine::findDockWidget(QMainWindow* mainWindow, QString title, QPoint near)
{
	// Prefer the dock widget shown as the current tab, then the one closest to the tab bar
	const int kNotShownPenalty = 1000000;
	QDockWidget* retval = NULL;
	int bestDistance = 0;
	foreach (QDockWidget* dockWidget, mainWindow->findChildren<QDockWidget*>())
	{
		if (!dockWidget->isHidden() && dockWidget->windowTitle() == title)
		{
			int distance = (dockWidget->geometry().center() - near).manhattanLength();
			if (dockWidget->visibleRegion().isEmpty())
			{
				distance += kNotShownPenalty;
			}
			if (!retval || distance < bestDistance)
			{
				retval = dockWidget;
				bestDistance = distance;
			}
		}
	}
	return retval;
}

}
