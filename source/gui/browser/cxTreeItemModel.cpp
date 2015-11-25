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
#include "cxTypeConversions.h"
#include "cxTreeRepository.h"
#include "cxLogger.h"
#include <QVariant>

namespace cx
{



TreeItemModel::TreeItemModel(VisServicesPtr services, QObject* parent) :
	QAbstractItemModel(parent),
	mServices(services)
{
	mSelectionModel = NULL;
	mViewGroupCount = 3;

	mRepository = TreeRepository::create(services);
	connect(mRepository.get(), &TreeRepository::invalidated, this, &TreeItemModel::hasBeenReset);
	connect(mRepository.get(), &TreeRepository::changed, this, &TreeItemModel::onRepositoryChanged);
}

void TreeItemModel::onRepositoryChanged()
{
	emit dataChanged(QModelIndex(),QModelIndex());
}


void TreeItemModel::update()
{
//	CX_LOG_CHANNEL_DEBUG("CA") << "TreeItemModel::buildTree() B";
	this->beginResetModel();
	mRepository->update();
	this->endResetModel();
//	CX_LOG_CHANNEL_DEBUG("CA") << "TreeItemModel::buildTree() E";
}


void TreeItemModel::setSelectionModel(QItemSelectionModel* selectionModel)
{
	mSelectionModel = selectionModel;
	connect(mSelectionModel, &QItemSelectionModel::currentChanged,
			this, &TreeItemModel::currentItemChangedSlot);
//	connect(mSelectionModel, SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(currentItemChangedSlot(const QModelIndex&, const QModelIndex&)));
}

TreeItemModel::~TreeItemModel()
{
}

void TreeItemModel::currentItemChangedSlot(const QModelIndex& current, const QModelIndex& previous)
{
	TreeNode *item = this->itemFromIndex(current);
	if (!item)
		return;
	item->activate();
	emit currentItemChanged();
}

TreeNodePtr TreeItemModel::getCurrentItem()
{
	QModelIndex mi = mSelectionModel->currentIndex();
	TreeNode *item = this->itemFromIndex(mi);
	if (!item)
		return TreeNodePtr();
	return mRepository->getNode(item->getUid());
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
	return 1+mViewGroupCount;
	//  return parentItem->getColumnCount();
}

int TreeItemModel::rowCount(const QModelIndex& parent) const
{
	TreeNode *parentItem = this->itemFromIndex(parent);
	if (parent.column() > 0) // ignore for all but first column
		return 0;
	return parentItem->getVisibleChildren().size();
}

QVariant TreeItemModel::data(const QModelIndex& index, int role) const
{
	int namepos = 0;
	int iconpos = 0;

	if (role==Qt::DisplayRole)
	{
		TreeNode *item = this->itemFromIndex(index);
		if (index.column()==namepos)
			return item->getName();
//		if (index.column()<mViewGroupCount+1)
//			return item->getViewGroupVisibility(index.column()-1);
//		if (index.column()==1)
//			return item->getType();
//		if (index.column()==2)
//			return item->getData();
	}
	if (role==Qt::CheckStateRole)
	{
		TreeNode *item = this->itemFromIndex(index);
		if (index.column()>0 && index.column()<mViewGroupCount+1)
			return item->getViewGroupVisibility(index.column()-1);
	}
	if (role==Qt::ToolTipRole || role==Qt::StatusTipRole)
	{
		TreeNode *item = this->itemFromIndex(index);
		if (index.column()==namepos)
			return QString("%1 of type %2").arg(item->getName()).arg(item->getType());
		if (index.column()>0 && index.column()<mViewGroupCount+1)
			return QString("Set visibility of %1 in view group %2").arg(item->getName()).arg(index.column()-1);
//		if (index.column()<mViewGroupCount+1)
//			return item->getViewGroupVisibility(index.column()-1);
//		if (index.column()==1)
//			return item->getType();
//		if (index.column()==2)
//			return item->getData();
	}
	if (role==Qt::DecorationRole)
	{
		TreeNode *item = this->itemFromIndex(index);
		if (index.column()==iconpos)
			return item->getIcon();
	}
	if (role==Qt::FontRole)
	{
		TreeNode *item = this->itemFromIndex(index);
		return item->getFont();
	}
	if (role==Qt::ForegroundRole)
	{
		if (index.column()==namepos)
		{
			TreeNode *item = this->itemFromIndex(index);
			return item->getColor();
		}
	}
	return QVariant();
}

bool TreeItemModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (role==Qt::CheckStateRole)
	{
		TreeNode *item = this->itemFromIndex(index);
		if (index.column()>0 && index.column()<mViewGroupCount+1)
			item->setViewGroupVisibility(index.column()-1, value.value<int>());
		return true;
	}
	return false;
}

Qt::ItemFlags TreeItemModel::flags(const QModelIndex& index) const
{
	if (index.column()>0)
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
	else
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant TreeItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		if (section==0)
			return "Item";
		if (section>0 && section-1<mViewGroupCount)
			return QString("V%1").arg(section);
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
	std::vector<TreeNodePtr> children = parentItem->getVisibleChildren();
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

	TreeNode *parentItem = childItem->getVisibleParent().get();

	if (parentItem == mRepository->getTopNode().get())
		return QModelIndex();

	// find row of parent within grandparent
	TreeNodePtr grandParent = parentItem->getVisibleParent();
	int row = 0;
	if (grandParent)
	{
		std::vector<TreeNodePtr> parentSiblings = grandParent->getVisibleChildren();
		for (row=0; row<parentSiblings.size(); ++row)
			if (parentItem==parentSiblings[row].get())
				break;
	}

	return createIndex(row, 0, parentItem);
}



}//end namespace cx

