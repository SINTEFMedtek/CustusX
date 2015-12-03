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
#include "cxProfile.h"
#include "cxStringListSelectWidget.h"
#include "cxDataSelectWidget.h"

namespace cx
{

BrowserWidget::BrowserWidget(QWidget* parent, VisServicesPtr services) :
	BaseWidget(parent, "BrowserWidget", "Browser"),
	mServices(services),
	mModel(NULL)
{
	mOptions = profile()->getXmlSettings().descend(this->objectName());
	this->setModified();
}

void BrowserWidget::createGUI()
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);

	mModel = new TreeItemModel(mOptions.descend("model"), mServices, this);
	connect(mModel, &TreeItemModel::hasBeenReset, this, &BrowserWidget::setModified);
	connect(mModel, &TreeItemModel::currentItemChanged, this, &BrowserWidget::onCurrentItemChanged);

	//layout
	mTreeView = new QTreeView(this);
//	mTreeView->setRootIsDecorated(false);
//	mTreeView->setTreePosition(1);
	mTreeView->setModel(mModel);
	mModel->setSelectionModel(mTreeView->selectionModel());

	mPopupWidget = new PopupToolbarWidget(this);
	connect(mPopupWidget, &PopupToolbarWidget::popup, this, &BrowserWidget::onPopup);
	layout->addWidget(mPopupWidget);
	mPopupWidget->setPopupVisible(this->getShowToolbarOption().readValue(QString::number(false)).toInt());

	mPropertiesWidget = new ReplacableContentWidget(this);
	mPropertiesWidget->setWidget(new QLabel("no\nproperties\nselected"));

	mSplitter = new ControllableSplitter(mOptions.descend("splitter"), this);
	layout->addWidget(mSplitter, 1);
	mSplitter->addLeftWidget(mTreeView, "browser");
	mSplitter->addRightWidget(mPropertiesWidget, "properties");

	this->createButtonWidget(mPopupWidget->getToolbar());
}

void BrowserWidget::onPopup()
{
	this->getShowToolbarOption().writeValue(QString::number(mPopupWidget->popupIsVisible()));
}

BrowserWidget::~BrowserWidget()
{
}

void BrowserWidget::prePaintEvent()
{
	if (!mModel)
		this->createGUI();
	mPopupWidget->refresh();
//	CX_LOG_CHANNEL_DEBUG("CA") << "BrowserWidget::prePaintEvent() modified";
	mModel->update();
	this->resetView();
}

void BrowserWidget::updateNodeName()
{
	TreeNodePtr node =  mModel->getCurrentItem();

	if (mName->actions().empty())
		mName->addAction(node->getIcon(), QLineEdit::LeadingPosition);
	else
		mName->actions().front()->setIcon(node->getIcon());


	mName->setText(node->getName());
//	mName->addAction(node->getIcon(), QLineEdit::LeadingPosition);

	int height = mName->height();

	QFontMetrics fm(QFont("", 0));
	mName->setFixedSize(fm.width(mName->text())+height*2, height);

//    this->adjustSize();
}


void BrowserWidget::createButtonWidget(QWidget* widget)
{
	QHBoxLayout* buttonLayout = new QHBoxLayout(widget);
	buttonLayout->setMargin(0);
	buttonLayout->setSpacing(0);

	StringListSelectWidget* visibility = new StringListSelectWidget(this, mModel->repo()->getVisibilityProperty());
	visibility->showLabel(false);
	visibility->setIcon(QIcon(":/icons/open_icon_library/arrange_vertical.png"));
	buttonLayout->addWidget(visibility);

	LabeledComboBoxWidget* mode = new LabeledComboBoxWidget(this, mModel->repo()->getModeProperty());
	mode->showLabel(false);
	buttonLayout->addSpacing(8);
	buttonLayout->addWidget(mode);
	buttonLayout->setStretch(buttonLayout->count()-1, 0);

	StringListSelectWidget* columns = new StringListSelectWidget(this, mModel->getShowColumnsProperty());
	columns->showLabel(false);
	columns->setIcon(QIcon(":/icons/open_icon_library/arrange_horizontal.png"));
	buttonLayout->addWidget(columns);

	buttonLayout->addSpacing(8);

	// everything being added after this will be aligned to the right side
	buttonLayout->addStretch(1);

	mRemoveButton = new EraseDataToolButton(this);
	connect(mRemoveButton, &EraseDataToolButton::eraseData, this, &BrowserWidget::eraseCurrentNode);
	buttonLayout->addWidget(mRemoveButton);

	mName = new QLineEdit;
	mName->setReadOnly(true);
	buttonLayout->addWidget(mName);

	QToolButton* button;
	button = new CXSmallToolButton();
	button->setDefaultAction(mSplitter->getMoveLeftAction());
	buttonLayout->addWidget(button);

	button = new CXSmallToolButton();
	button->setDefaultAction(mSplitter->getMoveRightAction());
	buttonLayout->addWidget(button);
}

XmlOptionItem BrowserWidget::getShowToolbarOption()
{
	return XmlOptionItem("show_toolbar", mOptions.getElement());
}

void BrowserWidget::expandDefault(QModelIndex index)
{
	TreeNodePtr node = mModel->getNodeFromIndex(index);
//	CX_LOG_CHANNEL_DEBUG("CA") << "check expand " << node->getName() << ", " <<  node->isDefaultExpanded();
	if (!node->isDefaultExpanded())
		return;

//	CX_LOG_CHANNEL_DEBUG("CA") << "   did expand" << node->getName();
	mTreeView->expand(index);

	int rc = mModel->rowCount(index);
	for (int r=0; r<rc; ++r)
	{
		QModelIndex child = mModel->index(r, 0, index);
		this->expandDefault(child);
	}
}

void BrowserWidget::resetView()
{
	mTreeView->setRootIsDecorated(mModel->repo()->getMode()!="spaces");
	this->expandDefault(QModelIndex());
//	mTreeView->expandToDepth(4);
		
	for (unsigned i=0; i<mModel->columnCount(); ++i)
		mTreeView->resizeColumnToContents(i);
}

void BrowserWidget::onCurrentItemChanged()
{
	TreeNodePtr node = mModel->getCurrentItem();
	mRemoveButton->setEnabled(node && node->isRemovable());
	this->updateNodeName();

	if (node)
	{
		QWidget* widget = node->createPropertiesWidget();
		mPropertiesWidget->setWidget(widget);
	}
}

void BrowserWidget::eraseCurrentNode()
{
	TreeNodePtr node = mModel->getCurrentItem();
	if (!node)
		return;

	node->remove();
}

}//end namespace cx
