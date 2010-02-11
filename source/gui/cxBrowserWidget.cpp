#include "cxBrowserWidget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QVBoxLayout>
#include "cxDataManager.h"
#include "cxToolManager.h"
#include "cxMessageManager.h"
#include "cxViewManager.h"
#include "cxView2D.h"
#include "cxView3D.h"

namespace cx
{
BrowserWidget::BrowserWidget(QWidget* parent) :
    QWidget(parent),
    mTreeWidget(new QTreeWidget(this)),
    mVerticalLayout(new QVBoxLayout()),
    mDataManager(DataManager::getInstance()),
    mToolManager(ToolManager::getInstance()),
    mMessageManager(MessageManager::getInstance()),
    mViewManager(ViewManager::getInstance())
{
  //dock widget
  this->setWindowTitle("Browser");

  //layout
  mVerticalLayout->addWidget(mTreeWidget);
}
BrowserWidget::~BrowserWidget()
{}
void BrowserWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
  connect(mDataManager, SIGNAL(dataLoaded()),
          this, SLOT(populateTreeWidget()));
  connect(mViewManager, SIGNAL(imageDeletedFromViews(ssc::ImagePtr)),
          this, SLOT(populateTreeWidget()));
  this->populateTreeWidget();
}
void BrowserWidget::closeEvent(QCloseEvent* event)
{
  QWidget::closeEvent(event);
  disconnect(mDataManager, SIGNAL(dataLoaded()),
             this, SLOT(populateTreeWidget()));
  disconnect(mViewManager, SIGNAL(imageDeletedFromViews(ssc::ImagePtr)),
             this, SLOT(populateTreeWidget()));
}
void BrowserWidget::populateTreeWidget()
{
  //get all images, meshes and tools
  std::map<std::string, std::string> imageUidAndNames =
      mDataManager->getImageUidsAndNames();
  std::map<std::string, std::string> meshUidAndNames =
      mDataManager->getMeshUIDsWithNames();
  std::map<std::string, std::string> toolUidAndName =
      mToolManager->getToolUidsAndNames();

  //get all views
  std::map<std::string, View2D*>* view2DMap = mViewManager->get2DViews();
  std::map<std::string, View3D*>* view3DMap = mViewManager->get3DViews();

  //ready the tree
  mTreeWidget->clear();
  mTreeWidget->setColumnCount(2);
  QStringList headerItems(QStringList() << "Name" << "Uid");
  mTreeWidget->setHeaderLabels(headerItems);

  //keep track of what index we are at
  int index = 0;

  //make QTreeWidgetItems for all the views
  //TODO hide views thats not a part of the current layout
  QList<QTreeWidgetItem *> viewItems;
  std::map<std::string, View3D*>::iterator view3Diter = view3DMap->begin();
  while(view3Diter != view3DMap->end())
  {
    QStringList view3D(QStringList() << QString(view3Diter->second->getName().c_str())
                                     << QString(view3Diter->second->getUid().c_str()));
    QTreeWidgetItem* view3DItem = new QTreeWidgetItem(view3D);
    viewItems.append(view3DItem);
    view3Diter++;
  }
  std::map<std::string, View2D*>::iterator view2Diter = view2DMap->begin();
  while(view2Diter != view2DMap->end())
  {
    QStringList view2D(QStringList() << QString(view2Diter->second->getName().c_str())
                                     << QString(view2Diter->second->getUid().c_str()));
    QTreeWidgetItem* view2DItem = new QTreeWidgetItem(view2D);
    viewItems.append(view2DItem);
    view2Diter++;
  }
  mTreeWidget->insertTopLevelItems(index, viewItems);

  QTreeWidgetItem* topLevelItem;
  foreach(topLevelItem, viewItems)
  {
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
    }
  }
}
}//end namespace cx
