/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxLayoutInteractor.h"
#include "cxLayoutEditorWidget.h"
#include <QMenu>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QMetaMethod>
#include "boost/bind.hpp"
#include "libQtSignalAdapters/Qt2Func.h"
#include "libQtSignalAdapters/ConnectionFactories.h"
#include <QVBoxLayout>
#include "cxLayoutRepository.h"
#include "cxViewService.h"

namespace cx
{

LayoutInteractor::LayoutInteractor(ViewServicePtr viewService, QObject* parent) :
	QObject(parent),
	mViewService(viewService)
{
	mSecondaryLayoutActionGroup = NULL;
	mLayoutActionGroup = NULL;
	this->createActions();
	connect(viewService.get(), SIGNAL(activeLayoutChanged()), this, SLOT(layoutChangedSlot()));
}

LayoutRepositoryPtr LayoutInteractor::getRepo()
{
	return mViewService->getLayoutRepository();
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
	this->getRepo()->insert(data);
	mViewService->setActiveLayout(data.getUid());
}

void LayoutInteractor::editCustomLayoutSlot()
{
	LayoutData data = this->executeLayoutEditorDialog("Edit Current Layout", false);
	if (data.getUid().isEmpty())
		return;
	this->getRepo()->insert(data);
}

void LayoutInteractor::deleteCustomLayoutSlot()
{
	if (QMessageBox::question(NULL, "Delete current layout", "Do you really want to delete the current layout?",
		QMessageBox::Cancel | QMessageBox::Ok) != QMessageBox::Ok)
		return;
	this->getRepo()->erase(mViewService->getActiveLayout());
	mViewService->setActiveLayout(this->getRepo()->getAvailable().front()); // revert to existing state
}

/** Called when the layout is changed: update the layout menu
 */
void LayoutInteractor::layoutChangedSlot()
{
	if (!mMenu)
		return;
	if (!this->getRepo())
		return;
	// reset list of available layouts

	this->deepDeleteActionGroup(mLayoutActionGroup);
	mLayoutActionGroup = this->createLayoutActionGroup(0);
	mMenu->addActions(mLayoutActionGroup->actions());

	this->deepDeleteActionGroup(mSecondaryLayoutActionGroup);
	mSecondaryLayoutActionGroup = this->createLayoutActionGroup(1);
	mSecondaryLayoutMenu->addActions(mSecondaryLayoutActionGroup->actions());

	bool editable = this->getRepo()->isCustom(mViewService->getActiveLayout());
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

	LayoutEditorWidget* editor = new LayoutEditorWidget(mViewService, dialog.get());

	LayoutData data = this->getRepo()->get(mViewService->getActiveLayout());

	if (createNew)
	{
		data.resetUid(this->getRepo()->generateUid());
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
	if (!this->getRepo())
		return retval;


	// add default layouts
	std::vector<QString> layouts = this->getRepo()->getAvailable();
	int defaultLayouts = 0;
	for (unsigned i = 0; i < layouts.size(); ++i)
	{
		if (!this->getRepo()->isCustom(layouts[i]))
		{
			this->addLayoutAction(layouts[i], retval, widgetIndex);
			defaultLayouts++;
		}
	}

	// add separator
	QAction* sep = new QAction(retval);
    sep->setSeparator(true);
	//retval->addAction(sep);

	if (defaultLayouts != layouts.size())
	{
		QAction* action = new QAction("Custom", retval);
		action->setEnabled(false);
	}

	// add custom layouts
	for (unsigned i = 0; i < layouts.size(); ++i)
	{
		if (this->getRepo()->isCustom(layouts[i]))
			this->addLayoutAction(layouts[i], retval, widgetIndex);
	}

	// set checked status
	QString type = mViewService->getActiveLayout(widgetIndex);
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
	LayoutData data = this->getRepo()->get(layout);
	if (data.isEmpty())
	{
		QAction* sep = new QAction(group);
        sep->setSeparator(true);
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
	mViewService->setActiveLayout(layout, widgetIndex);
}

} // namespace cx

