// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxLayoutInteractor.h"
#include "cxLayoutEditorWidget.h"
#include "cxViewManager.h"
#include <QMenu>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QMetaMethod>
#include "boost/bind.hpp"
#include "libQtSignalAdapters/Qt2Func.h"
#include "libQtSignalAdapters/ConnectionFactories.h"

namespace cx
{

LayoutInteractor::LayoutInteractor(QObject* parent) : QObject(parent)
{
	mSecondaryLayoutActionGroup = NULL;
	mLayoutActionGroup = NULL;
	this->createActions();
	connect(viewManager(), SIGNAL(activeLayoutChanged()), this, SLOT(layoutChangedSlot()));
}

void LayoutInteractor::createActions()
{
	mNewLayoutAction = new QAction(tr("New Layout"), this);
	mNewLayoutAction->setToolTip("Create a new Custom Layout");
	connect(mNewLayoutAction, SIGNAL(triggered()), this, SLOT(newCustomLayoutSlot()));
	mEditLayoutAction = new QAction(tr("Edit Layout"), this);
	mEditLayoutAction->setToolTip("Edit the current Custom Layout");
	connect(mEditLayoutAction, SIGNAL(triggered()), this, SLOT(editCustomLayoutSlot()));
	mDeleteLayoutAction = new QAction(tr("Delete Layout"), this);
	mDeleteLayoutAction->setToolTip("Delete the current Custom Layout");
	connect(mDeleteLayoutAction, SIGNAL(triggered()), this, SLOT(deleteCustomLayoutSlot()));
}

void LayoutInteractor::connectToMenu(QMenu* menu)
{
	mMenu = menu;

	mMenu->addAction(mNewLayoutAction);
	mMenu->addAction(mEditLayoutAction);
	mMenu->addAction(mDeleteLayoutAction);

	mSecondaryLayoutMenu = new QMenu("Secondary Layout", mMenu);
	mMenu->addMenu(mSecondaryLayoutMenu);

	mMenu->addSeparator();

	this->layoutChangedSlot();
}


void LayoutInteractor::newCustomLayoutSlot()
{
	LayoutData data = this->executeLayoutEditorDialog("New Custom Layout", true);
	if (data.getUid().isEmpty())
		return;
	viewManager()->setLayoutData(data);
	viewManager()->setActiveLayout(data.getUid());
}

void LayoutInteractor::editCustomLayoutSlot()
{
	LayoutData data = this->executeLayoutEditorDialog("Edit Current Layout", false);
	if (data.getUid().isEmpty())
		return;
	viewManager()->setLayoutData(data);
}

void LayoutInteractor::deleteCustomLayoutSlot()
{
	if (QMessageBox::question(NULL, "Delete current layout", "Do you really want to delete the current layout?",
		QMessageBox::Cancel | QMessageBox::Ok) != QMessageBox::Ok)
		return;
	viewManager()->deleteLayoutData(viewManager()->getActiveLayout());
	viewManager()->setActiveLayout(viewManager()->getAvailableLayouts().front()); // revert to existing state
}

/** Called when the layout is changed: update the layout menu
 */
void LayoutInteractor::layoutChangedSlot()
{
	if (!mMenu)
		return;
	// reset list of available layouts

	this->deepDeleteActionGroup(mLayoutActionGroup);
	mLayoutActionGroup = this->createLayoutActionGroup(0);
	mMenu->addActions(mLayoutActionGroup->actions());

	this->deepDeleteActionGroup(mSecondaryLayoutActionGroup);
	mSecondaryLayoutActionGroup = this->createLayoutActionGroup(1);
	mSecondaryLayoutMenu->addActions(mSecondaryLayoutActionGroup->actions());

	bool editable = viewManager()->isCustomLayout(viewManager()->getActiveLayout());
	mEditLayoutAction->setEnabled(editable);
	mDeleteLayoutAction->setEnabled(editable);
}

void LayoutInteractor::deepDeleteActionGroup(QActionGroup* actionGroup)
{
	//Make sure all actions in the group are deleted - possibly removes a few memory leaks
	if (actionGroup)
	{
		QList<QAction*> actionList = actionGroup->actions();
		for (int i = 0; i < actionList.size(); i++)
			delete actionList.at(i);
	}

	delete actionGroup;
}

/**create and execute a dialog for determining layout.
 * Return layout data, or invalid layout data if cancelled.
 */
LayoutData LayoutInteractor::executeLayoutEditorDialog(QString title, bool createNew)
{
	boost::shared_ptr<QDialog> dialog(new QDialog(NULL, Qt::Dialog));
	dialog->setWindowTitle(title);
	QVBoxLayout* layout = new QVBoxLayout(dialog.get());
	layout->setMargin(0);

	LayoutEditorWidget* editor = new LayoutEditorWidget(dialog.get());

	LayoutData data = viewManager()->getLayoutData(viewManager()->getActiveLayout());

	if (createNew)
	{
		data.resetUid(viewManager()->generateLayoutUid());
	}
	editor->setLayoutData(data);
	layout->addWidget(editor);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), dialog.get(), SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), dialog.get(), SLOT(reject()));
	layout->addWidget(buttonBox);

	if (!dialog->exec())
		return LayoutData();

	return editor->getLayoutData();
}

