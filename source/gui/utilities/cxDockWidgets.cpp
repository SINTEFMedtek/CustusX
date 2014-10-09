#include "cxDockWidgets.h"
#include <QDockWidget>
#include <QMainWindow>
#include <QScrollArea>

namespace cx
{

DockWidgets::DockWidgets(QMainWindow* parent) :
	mParent(parent)
{

}

QDockWidget* DockWidgets::addAsDockWidget(QWidget* widget, QString groupname)
{
	QDockWidget* dockWidget = this->createDockWidget(widget);
//	QMainWindow::addDockWidget(Qt::LeftDockWidgetArea, dockWidget);

	this->tabifyWidget(dockWidget);

	mDockWidgets.insert(dockWidget);
	dockWidget->setVisible(false); // default visibility
//	this->restoreDockWidget(dockWidget); // restore if added after construction

	//    this->addToWidgetGroupMap(dockWidget->toggleViewAction(), groupname);
	return dockWidget;
}

QDockWidget* DockWidgets::createDockWidget(QWidget* widget)
{
		QScrollArea* scroller = this->addVerticalScroller(widget);
		QDockWidget* dockWidget = new QDockWidget(widget->windowTitle(), mParent);
		dockWidget->setObjectName(widget->objectName() + "DockWidget");
		dockWidget->setFocusPolicy(Qt::StrongFocus); // we really want to focus on the embedded widget, see focusInsideDockWidget()
		dockWidget->setWidget(scroller);
		return dockWidget;
}

QScrollArea* DockWidgets::addVerticalScroller(QWidget *widget)
{
		QScrollArea* scroller = new QScrollArea(NULL);
		scroller->setWidget(widget);
		scroller->setWidgetResizable(true);
		scroller->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		QSizePolicy policy = scroller->sizePolicy();
		policy.setHorizontalPolicy(QSizePolicy::Minimum);
		scroller->setSizePolicy(policy);
		return scroller;
}

void DockWidgets::tabifyWidget(QDockWidget* dockWidget)
{
		for (std::set<QDockWidget*>::iterator iter = mDockWidgets.begin(); iter != mDockWidgets.end(); ++iter)
		{
				if (mParent->dockWidgetArea(*iter) == Qt::LeftDockWidgetArea)
				{
						mParent->tabifyDockWidget(*iter, dockWidget);
						break;
				}
		}
}

void DockWidgets::erase(QDockWidget* dockWidget)
{
		this->mDockWidgets.erase(dockWidget);
}

void DockWidgets::toggleDebug(QAction* action, bool checked)
{
	for (std::set<QDockWidget*>::iterator iter = mDockWidgets.begin(); iter != mDockWidgets.end(); ++iter)
	{
		if (action == (*iter)->toggleViewAction())
		{
			if (!checked)
				(*iter)->hide();
		}
	}
}

void DockWidgets::hideAll()
{
	for (std::set<QDockWidget*>::iterator iter = mDockWidgets.begin(); iter != mDockWidgets.end(); ++iter)
		(*iter)->hide();
}

} // namespace cx
