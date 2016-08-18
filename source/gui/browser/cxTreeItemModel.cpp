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
#include "cxStringListProperty.h"

namespace cx
{



TreeItemModel::TreeItemModel(XmlOptionFile options, VisServicesPtr services, QObject* parent) :
	QAbstractItemModel(parent),
	mServices(services),
	mOptions(options)
{
	mSelectionModel = NULL;
	mNameIndex = 1;
	mColorIndex = 0;
	mViewGroupIndex = 2;
	mViewGroupCount = 3;

	this->createShowColumnsProperty();
	this->onShowColumnsChanged();

	mRepository = TreeRepository::create(options.descend("repository"), services);
	connect(mRepository.get(), &TreeRepository::invalidated, this, &TreeItemModel::beginResetModel);
	connect(mRepository.get(), &TreeRepository::loaded, this, &TreeItemModel::loaded);
	connect(mRepository.get(), &TreeRepository::changed, this, &TreeItemModel::onRepositoryChanged);
}

void TreeItemModel::onRepositoryChanged(TreeNode* node)
{
	if (node)
	{
//		CX_LOG_CHANNEL_DEBUG("CA") << "change on node=" << node->getName();

		QModelIndex index0 = this->createIndex(0, 0, node);
		QModelIndex index1 = this->createIndex(0, mColumnCount, node);
		emit dataChanged(index0, index1);
	}
	else
	{
//		CX_LOG_CHANNEL_DEBUG("CA") << "change on all";
		emit dataChanged(QModelIndex(),QModelIndex());
	}
}


void TreeItemModel::update()
{
//	CX_LOG_CHANNEL_DEBUG("CA") << "TreeItemModel::buildTree() B";
	this->beginResetModel();
	mRepository->update();
	this->endResetModel();
//	CX_LOG_CHANNEL_DEBUG("CA") << "TreeItemModel::buildTree() E";
}

void TreeItemModel::createShowColumnsProperty()
{
	QStringList range = QStringList() << "color" << "vg0" << "vg1" << "vg2" << "vg3";
	QStringList defvals = range;
	defvals.pop_back();
	defvals.pop_back();

	std::map<QString, QString> names;
	names["color"] = "Color";
	for (unsigned i=0; i<4; ++i)
		names[QString("vg%1").arg(i)] = QString("View Group %1").arg(i);

//	QStringList range = QStringList() << "Color"
//									  << "View Group 0"
//									  << "View Group 1"
//									  << "View Group 2"
//									  << "View Group 3";
	mShowColumnsProperty = StringListProperty::initialize("visible_columns",
														"Columns",
														"Select visible columns",
														range,
														range,
														 mOptions.getElement());
	mShowColumnsProperty->setDisplayNames(names);
	connect(mShowColumnsProperty.get(), &Property::changed, this, &TreeItemModel::onShowColumnsChanged);
}

void TreeItemModel::onShowColumnsChanged()
{
//	CX_LOG_CHANNEL_DEBUG("CA") << "TreeItemModel::onShowColumnsChanged()";
	this->beginResetModel();

	int none = 1000;
	QStringList cols = mShowColumnsProperty->getValue();

	mColumnCount = 0;
	mNameIndex = mColumnCount++;
	mColorIndex = cols.contains("color") ? mColumnCount++ : none;
	mViewGroupCount = 0;
	while (cols.contains(QString("vg%1").arg(mViewGroupCount)))
		++mViewGroupCount;
	mViewGroupIndex = (mViewGroupCount>0) ? mColumnCount : none;
	mColumnCount += mViewGroupCount;

//	CX_LOG_CHANNEL_DEBUG("CA") << "mColorIndex: " << mColorIndex;
//	CX_LOG_CHANNEL_DEBUG("CA") << "mNameIndex: " << mNameIndex;
//	CX_LOG_CHANNEL_DEBUG("CA") << "mViewGroupIndex: " << mViewGroupIndex;
//	CX_LOG_CHANNEL_DEBUG("CA") << "mViewGroupCount: " << mViewGroupCount;
//	CX_LOG_CHANNEL_DEBUG("CA") << "mColumnCount: " << mColumnCount;

//	this->endResetModel();
//	emit hasBeenReset();
}



void TreeItemModel::setSelectionModel(QItemSelectionModel* selectionModel)
{
	mSelectionModel = selectionModel;
	connect(mSelectionModel, &QItemSelectionModel::currentChanged,
			this, &TreeItemModel::currentItemChangedSlot);
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
	return this->getNodeFromIndex(mi);
}

TreeNodePtr TreeItemModel::getNodeFromIndex(const QModelIndex& index)
{
	TreeNode *item = this->itemFromIndex(index);
	if (!item)
		return TreeNodePtr();
	return mRepository->getNode(item->getUid());
}

TreeNode* TreeItemModel::itemFromIndex(const QModelIndex& index) const
{
	if (!index.isValid())
		return mRepository->getTopNode().get();
	else
		return static_cast<TreeNode*>(index.internalPointer());
}

int TreeItemModel::columnCount(const QModelIndex& parent) const
{
	if (parent.isValid() && (parent.column() != mNameIndex)) // ignore for all but first column
		return 0;
	return mColumnCount;
}

int TreeItemModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid() && (parent.column() != mNameIndex)) // ignore for all but first column
		return 0;
	TreeNode *parentItem = this->itemFromIndex(parent);
	return parentItem->getVisibleChildren().size();
}

