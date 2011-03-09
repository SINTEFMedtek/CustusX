/*
 * cxCameraStyle.cpp
 *
 *  Created on: Mar 9, 2011
 *      Author: dev
 */

#include "cxCameraStyle.h"

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
#include "cxViewManager.h"

namespace cx
{

/**
 */
CameraStyle::CameraStyle() :
  mCameraStyle(DEFAULT_STYLE),
  mCameraOffset(-1)
{
//  mRenderer->GetActiveCamera()->SetClippingRange(1, 2000);
  connect(viewManager(), SIGNAL(activeLayoutChanged()), this, SLOT(viewChangedSlot()));

  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot()));
  dominantToolChangedSlot();
}

View3D* CameraStyle::getView() const
{
  return viewManager()->get3DView();
}

ssc::ToolRep3DPtr CameraStyle::getToolRep() const
{
  if (!this->getView())
    return ssc::ToolRep3DPtr();

  ssc::ToolRep3DPtr rep = repManager()->findFirstRep<ssc::ToolRep3D>(this->getView()->getReps(), mFollowingTool);
  return rep;
}

vtkRendererPtr CameraStyle::getRenderer() const
{
  if (!this->getView())
    return vtkRendererPtr();
  return this->getView()->getRenderer();
}

vtkCameraPtr CameraStyle::getCamera() const
{
  if (!this->getRenderer())
    return vtkCameraPtr();
  return this->getRenderer()->GetActiveCamera();
}

void CameraStyle::setCameraStyle(Style style, int offset)
{
//  ssc::messageManager()->sendDebug("View3D is trying to set the camerastyle.");

  if(mCameraStyle == style)
    return;

  switch(style)
  {
  case DEFAULT_STYLE:
    this->activateCameraDefaultStyle();
    break;
  case TOOL_STYLE:
    this->activateCameraToolStyle(offset);
    break;
  default:
    break;
  };
}

void CameraStyle::setCameraOffsetSlot(int offset)
{
  if(offset != -1)
    mCameraOffset = std::max<int>(offset, 1.0);
}

void CameraStyle::moveCameraToolStyleSlot(ssc::Transform3D prMt, double timestamp)
{
  ssc::Transform3D rMpr = *ssc::toolManager()->get_rMpr();

  ssc::Transform3D rMt = rMpr * prMt;

  double offset = mFollowingTool->getTooltipOffset();

  ssc::Vector3D camera_r = rMt.coord(ssc::Vector3D(0,0,offset-mCameraOffset));
  ssc::Vector3D focus_r = rMt.coord(ssc::Vector3D(0,0,offset));
  ssc::Vector3D vup_r = rMt.vector(ssc::Vector3D(-1,0,0));

  vtkCameraPtr camera = this->getCamera();
  if (!camera)
    return;
  camera->SetPosition(camera_r.begin());
  camera->SetFocalPoint(focus_r.begin());
  camera->SetViewUp(vup_r.begin());

  camera->SetClippingRange(1, 2000);
}

/**reset the view connection, this is in case the view, reps or tool has been deleted/recreated in
 * the layout change process.
 */
void CameraStyle::viewChangedSlot()
{
  this->disconnectTool();
  this->connectTool();
}

void CameraStyle::activateCameraDefaultStyle()
{
  this->disconnectTool();

  if (!this->getRenderer())
    return;

  // removed: this swap confuses surgeon.
  //this->getRenderer()->ResetCamera();
  //this->getCamera()->SetClippingRange(1, 2000);

  mCameraStyle = DEFAULT_STYLE;

  ssc::messageManager()->sendInfo("Default camera style activated.");
}

void CameraStyle::activateCameraToolStyle(int offset)
{
  this->setCameraOffsetSlot(offset);
  mCameraStyle = TOOL_STYLE;
  this->connectTool();
  ssc::messageManager()->sendInfo("Tool camera style activated.");
}

void CameraStyle::dominantToolChangedSlot()
{
  ssc::ToolPtr newTool = ssc::toolManager()->getDominantTool();
  if (newTool==mFollowingTool)
    return;
  if (mCameraStyle!=TOOL_STYLE)
    return;

  this->disconnectTool();
  this->connectTool();
}

void CameraStyle::connectTool()
{
  mFollowingTool = ssc::toolManager()->getDominantTool();

  if (mFollowingTool)
  {
    if (!this->getView())
      return;
    //Need the toolrep to get the direction the camera should point in
    ssc::ToolRep3DPtr rep = this->getToolRep();

    if(!rep)
    {
      return; //cannot set the camera to follow a tool if that tool dose not have a rep
    }

    connect(mFollowingTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(moveCameraToolStyleSlot(Transform3D, double)));

    rep->setOffsetPointVisibleAtZeroOffset(true);
    rep->setStayHiddenAfterVisible(true);

    ssc::messageManager()->sendInfo("Tool camera style: Following tool with uid: "+mFollowingTool->getUid());
  }
}



void CameraStyle::disconnectTool()
{
  if (mFollowingTool)
  {
    disconnect(mFollowingTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(moveCameraToolStyleSlot(Transform3D, double)));

    ssc::ToolRep3DPtr rep = this->getToolRep();
    if(rep)
      rep->setStayHiddenAfterVisible(false);
  }

  mFollowingTool.reset();
}


}//namespace cx
