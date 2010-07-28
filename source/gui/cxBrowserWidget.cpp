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
BrowserWidget::BrowserWidget(QWidget* parent) :
    QWidget(parent),
    mTreeWidget(new QTreeWidget(this)),
    mVerticalLayout(new QVBoxLayout())
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
  connect(dataManager(), SIGNAL(dataLoaded()),
          this, SLOT(populateTreeWidget()));
  connect(viewManager(), SIGNAL(imageDeletedFromViews(ssc::ImagePtr)),
          this, SLOT(populateTreeWidget()));
  this->populateTreeWidget();
}
void BrowserWidget::closeEvent(QCloseEvent* event)
{
  QWidget::closeEvent(event);
  disconnect(dataManager(), SIGNAL(dataLoaded()),
             this, SLOT(populateTreeWidget()));
  disconnect(viewManager(), SIGNAL(imageDeletedFromViews(ssc::ImagePtr)),
             this, SLOT(populateTreeWidget()));
}
void BrowserWidget::populateTreeWidget()
{
  //get all images, meshes and tools
  std::map<std::string, std::string> imageUidAndNames =
      dataManager()->getImageUidsAndNames();
  std::map<std::string, std::string> meshUidAndNames =
      dataManager()->getMeshUidsWithNames();
  std::map<std::string, std::string> toolUidAndName =
      toolManager()->getToolUidsAndNames();

  //get all views
  std::map<std::string, View2D*>* view2DMap = viewManager()->get2DViews();
  std::map<std::string, View3D*>* view3DMap = viewManager()->get3DViews();

  //ready the tree
  mTreeWidget->clear();
  mTreeWidget->setColumnCount(2);
  QStringList headerItems(QStringList() << "Name" << "Uid");
  mTreeWidget->setHeaderLabels(headerItems);
  mTreeWidget->resizeColumnToContents(1);
  mTreeWidget->resize(this->size());


//  //make QTreeWidgetItems for all the views
//  LayoutType layoutType = viewManager()->getActiveLayout();
  int numberOf3DViews = 0;
  int numberOf2DViews = 0;
//  switch(layoutType)
//  {
//    case LAYOUT_NONE:
//      break;
//    case LAYOUT_3D_1X1:
//      numberOf3DViews = 1;
//      break;
//    /*case LAYOUT_3DACS_2X2: INRIA
//      numberOf3DViews = 1;
//      numberOf2DViews = 3;
//      break;
//    case LAYOUT_3DACS_1X3: INRIA
//      numberOf3DViews = 1;
//      numberOf2DViews = 3;
//      break;
//    case LAYOUT_ACSACS_2X3: INRIA
//      numberOf2DViews = 6;
//      break;*/
//    default:
//      break;
//  }
  QList<QTreeWidgetItem *> viewItems;
  std::map<std::string, View3D*>::iterator view3Diter = view3DMap->begin();
  while(view3Diter != view3DMap->end())
  {
    if(numberOf3DViews == 0) //only adding the views shown by the layout
      break;
    QStringList view3DList(QStringList() << QString(view3Diter->second->getName().c_str())
                                     << QString(view3Diter->second->getUid().c_str()));
    QTreeWidgetItem* view3DItem = new QTreeWidgetItem(view3DList);
    viewItems.append(view3DItem);
    view3Diter++;
    numberOf3DViews--;
  }
  std::map<std::string, View2D*>::iterator view2Diter = view2DMap->begin();
  while(view2Diter != view2DMap->end())
  {
    if(numberOf2DViews == 0) //only adding the views shown by the layout
      break;
    QStringList view2DList(QStringList() << QString(view2Diter->second->getName().c_str())
                                     << QString(view2Diter->second->getUid().c_str()));
    QTreeWidgetItem* view2DItem = new QTreeWidgetItem(view2DList);
    viewItems.append(view2DItem);
    view2Diter++;
    numberOf2DViews--;
  }
  mTreeWidget->insertTopLevelItems(0, viewItems);

  QTreeWidgetItem* topLevelItem;
  foreach(topLevelItem, viewItems)
  {
    ssc::View* view = viewManager()->getView(topLevelItem->text(1).toStdString());
    if(view==NULL) //couldn't find a view with that id
      return;
    std::vector<ssc::RepPtr> reps = view->getReps();
    std::vector<ssc::RepPtr>::iterator repIter = reps.begin();
    while(repIter != reps.end())
    {
      //we need to add slightly different things per reptype,
      //some rep types (like landmarkRep and probeRep, we dont want to add at all
      std::string repType = (*repIter)->getType();
      if(repType == "ssc::VolumetricRep")
      {
        //add the reps
        /*mMessageManager->sendWarning((*repIter)->getUid().c_str());
        std::cout << (*repIter)->getUid().c_str() << std::endl;*/
        QStringList repList(QStringList() << QString((*repIter)->getName().c_str())
                                        << QString((*repIter)->getUid().c_str()));
        QTreeWidgetItem* repItem = new QTreeWidgetItem(repList);
        topLevelItem->addChild(repItem);
        //add the images
        std::string repUid = (*repIter)->getUid();

        ssc::VolumetricRepPtr volRep = repManager()->getVolumetricRep(repUid);
        if(!volRep)
          break;
        ssc::ImagePtr image = volRep->getImage();
        QStringList imageList(QStringList() << QString(image->getName().c_str())
                                        << QString(image->getUid().c_str()));
        QTreeWidgetItem* imageItem = new QTreeWidgetItem(imageList);
        repItem->addChild(imageItem);
      }else if (repType == "ssc::GeometricRep")
      {
        QStringList repList(QStringList() << QString((*repIter)->getName().c_str())
                                        << QString((*repIter)->getUid().c_str()));
        QTreeWidgetItem* repItem = new QTreeWidgetItem(repList);
        topLevelItem->addChild(repItem);
        //add meshes under geometricRep
        std::string repUid = (*repIter)->getUid();
        ssc::GeometricRepPtr geometricRep = repManager()->getGeometricRep(repUid);
        if(!geometricRep)
          break;
        ssc::MeshPtr mesh = geometricRep->getMesh();
        QStringList meshList(QStringList() << QString(mesh->getName().c_str())
                                        << QString(mesh->getUid().c_str()));
        QTreeWidgetItem* meshItem = new QTreeWidgetItem(meshList);
        repItem->addChild(meshItem);
      }else if(repType == "ssc::ToolRep3D")
      {
        QStringList repList(QStringList() << QString((*repIter)->getName().c_str())
                                        << QString((*repIter)->getUid().c_str()));
        QTreeWidgetItem* repItem = new QTreeWidgetItem(repList);
        topLevelItem->addChild(repItem);
        //add tools under toolreps
        std::string repUid = (*repIter)->getUid();
        ssc::ToolRep3DPtr toolRep3D = repManager()->getToolRep3DRep(repUid);
        if(!toolRep3D)
          break;
        ssc::ToolPtr tool = toolRep3D->getTool();
        QStringList toolList(QStringList() << QString(tool->getName().c_str())
                                        << QString(tool->getUid().c_str()));
        QTreeWidgetItem* toolItem = new QTreeWidgetItem(toolList);
        repItem->addChild(toolItem);
      }
      repIter++;
    }
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
  }
}
}//end namespace cx
