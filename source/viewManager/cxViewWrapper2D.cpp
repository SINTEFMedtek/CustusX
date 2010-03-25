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
  //viewportChanged();
}

ssc::Vector3D ViewWrapper2D::viewToDisplay(ssc::Vector3D p_v)
{
  vtkRendererPtr renderer = mView->getRenderer();
  renderer->SetViewPoint(p_v.begin());
  renderer->ViewToDisplay();///pang
  ssc::Vector3D p_d(renderer->GetDisplayPoint());
  return p_d;
}

ssc::Vector3D ViewWrapper2D::displayToWorld(ssc::Vector3D p_d)
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
  // coordinates in display space (pixels, right-handed)
  ssc::Vector3D p0_d = viewToDisplay(ssc::Vector3D(0,0,0));
  ssc::Vector3D p0_w = displayToWorld(p0_d);
  ssc::Vector3D p1_w = displayToWorld(p0_d+ssc::Vector3D(1,0,0));
  ssc::Vector3D p2_w = displayToWorld(p0_d+ssc::Vector3D(0,1,0));

  ssc::Vector3D ex_w = p1_w - p0_w;
  ssc::Vector3D ey_w = p2_w - p0_w;

  std::cout << "ez_w: " << cross(ex_w, ey_w) << std::endl;

  //std::cout << ": " << vp << std::endl;

  // eller motsatt....
  ssc::Transform3D vpMs = ssc::createTransformIJC(ex_w, ey_w, p0_w);

  //ssc::DoubleBoundingBox3D vp(mView->getRenderer()->GetViewport());
  QSize size = mView->size();
  ssc::DoubleBoundingBox3D vp(0,size.width(), 0, size.height(), 0, 1);

  std::cout << "vp: " << vp << std::endl;

  mToolRep2D->setViewportData(vpMs, vp);
}

void ViewWrapper2D::fixStuff()
{
  std::cout << "stuff fixed" << std::endl;
  mView->getRenderer()->ResetCamera();
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

  connect(dominantTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(fixStuff()));

}


//------------------------------------------------------------------------------


}