QActionGroup* LayoutInteractor::createLayoutActionGroup(int widgetIndex)
{
	QActionGroup* retval = new QActionGroup(this);
	retval->setExclusive(true);

	// add default layouts
	std::vector<QString> layouts = viewManager()->getAvailableLayouts();
	int defaultLayouts = 0;
	for (unsigned i = 0; i < layouts.size(); ++i)
	{
		if (!viewManager()->isCustomLayout(layouts[i]))
		{
			this->addLayoutAction(layouts[i], retval, widgetIndex);
			defaultLayouts++;
		}
	}

	// add separator
	QAction* sep = new QAction(retval);
	sep->setSeparator(this);
	//retval->addAction(sep);

	if (defaultLayouts != layouts.size())
	{
		QAction* action = new QAction("Custom", retval);
		action->setEnabled(false);
	}

	// add custom layouts
	for (unsigned i = 0; i < layouts.size(); ++i)
	{
		if (viewManager()->isCustomLayout(layouts[i]))
			this->addLayoutAction(layouts[i], retval, widgetIndex);
	}

	// set checked status
	QString type = viewManager()->getActiveLayout(widgetIndex);
	QList<QAction*> actions = retval->actions();
	for (int i = 0; i < actions.size(); ++i)
	{
		if (actions[i]->data().toString() == type)
			actions[i]->setChecked(true);
	}

	return retval;
}

/** Add one layout as an action to the layout menu.
 */
QAction* LayoutInteractor::addLayoutAction(QString layout, QActionGroup* group, int widgetIndex)
{
	LayoutData data = viewManager()->getLayoutData(layout);
	if (data.isEmpty())
	{
		QAction* sep = new QAction(group);
		sep->setSeparator(this);
	}
	QAction* action = new QAction(data.getName(), group);
	action->setEnabled(!data.isEmpty());
	action->setCheckable(!data.isEmpty());
	action->setData(QVariant(layout));

//	std::cout << "** " << layout << "-" << widgetIndex << std::endl;
	QtSignalAdapters::connect0<void()>(
		action,
		SIGNAL(triggered()),
		boost::bind(&LayoutInteractor::setActiveLayout, this, layout, widgetIndex));

	return action;
}

void LayoutInteractor::setActiveLayout(QString layout, int widgetIndex)
{
//	std::cout << "*slot* " << layout << "-" << widgetIndex << std::endl;
	viewManager()->setActiveLayout(layout, widgetIndex);
}

} // namespace cx

