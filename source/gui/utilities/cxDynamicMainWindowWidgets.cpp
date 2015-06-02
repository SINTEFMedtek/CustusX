#include "cxDynamicMainWindowWidgets.h"
#include <QDockWidget>
#include <QMainWindow>
#include <QScrollArea>
#include <QToolBar>
#include <QMenu>
#include <iostream>
#include "cxLogger.h"
#include "cxStateService.h"

namespace cx
{

DynamicMainWindowWidgets::DynamicMainWindowWidgets(QMainWindow* mainWindow) :
	mMainWindow(mainWindow)
{
	// use dummy toolbar to force default preset toolbars to the left
	mFirstDummyToolbar = new QToolBar;
	mFirstDummyToolbar->setObjectName("dummy_toolbar");
//	mFirstDummyToolbar->hide();
	mMainWindow->addToolBar(mFirstDummyToolbar);
}

QDockWidget* DynamicMainWindowWidgets::addAsDockWidget(QWidget* widget, QString groupname, QObject* owningPlugin)
{
	QDockWidget* dockWidget = this->createDockWidget(widget);
	mMainWindow->addDockWidget(Qt::LeftDockWidgetArea, dockWidget);

	this->tabifyWidget(dockWidget);

	DynamicWidget item;
	item.mWidget = dockWidget;
	dockWidget->setVisible(false); // default visibility

	item.mGroup = groupname;
	item.mName = widget->windowTitle();
	mMainWindow->restoreDockWidget(dockWidget); // restore if added after construction

	item.mOwningService = owningPlugin;

	connect(dockWidget->toggleViewAction(), &QAction::triggered, this, &DynamicMainWindowWidgets::onWidgetActionTriggered);

	mItems.push_back(item);
	return dockWidget;
}

void DynamicMainWindowWidgets::registerToolBar(QToolBar* toolbar, QString groupname)
{
	DynamicWidget item;
	item.mWidget = toolbar;
	item.mGroup = groupname;
	item.mName = toolbar->windowTitle();
	mItems.push_back(item);

	mMainWindow->addToolBar(toolbar);
	// this avoids overpopulation of gui at startup, and is the same functionality as for dockwidgets.
	// also gives correct size of mainwindow at startup.
	toolbar->hide();
}

QDockWidget* DynamicMainWindowWidgets::createDockWidget(QWidget* widget)
{
	QScrollArea* scroller = this->addVerticalScroller(widget);
	QDockWidget* dockWidget = new QDockWidget(widget->windowTitle(), mMainWindow);
	connect(widget, &QWidget::windowTitleChanged, this, &DynamicMainWindowWidgets::onConsoleWindowTitleChanged);
	dockWidget->setObjectName(widget->objectName() + "DockWidget");
	dockWidget->setFocusPolicy(Qt::StrongFocus); // we really want to focus on the embedded widget, see focusInsideDockWidget()
	dockWidget->setWidget(scroller);
	return dockWidget;
}

void DynamicMainWindowWidgets::showWidget(QString name)
{
	for (unsigned i=0; i<mItems.size(); ++i)
	{
		DynamicWidget item = mItems[i];

		if (item.mName != name)
		{
			continue;
		}

		QDockWidget* dockWidget = dynamic_cast<QDockWidget*>(item.mWidget);
		if (!dockWidget)
			continue;

		dockWidget->show();
		dockWidget->raise();
	}
}

void DynamicMainWindowWidgets::restoreFrom(const Desktop& desktop)
{
	this->hideAll();
	mMainWindow->restoreState(desktop.mMainWindowState);

	for (unsigned i=0; i<desktop.mPresets.size(); ++i)
		this->restorePreset(desktop.mPresets[i]);
}

void DynamicMainWindowWidgets::restorePreset(const Desktop::Preset& preset)
{
	QToolBar* tb = mMainWindow->findChild<QToolBar*>(preset.name);
	if (tb)
	{
		// add before dummy
		mMainWindow->removeToolBar(tb);
		mMainWindow->insertToolBar(mFirstDummyToolbar, tb);
		tb->show();
		return;
	}

	QDockWidget* dw = mMainWindow->findChild<QDockWidget*>(preset.name+"DockWidget");
	if (dw)
	{
		dw->show();
		if (preset.tabbed)
		{
			QDockWidget* first = this->getFirstDockWidget(Qt::DockWidgetArea(preset.position));
			if (first)
				mMainWindow->tabifyDockWidget(first, dw);
			else
				mMainWindow->addDockWidget(Qt::DockWidgetArea(preset.position), dw);
		}
		else
		{
			mMainWindow->addDockWidget(Qt::DockWidgetArea(preset.position), dw);
		}
		return;
	}

	CX_LOG_WARNING() << QString("Attempted to restore a nonexitent preset widget: [%1]").arg(preset.name);
}

void DynamicMainWindowWidgets::onWidgetActionTriggered(bool checked)
{
	QAction* action = dynamic_cast<QAction*>(sender());
	if (!action)
		return;
	QWidget* widget = dynamic_cast<QWidget*>(action->parent());
	if (!widget)
		return;

	// if dockwidgets is tabbed, this ensures that the widget is shown instead
	// of merely showing the tab.
	if (checked)
		widget->raise();
}

void DynamicMainWindowWidgets::owningServiceRemoved(QObject* service)
{
	for (unsigned i=0; i<mItems.size(); )
	{
		DynamicWidget item = mItems[i];

		if (item.mOwningService != service)
		{
			++i;
			continue;
		}
		else
		{
			mItems.erase(mItems.begin()+i);

			QDockWidget* dockWidget = dynamic_cast<QDockWidget*>(item.mWidget);
			if (dockWidget)
				mMainWindow->removeDockWidget(dockWidget);
		}
	}
}

void DynamicMainWindowWidgets::onConsoleWindowTitleChanged(const QString & title)
{
	QWidget* widget = dynamic_cast<QWidget*>(sender());

	for (int i=0; i<3; ++i)
	{
		if (!widget)
			return;
		QDockWidget* dockWidget = dynamic_cast<QDockWidget*>(widget);
		if (dockWidget)
		{
			dockWidget->setWindowTitle(title);
			return;
		}
		widget = widget->parentWidget();
	}
}

QScrollArea* DynamicMainWindowWidgets::addVerticalScroller(QWidget *widget)
{
	QScrollArea* scroller = new QScrollArea(NULL);
	scroller->setWidget(widget);
	widget->setParent(scroller);
	scroller->setWidgetResizable(true);
	scroller->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	QSizePolicy policy = scroller->sizePolicy();
	policy.setHorizontalPolicy(QSizePolicy::Minimum);
	scroller->setSizePolicy(policy);
	return scroller;
}

void DynamicMainWindowWidgets::tabifyWidget(QDockWidget* dockWidget)
{
	QDockWidget* widget = this->getFirstDockWidget(Qt::LeftDockWidgetArea);

	if (widget)
		mMainWindow->tabifyDockWidget(widget, dockWidget);
}

QDockWidget* DynamicMainWindowWidgets::getFirstDockWidget(Qt::DockWidgetArea area)
{
	for (unsigned i=0; i<mItems.size(); ++i)
	{
		QDockWidget* widget = dynamic_cast<QDockWidget*>(mItems[i].mWidget);
		if (!widget)
			continue;
		if (mMainWindow->dockWidgetArea(widget) == Qt::LeftDockWidgetArea)
			return widget;
	}
	return NULL;
}

void DynamicMainWindowWidgets::hideAll()
{
	for (unsigned i=0; i<mItems.size(); ++i)
	{
		mItems[i].mWidget->hide();

		QToolBar* toolbar = dynamic_cast<QToolBar*>(mItems[i].mWidget);
		if (toolbar)
		{
			// move to end of list (i.e. after dummy)
			mMainWindow->removeToolBar(toolbar);
			mMainWindow->addToolBar(toolbar);
		}
	}

}

void DynamicMainWindowWidgets::addToWidgetGroupMap(ActionGroupMap& groups, QAction* action, QString groupname)
{
	action->setMenuRole(QAction::NoRole);

	if (!groups.count(groupname))
	{
		QActionGroup* group = new QActionGroup(this);
		group->setExclusive(false);
		groups[groupname] = group;

		QAction* heading = new QAction(groupname, this);
		heading->setDisabled(true);
		groups[groupname]->addAction(heading);
	}

	groups[groupname]->addAction(action);
}

QMenu* DynamicMainWindowWidgets::createPopupMenu()
{
	// temp attempt: split menu into two parts: widgets and toolbars. - fix

	QMenu* popupMenu = new QMenu;

	ActionGroupMap groups;
	ActionGroupMap tgroups;
	for (unsigned i=0; i<mItems.size(); ++i)
	{
		QDockWidget* widget = dynamic_cast<QDockWidget*>(mItems[i].mWidget);
		if (widget)
			this->addToWidgetGroupMap(groups, widget->toggleViewAction(), mItems[i].mGroup);
		QToolBar* toolbar = dynamic_cast<QToolBar*>(mItems[i].mWidget);
		if (toolbar)
			this->addToWidgetGroupMap(tgroups, toolbar->toggleViewAction(), mItems[i].mGroup);
	}

	QMenu* widgets = popupMenu->addMenu("Widgets");

	for(std::map<QString, QActionGroup*>::iterator it = groups.begin(); it != groups.end(); ++it)
	{
		widgets->addSeparator();
		widgets->addActions(it->second->actions());
	}

	QMenu* toolbars = popupMenu->addMenu("Toolbars");

	for(std::map<QString, QActionGroup*>::iterator it = tgroups.begin(); it != tgroups.end(); ++it)
	{
		toolbars->addSeparator();
		toolbars->addActions(it->second->actions());
	}

	return popupMenu;
}

} // namespace cx
