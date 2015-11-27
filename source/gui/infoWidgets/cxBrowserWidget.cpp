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

ControllableSplitter::ControllableSplitter(QWidget *parent) :
	mShiftSplitterLeft(NULL),
	mShiftSplitterRight(NULL),
	mSplitterRatio(0.5)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);

	mSplitter = new QSplitter(Qt::Horizontal);
	connect(mSplitter, &QSplitter::splitterMoved, this, &ControllableSplitter::onSplitterMoved);

	layout->addWidget(mSplitter, 1);
}

void ControllableSplitter::addLeftWidget(QWidget *widget)
{
	mSplitter->insertWidget(0, widget);
}
void ControllableSplitter::addRightWidget(QWidget *widget)
{
	mSplitter->insertWidget(1, widget);
}

QAction* ControllableSplitter::getMoveLeftAction()
{
	if (!mShiftSplitterLeft)
	{
		QAction* action = new QAction(QIcon(":/icons/open_icon_library/arrow-left-3.png"),
									  "Show props", this);
		action->setToolTip("Show more properties");
		action->setStatusTip(action->toolTip());
		connect(action, &QAction::triggered, this, &ControllableSplitter::onMoveSplitterLeft);
		mShiftSplitterLeft = action;
	}
	return mShiftSplitterLeft;
}

QAction* ControllableSplitter::getMoveRightAction()
{
	if (!mShiftSplitterRight)
	{
		QAction* action = new QAction(QIcon(":/icons/open_icon_library/arrow-right-3.png"),
									  "Show browser", this);
		action->setToolTip("Show more browser");
		action->setStatusTip(action->toolTip());
		connect(action, &QAction::triggered, this, &ControllableSplitter::onMoveSplitterRight);
		mShiftSplitterRight = action;
	}
	return mShiftSplitterRight;
}

void ControllableSplitter::onMoveSplitterLeft()
{
	this->shiftSplitter(-1);
}

void ControllableSplitter::onMoveSplitterRight()
{
	this->shiftSplitter(+1);
}

void ControllableSplitter::onSplitterMoved()
{
	QList<int> sizes = mSplitter->sizes();
	if (this->splitterShowsBoth())
	{
		mSplitterRatio = double(sizes[0]) /double(sizes[0]+sizes[1]);
//		CX_LOG_CHANNEL_DEBUG("CA") << "moved, r=" << mSplitterRatio;
	}
//	CX_LOG_CHANNEL_DEBUG("CA") << "moved,sizes: " << sizes[0] << " - " << sizes[1];

	mShiftSplitterLeft->setEnabled(this->getShiftState()>=0);
	mShiftSplitterRight->setEnabled(this->getShiftState()<=0);
}

bool ControllableSplitter::splitterShowsBoth() const
{
	QList<int> sizes = mSplitter->sizes();
	return (( sizes.size()==2 )&&( sizes[0]!=0 )&&( sizes[1]!=0 ));
}

int ControllableSplitter::getShiftState() const
{
	QList<int> sizes = mSplitter->sizes();

	if(sizes[0]==0)
		return -1;
	else if(sizes[1]==0)
		return 1;
	else
		return 0;
}

void ControllableSplitter::setShiftState(int shiftState)
{
	QList<int> sizes = mSplitter->sizes();

	if (shiftState<0) // show props
	{
		sizes[0] = 0;
		sizes[1] = 1;
	}
	else if (shiftState>0) // show both
	{
		sizes[0] = 1;
		sizes[1] = 0;
	}
	else // shop browser
	{
		int sizesum = sizes[0]+sizes[1];
//		CX_LOG_CHANNEL_DEBUG("CA") << "sizes: " << sizes[0] << " / " << sizes[1] << " / sum=" << sizesum << " / r=" << mSplitterRatio;
		sizes[0] = mSplitterRatio * sizesum;
		sizes[1] = (1.0-mSplitterRatio) * sizesum;
	}
	mSplitter->setSizes(sizes);
//	CX_LOG_CHANNEL_DEBUG("CA") << "set sizes: " << sizes[0] << " / " << sizes[1];

	this->onSplitterMoved();
//	CX_LOG_CHANNEL_DEBUG("CA") << "this->getShiftState(): " << this->getShiftState();
}

void ControllableSplitter::shiftSplitter(int shift)
{
	// positive shift axis goes to the right, from browser to properties
	//
	QList<int> sizes = mSplitter->sizes();

	int shiftState = this->getShiftState();

//	CX_LOG_CHANNEL_DEBUG("CA") << "shiftstateA: " << shiftState;
	shiftState += shift;
//	CX_LOG_CHANNEL_DEBUG("CA") << "shiftstateB: " << shiftState;

	this->setShiftState(shiftState);
}

} // namespace cx

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

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

	//layout
	mTreeView = new QTreeView(this);
	mTreeView->setRootIsDecorated(false);
	mTreeView->setModel(mModel);
	mModel->setSelectionModel(mTreeView->selectionModel());

	mPopupWidget = new PopupToolbarWidget(this);
//	connect(mPopupWidget, &PopupToolbarWidget::popup, this, &ConsoleWidget::updateShowHeader);
	layout->addWidget(mPopupWidget);

	mSplitter = new ControllableSplitter(this);
	layout->addWidget(mSplitter, 1);
	mSplitter->addLeftWidget(mTreeView);
	mPropertiesWidget = new ReplacableContentWidget(this);
	mSplitter->addRightWidget(mPropertiesWidget);

	this->createButtonWidget(mPopupWidget->getToolbar());
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
	CX_LOG_CHANNEL_DEBUG("CA") << "BrowserWidget::prePaintEvent() modified";
	mModel->update();
	this->resetView();
}

void BrowserWidget::createButtonWidget(QWidget* widget)
{
	QHBoxLayout* buttonLayout = new QHBoxLayout(widget);
	buttonLayout->setMargin(0);
	buttonLayout->setSpacing(0);

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
	}

	QToolButton* visibilityButton = new CXSmallToolButton();
	visibilityButton->setIcon(QIcon(":/icons/open_icon_library/eye.png.png"));
	visibilityButton->setToolTip("Select nodes to display");
	visibilityButton->setPopupMode(QToolButton::InstantPopup);
	visibilityButton->setMenu(menu);
	buttonLayout->addWidget(visibilityButton);

	this->createFilterSelector();
	LabeledComboBoxWidget* filterSelectorWidget = new LabeledComboBoxWidget(this, mFilterSelector);
	filterSelectorWidget->showLabel(false);
	buttonLayout->addSpacing(8);
	buttonLayout->addWidget(filterSelectorWidget);
	buttonLayout->setStretch(buttonLayout->count()-1, 0);

	buttonLayout->addSpacing(8);

	// everything being added after this will be aligned to the right side
	buttonLayout->addStretch(1);

	QToolButton* button;
	button = new CXSmallToolButton();
	button->setDefaultAction(mSplitter->getMoveLeftAction());
	buttonLayout->addWidget(button);

	button = new CXSmallToolButton();
	button->setDefaultAction(mSplitter->getMoveRightAction());
	buttonLayout->addWidget(button);
}

void BrowserWidget::onNodeVisibilityChanged(QString nodeType, bool value)
{
//	CX_LOG_CHANNEL_DEBUG("CA") << "BrowserWidget::onNodeVisibilityChanged " << nodeType << "  " << value;
	QStringList visible = mModel->repo()->getVisibleNodeTypes();
	if (value)
		visible.append(nodeType);
	else
		visible.removeAll(nodeType);
	mModel->repo()->setVisibleNodeTypes(visible);
}

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
