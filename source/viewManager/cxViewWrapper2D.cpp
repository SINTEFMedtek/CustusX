/*
 * cxViewWrapper2D.cpp
 *
 *  Created on: Mar 24, 2010
 *      Author: christiana
 */

#include "cxViewWrapper2D.h"
#include <vector>
#include <vtkRenderWindow.h>
#include "sscView.h"
#include "sscTypeConversions.h"
#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"
#include "sscTool2DRep.h"
#include "sscOrientationAnnotationRep.h"
#include "sscDisplayTextRep.h"
#include "cxRepManager.h"
#include "cxDataManager.h"
#include "cxMessageManager.h"
#include "cxInriaRep2D.h"
#include "cxLandmarkRep.h"
#include "cxToolManager.h"

namespace cx
{

std::string planeToString(ssc::PLANE_TYPE val)
{
  switch (val)
  {
  case ssc::ptNOPLANE      : return "";
  case ssc::ptSAGITTAL     : return "Sagittal";
  case ssc::ptCORONAL      : return "Coronal";
  case ssc::ptAXIAL        : return "Axial";
  case ssc::ptANYPLANE     : return "Any";
  case ssc::ptSIDEPLANE    : return "Side";
  case ssc::ptRADIALPLANE  : return "Radial";
  default                  : return "";
  }
}


ViewWrapper2D::ViewWrapper2D(ssc::View* view)
{
  mView = view;
  // disable vtk interactor: this wrapper IS an interactor
  mView->getRenderWindow()->GetInteractor()->Disable();

  // annotation rep
  mOrientationAnnotationRep = ssc::OrientationAnnotationRep::New("annotationRep_"+mView->getName(), "annotationRep_"+mView->getName());
  //(handled by init) mOrientationAnnotationRep->setPlaneType(planeType[i]);
  mView->addRep(mOrientationAnnotationRep);

  // plane type text rep
  mPlaneTypeText = ssc::DisplayTextRep::New("planeTypeRep_"+mView->getName(), "");
  //mPlaneTypeText->addText(ssc::Vector3D(0,1,0), planeToString(planeType[i]), ssc::Vector3D(0.98, 0.02, 0.0));
  mPlaneTypeText->addText(ssc::Vector3D(0,1,0), "not initialized", ssc::Vector3D(0.98, 0.02, 0.0));
  mView->addRep(mPlaneTypeText);

  // slice proxy
  mSliceProxy = ssc::SliceProxy::New("sliceproxy_("+ mView->getName() +")");
  //(handled by init) mSliceProxy->initializeFromPlane(planeType[i], false, ssc::Vector3D(0,0,1), false, 1, 0.25);
  //mElements[i].mSliceProxy->setTool(...);

  // slice rep
  mSliceRep = ssc::SliceRepSW::New("SliceRep_"+mView->getName());
  //std::cout << "setSliceProxy 2dviewgroup" << std::endl;
  mSliceRep->setSliceProxy(mSliceProxy);
  mView->addRep(mSliceRep);

  // tool rep
  mToolRep2D = ssc::ToolRep2D::New("Tool2D_"+mView->getName());
  mToolRep2D->setSliceProxy(mSliceProxy);
  mToolRep2D->setUseCrosshair(true);
  mView->addRep(mToolRep2D);

  connect(ToolManager::getInstance(), SIGNAL(dominantToolChanged(const std::string&)), this, SLOT(dominantToolChangedSlot()));
  dominantToolChangedSlot();

  connect(mView, SIGNAL(resized(QSize)), this, SLOT(viewportChanged()));
  connect(mView, SIGNAL(mouseWheelSignal(QWheelEvent*)), this, SLOT(viewportChanged()));
  connect(mView, SIGNAL(showSignal(QShowEvent*)), this, SLOT(showSlot()));
  connect(mView, SIGNAL(mouseReleaseSignal(QMouseEvent*)), this, SLOT(mouseReleaseSlot(QMouseEvent*)));
}

ssc::Vector3D ViewWrapper2D::viewToDisplay(ssc::Vector3D p_v) const
{
  vtkRendererPtr renderer = mView->getRenderer();
  renderer->SetViewPoint(p_v.begin());
  renderer->ViewToDisplay();///pang
  ssc::Vector3D p_d(renderer->GetDisplayPoint());
  return p_d;
}

ssc::Vector3D ViewWrapper2D::displayToWorld(ssc::Vector3D p_d) const
{
  vtkRendererPtr renderer = mView->getRenderer();
  //if ()
  renderer->SetDisplayPoint(p_d.begin());
  //renderer->SetDisplayPoint(0,0,0);
  renderer->DisplayToWorld();///pang
  double* p_wH = renderer->GetWorldPoint();
  ssc::Vector3D p_w = ssc::Vector3D(p_wH)/p_wH[3]; // convert from homogenous to cartesan coords
  return p_w;
}

void ViewWrapper2D::viewportChanged()
{
  //std::cout << "ViewWrapper2D::viewportChanged()" << std::endl;

  if (!mView->getRenderer()->IsActiveCameraCreated())
    return;
  std::cout << "ViewWrapper2D::viewportChanged() with camera, pt=" << planeToString(mPlaneType) << std::endl;

  // world == slice
  // display == vp

  QSize size = mView->size();

  ssc::Vector3D p0_d(0,0,0);
  ssc::Vector3D p1_d(size.width(), size.height(), 0);

  ssc::Vector3D p0_w = displayToWorld(p0_d);
  ssc::Vector3D p1_w = displayToWorld(p1_d);

  ssc::DoubleBoundingBox3D BB_vp(p0_d, p1_d);
  ssc::DoubleBoundingBox3D BB_s(p0_w, p1_w);

  ssc::Transform3D vpMs = get_vpMs();

  mToolRep2D->setViewportData(vpMs, BB_vp);
}

ssc::DoubleBoundingBox3D ViewWrapper2D::getViewport() const
{
  QSize size = mView->size();
  ssc::Vector3D p0_d(0,0,0);
  ssc::Vector3D p1_d(size.width(), size.height(), 0);
  ssc::DoubleBoundingBox3D BB_vp(p0_d, p1_d);
  return BB_vp;
}

ssc::Transform3D ViewWrapper2D::get_vpMs() const
{
  QSize size = mView->size();

  ssc::Vector3D p0_d(0,0,0);
  ssc::Vector3D p1_d(size.width(), size.height(), 1);

  ssc::Vector3D p0_w = displayToWorld(p0_d);
  ssc::Vector3D p1_w = displayToWorld(p1_d);

  ssc::DoubleBoundingBox3D BB_vp(p0_d, p1_d);
  ssc::DoubleBoundingBox3D BB_s(p0_w, p1_w);

  ssc::Transform3D vpMs = ssc::createTransformNormalize(BB_s, BB_vp);
  return vpMs;
}

void ViewWrapper2D::showSlot()
{
  //TODO: need better camera control than this...
  std::cout << "stuff fixed - show" << std::endl;
  mView->getRenderer()->ResetCamera();
  viewportChanged();
}

void ViewWrapper2D::initializePlane(ssc::PLANE_TYPE plane)
{
  mPlaneType = plane;
  mOrientationAnnotationRep->setPlaneType(plane);
  mPlaneTypeText->setText(0, planeToString(plane));
  mSliceProxy->initializeFromPlane(plane, false, ssc::Vector3D(0,0,1), false, 1, 0.25);
}

ssc::View* ViewWrapper2D::getView()
{
  return mView;
}

void ViewWrapper2D::setImage(ssc::ImagePtr image)
{
  if (!image)
    return;
  ssc::Vector3D c = image->get_rMd().coord(image->boundingBox().center());
  mSliceProxy->setDefaultCenter(c);
  mSliceRep->setImage(image);
}

void ViewWrapper2D::removeImage(ssc::ImagePtr image)
{
  mSliceRep->setImage(ssc::ImagePtr());
}

void ViewWrapper2D::dominantToolChangedSlot()
{
  ssc::ToolPtr dominantTool = ToolManager::getInstance()->getDominantTool();
  mSliceProxy->setTool(dominantTool);
  std::cout << "ViewWrapper2D::dominantToolChangedSlot(): " << dominantTool.get() << std::endl;

  //connect(dominantTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(fixStuff()));

}

void ViewWrapper2D::mouseReleaseSlot(QMouseEvent* event)
{
  ssc::ManualToolPtr tool = ToolManager::getInstance()->getManualTool();

  ssc::Transform3D sMr = mSliceProxy->get_sMr();
  ssc::Transform3D rMpr = *ToolManager::getInstance()->get_rMpr();
  ssc::Transform3D prMt = tool->get_prMt();

  // find tool position in s
  ssc::Vector3D tool_t(0,0,tool->getTooltipOffset());
  ssc::Vector3D tool_s = (sMr*rMpr*prMt).coord(tool_t);

  // find click position in s.
  QPoint click_q = event->pos();
  QSize size = mView->size();
  ssc::Vector3D click_vp(click_q.x(), size.height()-click_q.y(), 0.0); // convert from left-handed qt space to vtk space display/viewport
  ssc::Vector3D click_s = get_vpMs().inv().coord(click_vp);

  // compute the new tool position in slice space as a synthesis of the plane part of click and the z part of original.
  ssc::Vector3D cross_s(click_s[0], click_s[1], tool_s[2]);
  // compute the position change and transform to patient.
  ssc::Vector3D delta_s = cross_s - tool_s;
  ssc::Vector3D delta_pr = (rMpr.inv()*sMr.inv()).vector(delta_s);

  // MD is the actual tool movement in patient space, matrix form
  ssc::Transform3D MD = createTransformTranslate(delta_pr);
  // set new tool position to old modified by MD:
  tool->set_prMt(MD*prMt);

//  std::cout << "--- mouse click ---" << std::endl;
//  std::cout << "get_vpMs()\n" << get_vpMs() << std::endl;
//  std::cout << "get_vpMs().inv()\n" << get_vpMs().inv() << std::endl;
//  std::cout << "center_r\t" << DataManager::getInstance()->getCenter() << std::endl;
//  std::cout << "click_vp\t" << click_vp << std::endl;
//  std::cout << "click_s \t" << click_s << std::endl;
//  std::cout << "tool_s  \t" << tool_s << std::endl;
}



//------------------------------------------------------------------------------


}
