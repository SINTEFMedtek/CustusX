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
#include "cxBrowserWidget.h"

#include <QTreeView>
#include "cxTreeItemModel.h"
#include "cxLogger.h"
#include <QSplitter>
#include <QMenu>
#include <QLabel>
#include "cxTreeNode.h"
#include "cxPopupToolbarWidget.h"
#include <QWidgetAction>
#include <QCheckBox>
#include "boost/bind.hpp"
#include "cxTreeRepository.h"

namespace cx
{

BrowserWidget::BrowserWidget(QWidget* parent, VisServicesPtr services) :
	BaseWidget(parent, "BrowserWidget", "Browser"),
	mServices(services)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);

	this->setModified();
	mModel = new TreeItemModel(services, this);
	connect(mModel, &TreeItemModel::hasBeenReset, this, &BrowserWidget::setModified);
	connect(mModel, &TreeItemModel::currentItemChanged, this, &BrowserWidget::onCurrentItemChanged);
	//  mVerticalLayout->addWidget(new LabeledComboBoxWidget(this, mModel->getFilter()));

	//layout
	mTreeView = new QTreeView(this);
	mTreeView->setRootIsDecorated(false);
	mTreeView->setModel(mModel);
	mModel->setSelectionModel(mTreeView->selectionModel());

	mPopupWidget = new PopupToolbarWidget(this);
//	connect(mPopupWidget, &PopupToolbarWidget::popup, this, &ConsoleWidget::updateShowHeader);
	layout->addWidget(mPopupWidget);
	this->createButtonWidget(mPopupWidget->getToolbar());


	QSplitter *splitter = new QSplitter(Qt::Horizontal);
	mSplitter = splitter;

	layout->addWidget(mSplitter, 1);
	mSplitter->addWidget(mTreeView);

	mPropertiesWidget = new ReplacableContentWidget(this);
	mSplitter->addWidget(mPropertiesWidget);

}

BrowserWidget::~BrowserWidget()
{
}


void BrowserWidget::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
}
void BrowserWidget::closeEvent(QCloseEvent* event)
{
	QWidget::closeEvent(event);
}

void BrowserWidget::prePaintEvent()
{
	mPopupWidget->refresh();
//	CX_LOG_CHANNEL_DEBUG("CA") << "BrowserWidget::prePaintEvent()";
	mModel->update();
	this->resetView();
}

void BrowserWidget::createButtonWidget(QWidget* widget)
{
	QHBoxLayout* buttonLayout = new QHBoxLayout(widget);
	buttonLayout->setMargin(0);
	buttonLayout->setSpacing(0);

//	QIcon icon(":/icons/open_icon_library/system-run-5.png");
//	QAction* action = this->createAction(this,
//										 icon,
//										 "Details", "Show detailed info on each log entry",
//										 SLOT(updateUI()),
//										 buttonLayout, new CXSmallToolButton());
//	action->setCheckable(true);

	QStringList allnodetypes = mModel->repo()->getAllNodeTypes();
	QStringList visiblenodetypes = mModel->repo()->getVisibleNodeTypes();

	QMenu* menu = new QMenu(this);
	for (int i=0; i<allnodetypes.size(); ++i)
	{
		QString text = allnodetypes[i];
		QCheckBox *checkBox = new QCheckBox(text, menu);
		checkBox->setChecked(visiblenodetypes.contains(text));
		boost::function<void(bool)> func = boost::bind(&BrowserWidget::onNodeVisibilityChanged, this, text, _1);
		connect(checkBox, &QCheckBox::toggled, func);
		QWidgetAction *checkableAction = new QWidgetAction(menu);
		checkableAction->setDefaultWidget(checkBox);
		menu->addAction(checkableAction);

//		QAction* action = new QAction(QIcon(), QString("action%1").arg(i), this);
//		action->setCheckable(true);
//		menu->addAction(action);
	}

	QToolButton* visibilityButton = new CXSmallToolButton();
	visibilityButton->setIcon(QIcon(":/icons/open_icon_library/eye.png.png"));
	visibilityButton->setToolTip("Select nodes to display");
	visibilityButton->setPopupMode(QToolButton::InstantPopup);
	visibilityButton->setMenu(menu);
	buttonLayout->addWidget(visibilityButton);

	this->createFilterSelector();
	LabeledComboBoxWidget* channelSelectorWidget = new LabeledComboBoxWidget(this, mFilterSelector);
	channelSelectorWidget->showLabel(false);
	buttonLayout->addSpacing(8);
	buttonLayout->addWidget(channelSelectorWidget);
	buttonLayout->setStretch(buttonLayout->count()-1, 0);

	buttonLayout->addSpacing(8);
	buttonLayout->addStretch(1);
}

void BrowserWidget::onNodeVisibilityChanged(QString nodeType, bool value)
{
	CX_LOG_CHANNEL_DEBUG("CA") << "BrowserWidget::onNodeVisibilityChanged " << nodeType << "  " << value;
}

//QStringList getVisibleNodeTypes() const { return mVisibleNodeTypes; }
//void setVisibleNodeTypes(QStringList val) { if (mVisibleNodeTypes==val) return; mVisibleNodeTypes = val; this->invalidate(); }
//QStringList getAllNodeTypes() const { return mAllNodeTypes; }

void BrowserWidget::createFilterSelector()
{
	QStringList filters = mModel->repo()->getAllModes();
	QString filterval = mModel->repo()->getMode();

	StringPropertyPtr retval;
	retval = StringProperty::initialize("FilterSelector",
											  "", "Item display mode",
											  filterval, filters, QDomNode());
	connect(retval.get(), &StringPropertyBase::changed, this, &BrowserWidget::onFilterSelectorChanged);
	mFilterSelector = retval;
}


void BrowserWidget::onFilterSelectorChanged()
{
	mModel->repo()->setMode(mFilterSelector->getValue());
}

void BrowserWidget::resetView()
{
	mTreeView->expandToDepth(4);
	for (unsigned i=0; i<4; ++i)
		mTreeView->resizeColumnToContents(i);
}

void BrowserWidget::onCurrentItemChanged()
{
	QWidget* widget = mModel->getCurrentItem()->createPropertiesWidget();
	mPropertiesWidget->setWidget(widget);
}

}//end namespace cx
