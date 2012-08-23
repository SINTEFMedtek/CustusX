#include "cxBrowserWidget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QVBoxLayout>
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "sscToolManager.h"
#include "cxViewManager.h"
#include "cxView2D.h"
#include "cxView3D.h"
#include "cxViewGroup.h"
#include "sscRep.h"

namespace cx
{



BrowserItemModel::BrowserItemModel(QObject* parent) : QAbstractItemModel(parent)
{
  mSelectionModel = NULL;

  mFilter = ssc::StringDataAdapterXml::initialize("filter",
      "Filter",
      "Select which items to show in the browser",
      "all",
      QString("all views images tools").split(" "),
      QDomNode());
  connect(mFilter.get(), SIGNAL(valueWasSet()), this, SLOT(buildTree()));

  this->buildTree();
}


void BrowserItemModel::setSelectionModel(QItemSelectionModel* selectionModel)
{
  mSelectionModel = selectionModel;
  connect(mSelectionModel, SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(currentItemChangedSlot(const QModelIndex&, const QModelIndex&)));
}

ssc::StringDataAdapterXmlPtr BrowserItemModel::getFilter()
{
  return mFilter;
}

void BrowserItemModel::fillModelTreeFromViewManager(TreeItemPtr root)
{
  //TreeItemPtr item;
  TreeItemPtr topItem = TreeItemImpl::create(root, "view groups", "", "");
  std::vector<ViewGroupPtr> groups = viewManager()->getViewGroups();

  for (unsigned i = 0; i < groups.size(); ++i)
  {
    ViewGroupPtr group = groups[i];
    std::vector<ssc::ViewWidget*> views = group->getViews();
    if (views.empty())
      continue;
    TreeItemPtr groupItem = TreeItemImpl::create(topItem, "group"+qstring_cast(i), "view group", qstring_cast(i));
    for (unsigned j=0; j<views.size(); ++j)
    {
      TreeItemPtr viewItem = TreeItemImpl::create(groupItem, qstring_cast(views[j]->getName()), qstring_cast(views[j]->getTypeString()), "");
      std::vector<ssc::RepPtr> reps = views[j]->getReps();
      for (unsigned k=0; k<reps.size(); ++k)
      {
        QString name = reps[k]->getName();
        if (name.isEmpty())
          name = reps[k]->getType();
        TreeItemImpl::create(viewItem, qstring_cast(name), qstring_cast(reps[k]->getType()), "");
      }
    }

    std::vector<ssc::ImagePtr> images = group->getImages();
    for (unsigned j=0; j<images.size(); ++j)
    {
      TreeItemPtr imageItem = TreeItemImage::create(groupItem, images[j]->getName());
    }
  }
}

void BrowserItemModel::buildTree()
{
  this->beginResetModel();

  QString filter = mFilter->getValue();
  bool showViews  = filter=="all" || filter=="views";
  bool showImages = filter=="all" || filter=="images";
  bool showTools  = filter=="all" || filter=="tools";

  mTree = TreeItemImpl::create(this);

  if (showViews)
  {
  	this->fillModelTreeFromViewManager(mTree);
  }

  if (showImages)
  {
    //TreeItemPtr item;
    TreeItemPtr imagesItem = TreeItemImpl::create(mTree, "images", "", "");
    ssc::DataManager::ImagesMap images = ssc::dataManager()->getImages();
    for (ssc::DataManager::ImagesMap::iterator iter=images.begin(); iter!=images.end(); ++iter)
    {

      TreeItemPtr imageItem = TreeItemImage::create(imagesItem, iter->second->getUid());
  //    TreeItemPtr imageItem = TreeItemImpl::create(imagesItem, qstring_cast(iter->second->getName()), "image", "");
    }
  }

  if (showTools)
  {
    TreeItemToolManager::create(mTree);
  }

//  TreeItemPtr item;
//
//  item = TreeItemImpl::create(mTree, "A", "dummy", "text1");
//  TreeItemImpl::create(item, "A1", "dummy", "text2a1");
//  TreeItemImpl::create(item, "A2", "dummy", "text2a2");
//
//  TreeItemImpl::create(mTree, "B", "dummy", "text2");
//  TreeItemImpl::create(mTree, "C", "dummy", "text3");
  this->endResetModel();

  emit hasBeenReset();
}

BrowserItemModel::~BrowserItemModel()
{
}

void BrowserItemModel::currentItemChangedSlot(const QModelIndex& current, const QModelIndex& previous)
{
  //std::cout << "item changed" << std::endl;
  TreeItem *item = this->itemFromIndex(current);
  if (!item)
    return;
  item->activate();
}

void BrowserItemModel::treeItemChangedSlot()
{
  //std::cout << "BrowserItemModel::treeItemChangedSlot()" << std::endl;
  TreeItem* item = dynamic_cast<TreeItem*>(sender());
  //std::cout << "item " << item << std::endl;
  QModelIndex index = this->createIndex(0,0,item);
  emit dataChanged(index, index);
  //void QAbstractItemModel::dataChanged ( const QModelIndex & topLeft, const QModelIndex & bottomRight )
}

TreeItem* BrowserItemModel::itemFromIndex(const QModelIndex& index) const
{
  if (!index.isValid())
      return mTree.get();
  else
      return static_cast<TreeItem*>(index.internalPointer());
}

int BrowserItemModel::columnCount(const QModelIndex& parent) const
{
  TreeItem *parentItem = this->itemFromIndex(parent);
//  if (parent.column() > 0) // ignore for all but first column
//      return 0;
  return parentItem->getColumnCount();
}

int BrowserItemModel::rowCount(const QModelIndex& parent) const
{
  TreeItem *parentItem = this->itemFromIndex(parent);
  if (parent.column() > 0) // ignore for all but first column
      return 0;
  return parentItem->getChildCount();
}

QVariant BrowserItemModel::data(const QModelIndex& index, int role) const
{
  if (role==Qt::DisplayRole)
  {
    TreeItem *item = this->itemFromIndex(index);
    if (index.column()==0)
      return item->getName();
    if (index.column()==1)
      return item->getType();
    if (index.column()==2)
      return item->getData();
  }
  if (role==Qt::FontRole)
  {
    TreeItem *item = this->itemFromIndex(index);
    return item->getFont();
  }
  return QVariant();
}

Qt::ItemFlags BrowserItemModel::flags(const QModelIndex& index) const
{
  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant BrowserItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    if (section==0)
      return "Item";
    if (section==1)
      return "Type";
    if (section==2)
      return "Details";
  }
  return QVariant();
}

