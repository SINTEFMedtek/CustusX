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
  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot()));
  dominantToolChangedSlot();
}

View3D::~View3D()
{
}

void View3D::setCameraStyle(View3D::CameraStyle style, int offset)
{
//  ssc::messageManager()->sendDebug("View3D is trying to set the camerastyle.");

  if(mCameraStyle == style)
    return;

  switch(style)
  {
  case View3D::DEFAULT_STYLE:
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
  if(offset != -1)
    mCameraOffset = std::max<int>(offset, 1.0);
}

void View3D::moveCameraToolStyleSlot(Transform3D prMt, double timestamp)
{
  ssc::Transform3D rMpr = *ssc::toolManager()->get_rMpr();

  ssc::Transform3D rMt = rMpr * prMt;

  ssc::Vector3D camera_r = rMt.coord(ssc::Vector3D(0,0,-mCameraOffset));
  ssc::Vector3D focus_r = rMt.coord(ssc::Vector3D(0,0,0));
  ssc::Vector3D vup_r = rMt.vector(ssc::Vector3D(-1,0,0));

  vtkCamera* camera = mRenderer->GetActiveCamera();
  camera->SetPosition(camera_r.begin());
  camera->SetFocalPoint(focus_r.begin());
  camera->SetViewUp(vup_r.begin());

  camera->SetClippingRange(1, 2000);
}

void View3D::activateCameraDefaultStyle()
{
  this->disconnectTool();

  mRenderer->ResetCamera();
  mRenderer->GetActiveCamera()->SetClippingRange(1, 2000);

  mCameraStyle = View3D::DEFAULT_STYLE;

  ssc::messageManager()->sendInfo("Default camera style activated.");
}

void View3D::activateCameraToolStyle(int offset)
{
  this->setCameraOffsetSlot(offset);
  mCameraStyle = View3D::TOOL_STYLE;
  this->connectTool();
  ssc::messageManager()->sendInfo("Tool camera style activated.");
}

void View3D::dominantToolChangedSlot()
{
  ssc::ToolPtr newTool = ssc::toolManager()->getDominantTool();
  if (newTool==mFollowingTool)
    return;
  if (mCameraStyle!=View3D::TOOL_STYLE)
    return;

  this->disconnectTool();
  this->connectTool();
}

void View3D::connectTool()
{
  mFollowingTool = ssc::toolManager()->getDominantTool();

  if (mFollowingTool)
  {

    //Need the toolrep to get the direction the camera should point in
    ssc::ToolRep3DPtr rep = repManager()->findFirstRep<ssc::ToolRep3D>(this->getReps(), mFollowingTool);

    if(!rep)
    {
//      ssc::messageManager()->sendWarning("The tool you are requesting to follow does not have a rep associated with it.");
      return; //cannot set the camera to follow a tool if that tool dose not have a rep
    }

    connect(mFollowingTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(moveCameraToolStyleSlot(Transform3D, double)));

    rep->setOffsetPointVisibleAtZeroOffset(true);
    rep->setStayHiddenAfterVisible(true);

    ssc::messageManager()->sendInfo("Tool camera style: Following tool with uid: "+mFollowingTool->getUid());
  }
}

void View3D::disconnectTool()
{
  if (mFollowingTool)
  {
    disconnect(mFollowingTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(moveCameraToolStyleSlot(Transform3D, double)));

    ssc::ToolRep3DPtr toolRep = repManager()->findFirstRep<ssc::ToolRep3D>(this->getReps(), mFollowingTool);
    if(toolRep)
      toolRep->setStayHiddenAfterVisible(false);
  }

  mFollowingTool.reset();
}


}//namespace cx
