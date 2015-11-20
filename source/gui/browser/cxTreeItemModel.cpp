#include "cxTreeItemModel.h"

//#include "cxBrowserWidget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QVBoxLayout>
#include "cxPatientModelService.h"
#include "cxViewService.h"
#include "cxRep.h"
#include "cxViewGroupData.h"
#include "cxLegacySingletons.h"
#include "cxTypeConversions.h"
#include "cxTreeRepository.h"
#include "cxLogger.h"

namespace cx
{



TreeItemModel::TreeItemModel(QObject* parent) : QAbstractItemModel(parent)
{
	mSelectionModel = NULL;

	this->buildTree();
}


void TreeItemModel::setSelectionModel(QItemSelectionModel* selectionModel)
{
	mSelectionModel = selectionModel;
	connect(mSelectionModel, SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(currentItemChangedSlot(const QModelIndex&, const QModelIndex&)));
}

void TreeItemModel::buildTree()
{
	CX_LOG_CHANNEL_DEBUG("CA") << "TreeItemModel::buildTree() B";
	this->beginResetModel();
	mRepository = TreeRepository::create();
	this->endResetModel();
	emit hasBeenReset();
	CX_LOG_CHANNEL_DEBUG("CA") << "TreeItemModel::buildTree() E";
}

TreeItemModel::~TreeItemModel()
{
}

void TreeItemModel::currentItemChangedSlot(const QModelIndex& current, const QModelIndex& previous)
{
	//std::cout << "item changed" << std::endl;
	TreeNode *item = this->itemFromIndex(current);
	if (!item)
		return;
	item->activate();
}


void TreeItemModel::treeItemChangedSlot()
{
	CX_LOG_CHANNEL_DEBUG("CA") << "in use!";
	TreeNode* item = dynamic_cast<TreeNode*>(sender());
	QModelIndex index = this->createIndex(0,0,item);
	emit dataChanged(index, index);
}

TreeNode *TreeItemModel::itemFromIndex(const QModelIndex& index) const
{
	if (!index.isValid())
		return mRepository->getTopNode().get();
	else
		return static_cast<TreeNode*>(index.internalPointer());
}

int TreeItemModel::columnCount(const QModelIndex& parent) const
{
	TreeNode *parentItem = this->itemFromIndex(parent);
	if (parent.column() > 0) // ignore for all but first column
		return 0;
	return 1;
	//  return parentItem->getColumnCount();
}

int TreeItemModel::rowCount(const QModelIndex& parent) const
{
	TreeNode *parentItem = this->itemFromIndex(parent);
	if (parent.column() > 0) // ignore for all but first column
		return 0;
	return parentItem->getChildren().size();
}

QVariant TreeItemModel::data(const QModelIndex& index, int role) const
{
	if (role==Qt::DisplayRole)
	{
		TreeNode *item = this->itemFromIndex(index);
		if (index.column()==0)
			return item->getName();
//		if (index.column()==1)
//			return item->getType();
//		if (index.column()==2)
//			return item->getData();
	}
	if (role==Qt::FontRole)
	{
//		TreeItem *item = this->itemFromIndex(index);
//		return item->getFont();
		return QVariant();
	}
	return QVariant();
}

Qt::ItemFlags TreeItemModel::flags(const QModelIndex& index) const
{
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant TreeItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		if (section==0)
			return "Item";
//		if (section==1)
//			return "Type";
//		if (section==2)
//			return "Details";
	}
	return QVariant();
}

QModelIndex TreeItemModel::index(int row, int column, const QModelIndex& parent) const
{
	TreeNode *parentItem = this->itemFromIndex(parent);
	std::vector<TreeNodePtr> children = parentItem->getChildren();
	if (row < children.size())
	{
		return createIndex(row, column, children[row].get());
	}
	else
	{
		return QModelIndex();
	}
}

QModelIndex TreeItemModel::parent(const QModelIndex& index) const
{
	TreeNode *childItem = this->itemFromIndex(index);

	if (!childItem)
		return QModelIndex();

	TreeNode *parentItem = childItem->getParent().get();

	if (parentItem == mRepository->getTopNode().get())
		return QModelIndex();

	// find row of parent within grandparent
	TreeNodePtr grandParent = parentItem->getParent();
	int row = 0;
	if (grandParent)
	{
		std::vector<TreeNodePtr> parentSiblings = grandParent->getChildren();
		for (row=0; row<parentSiblings.size(); ++row)
			if (parentItem==parentSiblings[row].get())
				break;
	}

	return createIndex(row, 0, parentItem);
}

}//end namespace cx

