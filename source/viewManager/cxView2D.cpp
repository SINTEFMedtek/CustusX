#include "cxView2D.h"

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include "cxDataManager.h"
#include "cxRepManager.h"
#include "cxMessageManager.h"
#include "cxViewManager.h"

namespace cx
{
/**
 * \param uid the views unique id
 * \param name the views name
 * \param parent the views parent
 * \param f flags
 */
View2D::View2D(const std::string& uid, const std::string& name,
    QWidget *parent, Qt::WFlags f) :
  View(parent, f), mContextMenu(new QMenu(this)), mDataManager(
      DataManager::getInstance()), mRepManager(RepManager::getInstance())
{
  mUid = uid;
  mName = name;
}
View2D::~View2D()
{
}
void View2D::contextMenuEvent(QContextMenuEvent *event)
{
  //NOT SUPPORTING MESHES IN 2D VIEW YET

  mContextMenu->clear();

  //Get a list of available image and meshes uids and names
  std::map<std::string, std::string> imageUidsAndNames =
      mDataManager->getImageUidsAndNames();

  //Display the lists to the user
  std::map<std::string, std::string>::iterator imageIt =
      imageUidsAndNames.begin();
  while (imageIt != imageUidsAndNames.end())
  {
    const QString uid = imageIt->first.c_str();
    const QString name = imageIt->second.c_str();
    QAction* imageAction = new QAction(name, mContextMenu);
    imageAction->setStatusTip(uid.toStdString().c_str());
    mContextMenu->addAction(imageAction);
    imageIt++;
  }

  //Find out which the user chose
  QAction* theAction = mContextMenu->exec(event->globalPos());
  if (!theAction)
  {
    MessageManager::getInstance()->sendError("Could not figure out what the user clicked on in the view2d contextmenu.");
    return;
  }

  QString imageName = theAction->text();
  QString imageUid = theAction->statusTip();
  ssc::ImagePtr image = mDataManager->getImage(imageUid.toStdString());

  if(!image)
  {
    std::string error = "Couldn't find image with uid "+imageUid.toStdString()+" to set in View2D.";
    MessageManager::getInstance()->sendError(error);
    return;
  }

  //check which view this is
  if(mUid == "View2D_1" |
     mUid == "View2D_2" |
     mUid == "View2D_3")
  {
    InriaRep2DPtr inriaRep2D_1 = mRepManager->getInria2DRep("InriaRep2D_1");
    InriaRep2DPtr inriaRep2D_2 = mRepManager->getInria2DRep("InriaRep2D_2");
    InriaRep2DPtr inriaRep2D_3 = mRepManager->getInria2DRep("InriaRep2D_3");
    inriaRep2D_1->setImage(image);
    inriaRep2D_2->setImage(image);
    inriaRep2D_3->setImage(image);

    View2D* view2D_1 = ViewManager::getInstance()->get2DView("View2D_1");
    View2D* view2D_2 = ViewManager::getInstance()->get2DView("View2D_2");
    View2D* view2D_3 = ViewManager::getInstance()->get2DView("View2D_3");
    view2D_1->setRep(inriaRep2D_1);
    view2D_2->setRep(inriaRep2D_2);
    view2D_3->setRep(inriaRep2D_3);

    inriaRep2D_1->getVtkViewImage2D()->SetOrientation(vtkViewImage2D::AXIAL_ID);
    inriaRep2D_2->getVtkViewImage2D()->SetOrientation(vtkViewImage2D::CORONAL_ID);
    inriaRep2D_3->getVtkViewImage2D()->SetOrientation(vtkViewImage2D::SAGITTAL_ID);
    inriaRep2D_1->getVtkViewImage2D()->AddChild(inriaRep2D_2->getVtkViewImage2D());
    inriaRep2D_2->getVtkViewImage2D()->AddChild(inriaRep2D_3->getVtkViewImage2D());
    inriaRep2D_3->getVtkViewImage2D()->AddChild(inriaRep2D_1->getVtkViewImage2D());
    inriaRep2D_1->getVtkViewImage2D()->SyncRemoveAllDataSet();
    inriaRep2D_1->getVtkViewImage2D()->SyncAddDataSet(image->getRefVtkImageData());
    inriaRep2D_1->getVtkViewImage2D()->SyncReset();
  }else if(mUid == "View2D_4" |
           mUid == "View2D_5" |
           mUid == "View2D_6")
  {
    InriaRep2DPtr inriaRep2D_4 = mRepManager->getInria2DRep("InriaRep2D_4");
    InriaRep2DPtr inriaRep2D_5 = mRepManager->getInria2DRep("InriaRep2D_5");
    InriaRep2DPtr inriaRep2D_6 = mRepManager->getInria2DRep("InriaRep2D_6");
    inriaRep2D_4->setImage(image);
    inriaRep2D_5->setImage(image);
    inriaRep2D_6->setImage(image);

    View2D* view2D_4 = ViewManager::getInstance()->get2DView("View2D_4");
    View2D* view2D_5 = ViewManager::getInstance()->get2DView("View2D_5");
    View2D* view2D_6 = ViewManager::getInstance()->get2DView("View2D_6");
    view2D_4->setRep(inriaRep2D_4);
    view2D_5->setRep(inriaRep2D_5);
    view2D_6->setRep(inriaRep2D_6);

    inriaRep2D_4->getVtkViewImage2D()->SetOrientation(vtkViewImage2D::AXIAL_ID);
    inriaRep2D_5->getVtkViewImage2D()->SetOrientation(vtkViewImage2D::CORONAL_ID);
    inriaRep2D_6->getVtkViewImage2D()->SetOrientation(vtkViewImage2D::SAGITTAL_ID);
    inriaRep2D_4->getVtkViewImage2D()->AddChild(inriaRep2D_5->getVtkViewImage2D());
    inriaRep2D_5->getVtkViewImage2D()->AddChild(inriaRep2D_6->getVtkViewImage2D());
    inriaRep2D_6->getVtkViewImage2D()->AddChild(inriaRep2D_4->getVtkViewImage2D());
    inriaRep2D_4->getVtkViewImage2D()->SyncRemoveAllDataSet();
    inriaRep2D_4->getVtkViewImage2D()->SyncAddDataSet(image->getRefVtkImageData());
    inriaRep2D_4->getVtkViewImage2D()->SyncReset();
  }
}

}//namespace cx
