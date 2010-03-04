#include "cxView3D.h"

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include "sscImage.h"
#include "sscVolumetricRep.h"
#include "cxDataManager.h"
#include "cxRepManager.h"
#include "cxMessageManager.h"
#include "cxToolManager.h"

namespace cx
{
/**
 * \param uid the views unique id
 * \param name the views name
 * \param parent the views parent
 * \param f flags
 */
View3D::View3D(const std::string& uid, const std::string& name, QWidget *parent, Qt::WFlags f) :
  ssc::View(parent, f),
  mContextMenu(new QMenu(this)),
  mCameraStyle(DEFAULT_STYLE),
  mCameraOffset(-1),
  mDataManager(DataManager::getInstance()),
  mRepManager(RepManager::getInstance()),
  mMessageManager(MessageManager::getInstance())
{
  mUid = uid;
  mName = name;

  mRenderer->GetActiveCamera()->SetClippingRange(1, 2000);
}
View3D::~View3D()
{}
void View3D::contextMenuEvent(QContextMenuEvent *event)
{
  //NOT SUPPORTING MESHES IN 3D VIEW YET

    mContextMenu->clear();

    //Get a list of available image and meshes names
    std::map<std::string, std::string> imageUidsAndNames = mDataManager->getImageUidsAndNames();
    std::map<std::string, std::string> meshUidsAndNames = mDataManager->getMeshUidsWithNames();

    //Display the lists to the user
    std::map<std::string, std::string>::iterator imageIt = imageUidsAndNames.begin();
    while(imageIt != imageUidsAndNames.end())
    {
      const QString uid = imageIt->first.c_str();
      const QString name = imageIt->second.c_str();
      QAction* imageAction = new QAction(name, mContextMenu);
      imageAction->setStatusTip(uid.toStdString().c_str());
      mContextMenu->addAction(imageAction);
      imageIt++;
    }

    //Find out which the user chose
    //TODO: IMAGE OR MESH??? theAction->parent()?
    QAction* theAction = mContextMenu->exec(event->globalPos());
    if(!theAction)//this happens if you rightclick in the view and then don't select a action
      return;
  
    QString imageName = theAction->text();
    QString imageUid = theAction->statusTip();
    ssc::ImagePtr image = mDataManager->getImage(imageUid.toStdString());
  
    if(!image)
    {
      std::string error = "Couldn't find image with uid "+imageUid.toStdString()+" to set in View2D.";
      MessageManager::getInstance()->sendError(error);
      return;
    }

    //Make a volumetric rep out of the image
    ssc::VolumetricRepPtr volumetricRep = mRepManager->getVolumetricRep("VolumetricRep_1");
    volumetricRep->setImage(image);

    //Show the rep in this view
    this->setRep(volumetricRep);
}
void View3D::setCameraStyle(View3D::CameraStyle style, int offset)
{
  if(mCameraStyle == style)
    return;

  switch(style)
  {
  case View3D::DEFAULT_STYLE:
    if(mCameraStyle == View3D::TOOL_STYLE)
      this->deactivateCameraToolStyle(); //need to disconnect some signals
    this->activateCameraDefaultStyle();
    break;
  case View3D::TOOL_STYLE:
    this->activateCameraToolStyle(offset);
    break;
  default:
    break;
  };
}
void View3D::setCameraOffsetSlot(int offset)
{
  mCameraOffset = offset;
}
void View3D::moveCameraToolStyleSlot(Transform3D prMt, double timestamp)
{
  //TODO: work in progress, its not working as intended yet

  ssc::Vector3D cameraPoint_t = ssc::Vector3D(0,0,-mCameraOffset); //the camerapoint in tool space
  ssc::Transform3D cameraTransform_pr = prMt*ssc::createTransformTranslate(cameraPoint_t);
  ssc::Vector3D cameraPoint_pr = ssc::Vector3D(cameraTransform_pr[0][3],
      cameraTransform_pr[1][3], cameraTransform_pr[2][3]); //the camera point in patient ref space
  ssc::Transform3DPtr rMpr = ToolManager::getInstance()->get_rMpr();
  ssc::Transform3D cameraPoint_r = (*rMpr)*ssc::createTransformTranslate(cameraPoint_pr);

  ssc::Transform3DPtr rMt(new ssc::Transform3D((*rMpr)*prMt));

  double xCameraPos = cameraPoint_r[0][3];
  double yCameraPos = cameraPoint_r[1][3];
  double zCameraPos = cameraPoint_r[2][3];
  double xFocalPos = (*rMt)[0][3];
  double yFocalPos = (*rMt)[1][3];
  double zFocalPos = (*rMt)[2][3];
  
  //std::cout << rMt_withOffset << std::endl;
  //std::cout << (*rMt) << std::endl;

  vtkCamera* camera = mRenderer->GetActiveCamera();
  camera->SetPosition(xCameraPos, yCameraPos, zCameraPos);
  camera->SetFocalPoint(xFocalPos, yFocalPos, zFocalPos);
  camera->SetClippingRange(1, 2000);
}
void View3D::activateCameraDefaultStyle()
{
  mRenderer->ResetCamera();
  mRenderer->GetActiveCamera()->SetClippingRange(1, 2000);

  mCameraStyle = View3D::DEFAULT_STYLE;
}
void View3D::activateCameraToolStyle(int offset)
{
  if(offset != -1)
    mCameraOffset = offset;

  ssc::ToolPtr dominantToolPtr = ToolManager::getInstance()->getDominantTool();
  if(!dominantToolPtr)
    return;

  //Need the toolrep to get the direction the camera should point in
  ssc::ToolRep3DPtr dominantToolRepPtr;
  ToolRep3DMap* toolRep3DMap = mRepManager->getToolRep3DReps();
  ToolRep3DMap::iterator it = toolRep3DMap->begin();
  while(it != toolRep3DMap->end())
  {
    if(it->second->hasTool(dominantToolPtr) && //there must be a rep for the dominant tool
       this->hasRep(it->second)) //and this view must have it
    {
      dominantToolRepPtr = it->second;
    }
    it++;
  }
  if(!dominantToolRepPtr)
    return; //cannot set the camera to follow a tool if that tool dose not have a rep

  mFollowingTool = dominantToolPtr;
  connect(mFollowingTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)),
          this, SLOT(moveCameraToolStyleSlot(Transform3D, double)));

  dominantToolRepPtr->setOffsetPointVisibleAtZeroOffset(true);
  dominantToolRepPtr->setStayHiddenAfterVisible(true);

  mCameraStyle = View3D::TOOL_STYLE;
}
void View3D::deactivateCameraToolStyle()
{
  //make sure we disconnect from the right tool, which might not be the current
  //dominant tool
  if(!mFollowingTool)
    return;

  disconnect(mFollowingTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)),
      this, SLOT(moveCameraToolStyleSlot(Transform3D, double)));

  ssc::ToolRep3DPtr followingToolRepPtr = mRepManager->getToolRep3DRep(mFollowingTool->getUid());
  if(followingToolRepPtr)
  {
    followingToolRepPtr->setOffsetPointVisibleAtZeroOffset(false);
    followingToolRepPtr->setStayHiddenAfterVisible(false);
  }
}
}//namespace cx