QModelIndex BrowserItemModel::index(int row, int column, const QModelIndex& parent) const
{
//  if (parent!=QModelIndex())
//    return QModelIndex(); // valid only for direct siblings of top level
//  return this->createIndex(row,column, mValues[row]);
//

//  if (!hasIndex(row, column, parent))
//      return QModelIndex();

  TreeItem *parentItem = this->itemFromIndex(parent);
  TreeItem *childItem = parentItem->getChild(row).get();
  if (childItem)
      return createIndex(row, column, childItem);
  else
      return QModelIndex();
}

QModelIndex BrowserItemModel::parent(const QModelIndex& index) const
{
//  if (!index.isValid())
//      return QModelIndex();

  TreeItem *childItem = this->itemFromIndex(index);

  if (!childItem)
    return QModelIndex();

  TreeItem *parentItem = childItem->getParent().lock().get();

  if (parentItem == mTree.get())
      return QModelIndex();

  // find row of parent within grandparent
  TreeItemPtr grandParent = parentItem->getParent().lock();
  int row = 0;
  if (grandParent)
  {
    for (row=0; row<grandParent->getChildCount(); ++row)
      if (parentItem==grandParent->getChild(row).get())
        break;
  }

  return createIndex(row, 0, parentItem);
}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


BrowserWidget::BrowserWidget(QWidget* parent) :
    BaseWidget(parent, "BrowserWidget", "Browser"),
    //mTreeWidget(new QTreeWidget(this)),
    mVerticalLayout(new QVBoxLayout(this))
{
  mModel = new BrowserItemModel(this);
  connect(mModel, SIGNAL(hasBeenReset()), this, SLOT(resetView()));

  mVerticalLayout->addWidget(new ssc::LabeledComboBoxWidget(this, mModel->getFilter()));

  //layout
  mTreeView = new QTreeView(this);;
  mTreeView->setModel(mModel);
  mModel->setSelectionModel(mTreeView->selectionModel());
  mVerticalLayout->addWidget(mTreeView);
}
BrowserWidget::~BrowserWidget()
{
}

QString BrowserWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Browser</h3>"
      "<p>Lets you browse different aspects of the system.</p>"
      "<p><i></i></p>"
      "</html>";
}


void BrowserWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
  connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(populateTreeWidget()));
  connect(viewManager(), SIGNAL(imageDeletedFromViews(ssc::ImagePtr)), this, SLOT(populateTreeWidget()));
  connect(viewManager(), SIGNAL(activeLayoutChanged()), this, SLOT(populateTreeWidget()));
  connect(viewManager(), SIGNAL(activeViewChanged()), this, SLOT(populateTreeWidget()));

  this->populateTreeWidget();
}
void BrowserWidget::closeEvent(QCloseEvent* event)
{
  QWidget::closeEvent(event);
  disconnect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(populateTreeWidget()));
  disconnect(viewManager(), SIGNAL(imageDeletedFromViews(ssc::ImagePtr)), this, SLOT(populateTreeWidget()));
}

void BrowserWidget::resetView()
{
  mTreeView->expandToDepth(1);
  mTreeView->resizeColumnToContents(0);
}

void BrowserWidget::populateTreeWidget()
{
  mModel->buildTree();
}

}//end namespace cx
