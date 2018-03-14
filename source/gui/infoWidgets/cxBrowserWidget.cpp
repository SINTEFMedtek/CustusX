/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
	connect(mModel, &TreeItemModel::modelAboutToBeReset, this, &BrowserWidget::setModified);
	connect(mModel, &TreeItemModel::currentItemChanged, this, &BrowserWidget::onCurrentItemChanged);
	connect(mModel, &TreeItemModel::loaded, this, &BrowserWidget::onLoaded);

	//layout
	mTreeView = new QTreeView(this);
//	mTreeView->setRootIsDecorated(false);
//	mTreeView->setTreePosition(1);
	mTreeView->setModel(mModel);
	connect(mTreeView.data(), &QTreeView::collapsed, this, &BrowserWidget::onNodeCollapsed);
	connect(mTreeView.data(), &QTreeView::expanded, this, &BrowserWidget::onNodeExpanded);
	mModel->setSelectionModel(mTreeView->selectionModel());

	mPopupWidget = new PopupToolbarWidget(this);
	connect(mPopupWidget, &PopupToolbarWidget::popup, this, &BrowserWidget::onPopup);
	layout->addWidget(mPopupWidget);
	mPopupWidget->setPopupVisible(this->getShowToolbarOption().readValue(QString::number(false)).toInt());

	mPropertiesWidget = new ReplacableContentWidget(this);
	mPropertiesWidget->setWidgetDeleteOld(new QLabel("no\nproperties\nselected"));

	mSplitter = new ControllableSplitter(mOptions.descend("splitter"), this);
	layout->addWidget(mSplitter, 1);
	mSplitter->addLeftWidget(mTreeView, "browser");
	mSplitter->addRightWidget(mPropertiesWidget, "properties");

	this->createButtonWidget(mPopupWidget->getToolbar());
}

void BrowserWidget::onNodeCollapsed(const QModelIndex & index)
{
	mExpanded.removeAll(mModel->getNodeFromIndex(index)->getUid());
}

void BrowserWidget::onNodeExpanded(const QModelIndex & index)
{
	QString uid = mModel->getNodeFromIndex(index)->getUid();
	if (!mExpanded.contains(uid))
		mExpanded.push_back(uid);
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
	if (!node->isDefaultExpanded())
		return;

	mTreeView->expand(index);

	int rc = mModel->rowCount(index);
	for (int r=0; r<rc; ++r)
	{
		QModelIndex child = mModel->index(r, 0, index);
		this->expandDefault(child);
	}
}

void BrowserWidget::expandRestore(QModelIndex index)
{
	TreeNodePtr node = mModel->getNodeFromIndex(index);
	if (index.isValid() && !mExpanded.contains(node->getUid()))
		return;
//	CX_LOG_CHANNEL_DEBUG("CA") << " expandRestore " << node->getUid();

	mTreeView->expand(index);

	int rc = mModel->rowCount(index);
	for (int r=0; r<rc; ++r)
	{
		QModelIndex child = mModel->index(r, 0, index);
		this->expandRestore(child);
	}
}

bool BrowserWidget::setCurrentNode(QString uid, QModelIndex index)
{
	TreeNodePtr node = mModel->getNodeFromIndex(index);
//	CX_LOG_CHANNEL_DEBUG("CA") << " setCurrentNode: inspecting node " << node->getUid();

	if (node && (node->getUid()==uid))
	{
//		CX_LOG_CHANNEL_DEBUG("CA") << " setCurrentNode: HIT, setting current index " << node->getUid();
//		boost::function<void()> f =
//				boost::bind(&QItemSelectionModel::setCurrentIndex,
//							mTreeView->selectionModel(),
//							index,
//							QItemSelectionModel::Current);
//		QTimer::singleShot(0, f);
		mTreeView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Select);
//		mTreeView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Current);
		return true;
	}

	int rc = mModel->rowCount(index);
	for (int r=0; r<rc; ++r)
	{
		QModelIndex child = mModel->index(r, 0, index);
		if (this->setCurrentNode(uid, child))
			return true;
	}
	return false;
}

void BrowserWidget::resetView()
{
	mTreeView->setRootIsDecorated(mModel->repo()->getMode()!="spaces");

	if (mExpanded.empty())
	{
		this->expandDefault(QModelIndex());
//		CX_LOG_CHANNEL_DEBUG("CA") << "===BrowserWidget:: rebuild default()";

		for (unsigned i=0; i<mModel->columnCount(); ++i)
			mTreeView->resizeColumnToContents(i);
	}
	else
	{
//		CX_LOG_CHANNEL_DEBUG("CA") << "===BrowserWidget::reset content() ";// << mExpanded.join("\n ");
		this->expandRestore(QModelIndex());
	}

//	CX_LOG_CHANNEL_DEBUG("CA") << "===BrowserWidget::setCurrentNode  " << mActiveNodeUid;
	this->setCurrentNode(mActiveNodeUid, QModelIndex());

//.	mActiveNodeUid = node->getUid();
}

void BrowserWidget::onLoaded()
{
	mExpanded.clear();
	this->setModified();
}

void BrowserWidget::onCurrentItemChanged()
{
//	CX_LOG_CHANNEL_DEBUG("CA") << " ***********  BrowserWidget::onCurrentItemChanged()";

	TreeNodePtr node = mModel->getCurrentItem();

	if (node->getUid() == mActiveNodeUid)
		return;
	if (node->getUid() == mModel->repo()->getTopNode()->getUid())
		return;

	mRemoveButton->setEnabled(node && node->isRemovable());
	this->updateNodeName();

	if (node)
	{
		mActiveNodeUid = node->getUid();
//		CX_LOG_CHANNEL_DEBUG("CA") << "   store CurrentNode  " << mActiveNodeUid;
		boost::shared_ptr<QWidget> widget = node->createPropertiesWidget();
		mPropertiesWidget->setWidget(widget);
//		CX_LOG_CHANNEL_DEBUG("CA") << "mPropertiesWidget: " << mPropertiesWidget->parent();
//		if (widget)
//			CX_LOG_CHANNEL_DEBUG("CA") << "                 : " << widget->parent();
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
