#include "cxBrowserWidget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QVBoxLayout>
#include "sscMessageManager.h"
#include "cxDataManager.h"
#include "cxToolManager.h"
#include "cxViewManager.h"
#include "cxRepManager.h"
#include "cxView2D.h"
#include "cxView3D.h"

namespace cx
{



BrowserItemModel::BrowserItemModel(QObject* parent)
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

void BrowserItemModel::buildTree()
{
  this->beginResetModel();

  QString filter = mFilter->getValue();
  bool showViews  = filter=="all" || filter=="views";
  bool showImages = filter=="all" || filter=="images";
  bool showTools  = filter=="all" || filter=="tools";

  mTree = TreeItemImpl::create(TreeItemWeakPtr(), "","","");

  if (showViews)
  {
    viewManager()->fillModelTree(mTree);
  }

  if (showImages)
  {
    //TreeItemPtr item;
    TreeItemPtr imagesItem = TreeItemImpl::create(mTree, "images", "", "");
    ssc::DataManager::ImagesMap images = dataManager()->getImages();
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
  std::cout << "item changed" << std::endl;
  TreeItem *item = this->itemFromIndex(current);
  if (!item)
    return;
  item->activate();
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
    QWidget(parent),
    //mTreeWidget(new QTreeWidget(this)),
    mVerticalLayout(new QVBoxLayout(this))
{
  this->setWindowTitle("Browser");

  mModel = new BrowserItemModel(this);
  connect(mModel, SIGNAL(hasBeenReset()), this, SLOT(resetView()));

  mVerticalLayout->addWidget(new ssc::ComboGroupWidget(this, mModel->getFilter()));

  //layout
  mTreeView = new QTreeView(this);;
  mTreeView->setModel(mModel);
  mModel->setSelectionModel(mTreeView->selectionModel());
  mVerticalLayout->addWidget(mTreeView);
}
BrowserWidget::~BrowserWidget()
{}
void BrowserWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
  connect(dataManager(), SIGNAL(dataLoaded()), this, SLOT(populateTreeWidget()));
  connect(viewManager(), SIGNAL(imageDeletedFromViews(ssc::ImagePtr)), this, SLOT(populateTreeWidget()));
  connect(viewManager(), SIGNAL(activeLayoutChanged()), this, SLOT(populateTreeWidget()));
  connect(viewManager(), SIGNAL(activeViewChanged()), this, SLOT(populateTreeWidget()));

  this->populateTreeWidget();
}
void BrowserWidget::closeEvent(QCloseEvent* event)
{
  QWidget::closeEvent(event);
  disconnect(dataManager(), SIGNAL(dataLoaded()), this, SLOT(populateTreeWidget()));
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
//  mTreeView->expandToDepth(1);
//  mTreeView->resizeColumnToContents(0);


  //  //get all images, meshes and tools
//  std::map<std::string, std::string> imageUidAndNames =
//      dataManager()->getImageUidsAndNames();
//  std::map<std::string, std::string> meshUidAndNames =
//      dataManager()->getMeshUidsWithNames();
//  std::map<std::string, std::string> toolUidAndName =
//      toolManager()->getToolUidsAndNames();
//
//  //get all views
//  std::map<std::string, View2D*>* view2DMap = viewManager()->get2DViews();
//  std::map<std::string, View3D*>* view3DMap = viewManager()->get3DViews();
//
//  //ready the tree
//  mTreeWidget->clear();
//  mTreeWidget->setColumnCount(2);
//  QStringList headerItems(QStringList() << "Name" << "Uid");
//  mTreeWidget->setHeaderLabels(headerItems);
//  mTreeWidget->resizeColumnToContents(1);
//  mTreeWidget->resize(this->size());
//
//
////  //make QTreeWidgetItems for all the views
////  LayoutType layoutType = viewManager()->getActiveLayout();
//  int numberOf3DViews = 0;
//  int numberOf2DViews = 0;
////  switch(layoutType)
////  {
////    case LAYOUT_NONE:
////      break;
////    case LAYOUT_3D_1X1:
////      numberOf3DViews = 1;
////      break;
////    /*case LAYOUT_3DACS_2X2: INRIA
////      numberOf3DViews = 1;
////      numberOf2DViews = 3;
////      break;
////    case LAYOUT_3DACS_1X3: INRIA
////      numberOf3DViews = 1;
////      numberOf2DViews = 3;
////      break;
////    case LAYOUT_ACSACS_2X3: INRIA
////      numberOf2DViews = 6;
////      break;*/
////    default:
////      break;
////  }
//  QList<QTreeWidgetItem *> viewItems;
//  std::map<std::string, View3D*>::iterator view3Diter = view3DMap->begin();
//  while(view3Diter != view3DMap->end())
//  {
//    if(numberOf3DViews == 0) //only adding the views shown by the layout
//      break;
//    QStringList view3DList(QStringList() << QString(view3Diter->second->getName().c_str())
//                                     << QString(view3Diter->second->getUid().c_str()));
//    QTreeWidgetItem* view3DItem = new QTreeWidgetItem(view3DList);
//    viewItems.append(view3DItem);
//    view3Diter++;
//    numberOf3DViews--;
//  }
//  std::map<std::string, View2D*>::iterator view2Diter = view2DMap->begin();
//  while(view2Diter != view2DMap->end())
//  {
//    if(numberOf2DViews == 0) //only adding the views shown by the layout
//      break;
//    QStringList view2DList(QStringList() << QString(view2Diter->second->getName().c_str())
//                                     << QString(view2Diter->second->getUid().c_str()));
//    QTreeWidgetItem* view2DItem = new QTreeWidgetItem(view2DList);
//    viewItems.append(view2DItem);
//    view2Diter++;
//    numberOf2DViews--;
//  }
//  mTreeWidget->insertTopLevelItems(0, viewItems);
//
//  QTreeWidgetItem* topLevelItem;
//  foreach(topLevelItem, viewItems)
//  {
//    ssc::View* view = viewManager()->getView(topLevelItem->text(1).toStdString());
//    if(view==NULL) //couldn't find a view with that id
//      return;
//    std::vector<ssc::RepPtr> reps = view->getReps();
//    std::vector<ssc::RepPtr>::iterator repIter = reps.begin();
//    while(repIter != reps.end())
//    {
//      //we need to add slightly different things per reptype,
//      //some rep types (like landmarkRep and probeRep, we dont want to add at all
//      std::string repType = (*repIter)->getType();
//      if(repType == "ssc::VolumetricRep")
//      {
//        //add the reps
//        /*mMessageManager->sendWarning((*repIter)->getUid().c_str());
//        std::cout << (*repIter)->getUid().c_str() << std::endl;*/
//        QStringList repList(QStringList() << QString((*repIter)->getName().c_str())
//                                        << QString((*repIter)->getUid().c_str()));
//        QTreeWidgetItem* repItem = new QTreeWidgetItem(repList);
//        topLevelItem->addChild(repItem);
//        //add the images
//        std::string repUid = (*repIter)->getUid();
//
//        ssc::VolumetricRepPtr volRep = repManager()->getVolumetricRep(repUid);
//        if(!volRep)
//          break;
//        ssc::ImagePtr image = volRep->getImage();
//        QStringList imageList(QStringList() << QString(image->getName().c_str())
//                                        << QString(image->getUid().c_str()));
//        QTreeWidgetItem* imageItem = new QTreeWidgetItem(imageList);
//        repItem->addChild(imageItem);
//      }else if (repType == "ssc::GeometricRep")
//      {
//        QStringList repList(QStringList() << QString((*repIter)->getName().c_str())
//                                        << QString((*repIter)->getUid().c_str()));
//        QTreeWidgetItem* repItem = new QTreeWidgetItem(repList);
//        topLevelItem->addChild(repItem);
//        //add meshes under geometricRep
//        std::string repUid = (*repIter)->getUid();
//        ssc::GeometricRepPtr geometricRep = repManager()->getGeometricRep(repUid);
//        if(!geometricRep)
//          break;
//        ssc::MeshPtr mesh = geometricRep->getMesh();
//        QStringList meshList(QStringList() << QString(mesh->getName().c_str())
//                                        << QString(mesh->getUid().c_str()));
//        QTreeWidgetItem* meshItem = new QTreeWidgetItem(meshList);
//        repItem->addChild(meshItem);
//      }else if(repType == "ssc::ToolRep3D")
//      {
//        QStringList repList(QStringList() << QString((*repIter)->getName().c_str())
//                                        << QString((*repIter)->getUid().c_str()));
//        QTreeWidgetItem* repItem = new QTreeWidgetItem(repList);
//        topLevelItem->addChild(repItem);
//        //add tools under toolreps
//        std::string repUid = (*repIter)->getUid();
//        ssc::ToolRep3DPtr toolRep3D = repManager()->getToolRep3DRep(repUid);
//        if(!toolRep3D)
//          break;
//        ssc::ToolPtr tool = toolRep3D->getTool();
//        QStringList toolList(QStringList() << QString(tool->getName().c_str())
//                                        << QString(tool->getUid().c_str()));
//        QTreeWidgetItem* toolItem = new QTreeWidgetItem(toolList);
//        repItem->addChild(toolItem);
//      }
//      repIter++;
//    }

    //TODO:
    //for all reps
      //add rep to tree
      //add its data (image/mesh/tool etc) to the tree

    /*
    //make QTreeWidgetItems of all the images
    std::map<std::string, std::string>::iterator it1 = imageUidAndNames.begin();
    while(it1 != imageUidAndNames.end())
    {
      QStringList image(QStringList() << QString(it1->second.c_str())
                                      << QString(it1->first.c_str()));
      QTreeWidgetItem* imageItem = new QTreeWidgetItem(image);
      topLevelItem->addChild(imageItem);
      index++;
      it1++;
    }
    //make QTreeWidgetItems of all the meshes
    std::map<std::string, std::string>::iterator it2 = meshUidAndNames.begin();
    while(it2 != meshUidAndNames.end())
    {
      std::map<std::string, std::string>::iterator parentIt; // =
          //imageUidAndNames.find(it2->getParent()); //TODO waiting for the parent function
      if(parentIt != imageUidAndNames.end()) //mesh has a parent
      {
        //find the parents imageItem already in the tree
        QList<QTreeWidgetItem *> possibleParents =
            mTreeWidget->findItems(parentIt->first.c_str(), Qt::MatchFixedString, 1);
        if(!possibleParents.isEmpty()) //found a parent item
        {
          QTreeWidgetItem* parent = possibleParents.first();
          QStringList mesh(QStringList() << QString(it1->first.c_str())
                                          << QString(it1->second.c_str()));
          QTreeWidgetItem* meshItem = new QTreeWidgetItem(mesh);
          parent->addChild(meshItem);
        }
        else //could not find a parent in the tree, but it should have a parent...?
        {
          mMessageManager->sendWarning("Could not find the meshes parent in the tree.");
        }
      }
      else //mesh has no known parent
      {
        QStringList mesh(QStringList() << QString(it1->first.c_str())
                                        << QString(it1->second.c_str()));
        QTreeWidgetItem* meshItem = new QTreeWidgetItem(mesh);
        topLevelItem->addChild(meshItem);
        index++;
      }
      it2++;
    }
    //make QTreeWidgetItems of all the tools
    std::map<std::string, std::string>::iterator it3 = toolUidAndName.begin();
    while(it3 != toolUidAndName.end())
    {
      QStringList tool(QStringList() << QString(it1->second.c_str())
                                      << QString(it1->first.c_str()));
      QTreeWidgetItem* toolItem = new QTreeWidgetItem(tool);
      topLevelItem->addChild(toolItem);
      index++;
      it3++;
    }*/
  //}
}
}//end namespace cx
