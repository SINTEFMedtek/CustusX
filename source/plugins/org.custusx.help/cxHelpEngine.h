/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXHELPENGINE_H
#define CXHELPENGINE_H

#include <functional>
#include <map>
#include <memory>
#include "org_custusx_help_Export.h"

#include <QString>
#include <QObject>
#include <QHash>
#include <QList>
#include <QPoint>
#include <QPointer>

class QWidget;
class QDockWidget;
class QMainWindow;
class QEvent;
class QHelpEngineCore;
class QHelpEngine;
class QHelpSearchEngine;

namespace cx
{

/**
 * Core functionality and shared resource for help plugin.
 *
 * \ingroup org_custusx_help
 *
 * \date 2014-09-30
 * \author Christian Askeland
 */
class org_custusx_help_EXPORT HelpEngine : public QObject
{
	Q_OBJECT
public:
	HelpEngine();
	~HelpEngine();
	QHelpEngine* engine() { return helpEngine; }
	QString getCurrentKeyword() const; ///< The keyword last activated, empty if none

	typedef std::function<bool(QString)> HasKeywordFunction;
	/** Keyword for a focused or clicked object: the object or its nearest parent with a help page. */
	static QString findKeywordUpwards(QObject* object, HasKeywordFunction hasKeyword);
	/** Keyword for a dock widget or tab page: the root, the first shown widget inside it, or the nearest parent with a help page. */
	static QString findKeywordInside(QWidget* root, HasKeywordFunction hasKeyword);
	/** Keyword for a clicked widget. A clicked tab or dock widget title gives the keyword of its content.
	 *  Clicks on tool bars, menus and the status bar give no keyword. */
	static QString findKeywordForClick(QWidget* widget, HasKeywordFunction hasKeyword);
	/** The shown dock widget with the given title, as in its tab, or NULL.
	 *  With several, the one shown as the current tab and closest to near (main window coordinates). */
	static QDockWidget* findDockWidget(QMainWindow* mainWindow, QString title, QPoint near = QPoint());
signals:
	void keywordActivated(QString);
protected:
	bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
	void dockWidgetActivated(QDockWidget* dockWidget);
	void focusChanged(QWidget * old, QWidget * now);
	void setInitialPage();
private:
	static bool isHelpWidget(QObject* object);
	static bool isInsideHelpWidget(QObject* object);
	static bool isInsideBar(QObject* object);
	static QString findKeywordInChildren(QWidget* root, HasKeywordFunction hasKeyword);
	bool hasKeyword(QString id) const;
	void activateKeyword(QString keyword);
	void listenToDockWidgetActivation(QWidget* widget);
	QHelpEngine* helpEngine;
	QString mCurrentKeyword;
	mutable QHash<QString, bool> mHasKeywordCache;
	QList<QPointer<QMainWindow> > mListenedMainWindows;
	void setupDataWithWarning();

	bool isBreakChar(QChar c) const;
	bool isBreakChar(QString text, int index) const;

	void setupDocFile();
};
typedef std::shared_ptr<HelpEngine> HelpEnginePtr;

}

#endif // CXHELPENGINE_H
