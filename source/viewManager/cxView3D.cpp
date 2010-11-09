#include "cxView3D.h"

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include "sscImage.h"
#include "sscVolumetricRep.h"
#include "sscMessageManager.h"
#include "cxRepManager.h"
#include "sscToolManager.h"
#include "sscToolRep3D.h"
#include "sscTypeConversions.h"

namespace cx
{
/**
 * \param uid the views unique id
 * \param name the views name
 * \param parent the views parent
 * \param f flags
 */
View3D::View3D(const QString& uid, const QString& name, QWidget *parent, Qt::WFlags f) :
  ssc::View(parent, f),
  mCameraStyle(DEFAULT_STYLE),
  mCameraOffset(-1)
{
  mUid = uid;
  mName = name;
  this->setContextMenuPolicy(Qt::CustomContextMenu);

  mRenderer->GetActiveCamera()->SetClippingRange(1, 2000);
}
View3D::~View3D()
{
}

void View3D::setCameraStyle(View3D::CameraStyle style, int offset)
{
  ssc::messageManager()->sendDebug("View3D is trying to set the camerastyle.");

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
  ssc::Vector3D cameraPoint_t = ssc::Vector3D(0,0,-mCameraOffset); //the camerapoint in tool space
  ssc::Transform3D cameraTransform_pr = prMt*ssc::createTransformTranslate(cameraPoint_t);
  ssc::Vector3D cameraPoint_pr = ssc::Vector3D(cameraTransform_pr[0][3],
      cameraTransform_pr[1][3], cameraTransform_pr[2][3]); //the camera point in patient ref space
  ssc::Transform3DPtr rMpr = ssc::toolManager()->get_rMpr();
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

  ssc::messageManager()->sendDebug("Default camera style activated.");
}
void View3D::activateCameraToolStyle(int offset)
{
  if(offset != -1)
    mCameraOffset = offset;

  ssc::ToolPtr dominantToolPtr = ssc::toolManager()->getDominantTool();
  if(!dominantToolPtr)
    return;

  //Need the toolrep to get the direction the camera should point in
  ssc::ToolRep3DPtr dominantToolRepPtr;
  ToolRep3DMap* toolRep3DMap = repManager()->getToolRep3DReps();
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
  {
    ssc::messageManager()->sendWarning("The tool you are requesting to follow does not have a rep associated with it.");
    return; //cannot set the camera to follow a tool if that tool dose not have a rep
  }

  mFollowingTool = dominantToolPtr;
  connect(mFollowingTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)),
          this, SLOT(moveCameraToolStyleSlot(Transform3D, double)));

  std::cout << "dominant: " << dominantToolRepPtr->getTool()->getName() << std::endl;
  dominantToolRepPtr->setOffsetPointVisibleAtZeroOffset(true);
  dominantToolRepPtr->setStayHiddenAfterVisible(true);

  mCameraStyle = View3D::TOOL_STYLE;
  ssc::messageManager()->sendDebug("Tool camera style activated. Following tool with uid: "+mFollowingTool->getUid());
}
void View3D::deactivateCameraToolStyle()
{
  //make sure we disconnect from the right tool, which might not be the current
  //dominant tool
  if(!mFollowingTool)
    return;

  disconnect(mFollowingTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)),
      this, SLOT(moveCameraToolStyleSlot(Transform3D, double)));

  ToolRep3DMap* toolRep3DMap = repManager()->getToolRep3DReps();
  ToolRep3DMap::iterator it = toolRep3DMap->begin();
  while(it != toolRep3DMap->end())
  {
    if(it->second->hasTool(mFollowingTool))
    {
      //it->second->setOffsetPointVisibleAtZeroOffset(false);
      it->second->setStayHiddenAfterVisible(false);
    }
    ++it;
  }

  /*ssc::ToolRep3DPtr followingToolRepPtr = repManager()->getToolRep3DRep("ToolRep3D_1");
  if(followingToolRepPtr)
  {
    followingToolRepPtr->setOffsetPointVisibleAtZeroOffset(false);
    followingToolRepPtr->setStayHiddenAfterVisible(false);

  }*/

  ssc::messageManager()->sendDebug("Default camera style deactivated.");
}
}//namespace cx
