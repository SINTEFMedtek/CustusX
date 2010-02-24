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
  mMakeVolumetricRepMenu(new QMenu(tr("Change volumetric representation of...") ,mContextMenu)),
  //mMakeGeometricRepMenu(new QMenu(tr("Change geometric representation of...") ,mContextMenu)),
  mCameraStyle(DEFAULT_STYLE),
  mCameraOffset(-1),
  mDataManager(DataManager::getInstance()),
  mRepManager(RepManager::getInstance()),
  mMessageManager(MessageManager::getInstance())
{
  mUid = uid;
  mName = name;

  mContextMenu->addMenu(mMakeVolumetricRepMenu);
  //mContextMenu->addMenu(mMakeGeometricRepMenu);

  mRenderer->GetActiveCamera()->SetClippingRange(1, 2000);
}
View3D::~View3D()
{}
void View3D::contextMenuEvent(QContextMenuEvent *event)
{
  //NOT SUPPORTING MESHES IN 3D VIEW YET

    mMakeVolumetricRepMenu->clear();
    //mMakeGeometricRepMenu->clear();

    //mMakeGeometricRepMenu->setEnabled(false); //TODO remove when we know what to do with meshes

    //Get a list of available image and meshes names
    std::map<std::string, std::string> imageUidsAndNames = mDataManager->getImageUidsAndNames();
    std::map<std::string, std::string> meshUidsAndNames = mDataManager->getMeshUidsWithNames();

    //Display the lists to the user
    //Extract to own function if often reused...
    std::map<std::string, std::string>::iterator itImages = imageUidsAndNames.begin();
    while(itImages != imageUidsAndNames.end())
    {
      const QString id = itImages->first.c_str();
      QAction* imageIdAction = new QAction(id, mMakeVolumetricRepMenu);
      mMakeVolumetricRepMenu->addAction(imageIdAction);
      itImages++;
    }
    /*std::map<std::string, std::string>::iterator itMeshes = meshUidsAndNames.begin();
    while(itMeshes != meshUidsAndNames.end())
    {
      const QString id = itMeshes->first.c_str();
      QAction* meshIdAction = new QAction(id, mMakeGeometricRepMenu);
      mMakeGeometricRepMenu->addAction(meshIdAction);
      itMeshes++;
    }*/

    //Find out which the user chose
    //TODO: IMAGE OR MESH??? theAction->parent()?
    QAction* theAction = mContextMenu->exec(event->globalPos());
    if(theAction == NULL)
      return;

    std::string actionId = theAction->text().toStdString();
    std::string info = "Making a volumetricrep of data with name: "+actionId;
    mMessageManager->sendInfo(info);

    //Make a volumetric rep out of the image
    ssc::VolumetricRepPtr volumetricRep = mRepManager->getVolumetricRep("VolumetricRep_1");//TODO: REMOVE HACK???
    ssc::ImagePtr image = mDataManager->getImage(actionId);
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
  ssc::Transform3DPtr rMpr = ToolManager::getInstance()->get_rMpr();
  ssc::Transform3DPtr rMt(new ssc::Transform3D((*rMpr)*prMt));
  
  /*ssc::Vector3D cameraPoint = ssc::Vector3D((*rMt)[0][3],(*rMt)[1][3],(*rMt)[2][3])*mCameraOffset;
  ssc::Vector3D pos = ssc::Vector3D((*rMt)[0][3],(*rMt)[1][3],(*rMt)[2][3])-cameraPoint;
  ssc::Vector3D focalPoint = pos + ssc::Vector3D((*rMt)[0][3],(*rMt)[1][3],(*rMt)[2][3]);*/

  /*ssc::Transform3D prMo(vtkMatrix4x4::New());
  prMo[2][3] = mCameraOffset; //offset is only applied in the tools z direction
  ssc::Transform3D rMo = (*rMpr)*prMo;*/
  
  //ssc::Transform3D prMt_withOffset = prMt;
  //ssc::Transform3D prMt_withOffset = prMt;
  //prMt_withOffset[2][3] = prMt_withOffset[2][3]+mCameraOffset; //offset is only applied in the tools z(?) direction
  ssc::Vector3D cameraPoint_t = ssc::Vector3D(0,0,mCameraOffset);
  ssc::Vector3D cameraPoint_r = prMt.coord(cameraPoint_t);

  double xCameraPos = cameraPoint_r[0];
  double yCameraPos = cameraPoint_r[1];
  double zCameraPos = cameraPoint_r[2];
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
  //dominantToolRepPtr->setStayHiddenAfterVisible(true);

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
    followingToolRepPtr->setOffsetPointVisibleAtZeroOffset(false);
}
}//namespace cx