QVariant TreeItemModel::data(const QModelIndex& index, int role) const
{
	if (role==Qt::DisplayRole)
	{
		if (index.column()==mNameIndex)
		{
			TreeNode *item = this->itemFromIndex(index);
			return item->getName();
		}
	}
	if (role==Qt::CheckStateRole)
	{
		if (this->isViewGroupColumn(index.column()))
		{
			TreeNode *item = this->itemFromIndex(index);
			return item->getViewGroupVisibility(this->viewGroupFromColumn(index.column()));
		}
	}
	if (role==Qt::ToolTipRole || role==Qt::StatusTipRole)
	{
		TreeNode *item = this->itemFromIndex(index);
		if (index.column()==mNameIndex)
			return QString("%1 of type %2").arg(item->getName()).arg(item->getType());
		if (this->isViewGroupColumn(index.column()))
			return QString("Set visibility of %1 in view group %2")
					.arg(item->getName()).arg(this->viewGroupFromColumn(index.column()));
	}
	if (role==Qt::DecorationRole)
	{
		TreeNode *item = this->itemFromIndex(index);
		if (index.column()==mNameIndex)
			return item->getIcon();
		if (index.column()==mColorIndex)
		{
			if (item->getColor().canConvert<QColor>())
					return this->getColorIcon(item->getColor().value<QColor>());
		}
	}
	if (role==Qt::FontRole)
	{
		if (index.column()==mNameIndex)
		{
			TreeNode *item = this->itemFromIndex(index);
			return item->getFont();
		}
	}
	if (role==Qt::ForegroundRole)
	{
		if (index.column()==mNameIndex)
		{
			TreeNode *item = this->itemFromIndex(index);
//			if (item->useColoredName())
			{
				QVariant color = item->getColor();
				if (color.canConvert<QColor>())
				{
					QColor oldColor = color.value<QColor>().toHsv();
					QColor newColor = this->adjustColorToContrastWithWhite(oldColor);
					return newColor;
				}
			}
		}
	}
	return QVariant();
}


bool TreeItemModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (role==Qt::CheckStateRole)
	{
		if (this->isViewGroupColumn(index.column()))
		{
			TreeNode *item = this->itemFromIndex(index);
			item->setViewGroupVisibility(this->viewGroupFromColumn(index.column()), value.value<int>());
		}
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

bool TreeItemModel::isViewGroupColumn(int col) const
{
	return ( (0<=(col-mViewGroupIndex))
			&&
			((col-mViewGroupIndex) < mViewGroupCount)
			);
}
int TreeItemModel::viewGroupFromColumn(int col) const
{
	return (col-mViewGroupIndex);
}

QVariant TreeItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		if (section==mNameIndex)
			return "    Item";
		if (section==mColorIndex)
			return ""; // keep short + out of the way of the toolbar button
		if (this->isViewGroupColumn(section))
			return QString("V%1").arg(this->viewGroupFromColumn(section));
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

	return createIndex(row, mNameIndex, parentItem);
}

QIcon TreeItemModel::getColorIcon(QColor color) const
{
	QImage image(QSize(128,128), QImage::Format_RGBA8888);
	QPainter painter(&image);
	painter.fillRect(image.rect(), QColor("white"));
	painter.setBrush(QBrush(color));
	painter.drawRoundedRect(image.rect(), 75, 75, Qt::RelativeSize);
//	painter.drawEllipse(image.rect());
	return QIcon(QPixmap::fromImage(image));
}

QColor TreeItemModel::adjustColorToContrastWithWhite(QColor color) const
{
	// use some tricks in HSV space to get contrasting colors while
	// keeping the original color as much as possible.
	//
	// for colors: strengthen the saturation: s' = s+(1-s)/2
	// for for grayscale: lower value, or give up for whites:
	//           if s<0.1: v = max(0.7, v)
	double h = color.hueF();
	double s = color.hsvSaturationF();
	double v = color.valueF();

	double isChromatic = s > 0.1; // our definition
	if (isChromatic)
	{
		s = s+(1.0-s)/2;
	}
	else
	{
		v = std::min(0.6, v);
	}

	return QColor::fromHsvF(h,s,v);
}



}//end namespace cx

