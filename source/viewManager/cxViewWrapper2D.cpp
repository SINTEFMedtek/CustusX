/*
 * cxViewWrapper2D.cpp
 *
 *  Created on: Mar 24, 2010
 *      Author: christiana
 */

#include "cxViewWrapper2D.h"
#include <vector>
#include <vtkRenderWindow.h>
#include "sscUtilHelpers.h"
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
#include "cxViewManager.h"
#include "cxInriaRep2D.h"
#include "cxLandmarkRep.h"
#include "cxToolManager.h"

namespace cx
{

//std::string planeToString(ssc::PLANE_TYPE val)
//{
//  switch (val)
//  {
//  case ssc::ptNOPLANE      : return "";
//  case ssc::ptSAGITTAL     : return "Sagittal";
//  case ssc::ptCORONAL      : return "Coronal";
//  case ssc::ptAXIAL        : return "Axial";
//  case ssc::ptANYPLANE     : return "Any";
//  case ssc::ptSIDEPLANE    : return "Side";
//  case ssc::ptRADIALPLANE  : return "Radial";
//  default                  : return "";
//  }
//}


ViewWrapper2D::ViewWrapper2D(ssc::View* view) :
    mOrientationActionGroup(new QActionGroup(view)),
    mGlobal2DZoomActionGroup(new QActionGroup(view))
{
  mView = view;
  this->connectContextMenu(mView);
  mZoomFactor = 0;

  // disable vtk interactor: this wrapper IS an interactor
  mView->getRenderWindow()->GetInteractor()->Disable();
  mView->getRenderer()->GetActiveCamera()->SetParallelProjection(true);

  addReps();

  connect(toolManager(), SIGNAL(dominantToolChanged(const std::string&)), this, SLOT(dominantToolChangedSlot()));
  connect(mView, SIGNAL(resized(QSize)), this, SLOT(viewportChanged()));
  connect(mView, SIGNAL(showSignal(QShowEvent*)), this, SLOT(showSlot()));
  connect(mView, SIGNAL(mousePressSignal(QMouseEvent*)), this, SLOT(mousePressSlot(QMouseEvent*)));
  connect(mView, SIGNAL(mouseWheelSignal(QWheelEvent*)), this, SLOT(mouseWheelSlot(QWheelEvent*)));
}

void ViewWrapper2D::appendToContextMenu(QMenu& contextMenu)
{
  //messageManager()->sendInfo("ViewWrapper2D::appendToContextMenu");

  QAction* obliqueAction = new QAction("Oblique", &contextMenu);
  obliqueAction->setCheckable(true);
  obliqueAction->setData(qstring_cast(ssc::otOBLIQUE));
  obliqueAction->setChecked(getOrientationType()==ssc::otOBLIQUE);

  QAction* ortogonalAction = new QAction("Ortogonal", &contextMenu);
  ortogonalAction->setCheckable(true);
  ortogonalAction->setData(qstring_cast(ssc::otORTHOGONAL));
  ortogonalAction->setChecked(getOrientationType()==ssc::otORTHOGONAL);
  //ortogonalAction->setChecked(true);

  mOrientationActionGroup->addAction(obliqueAction);
  mOrientationActionGroup->addAction(ortogonalAction);

  QAction* global2DZoomAction = new QAction("Global 2D Zoom", &contextMenu);
  global2DZoomAction->setCheckable(true);
  global2DZoomAction->setChecked(viewManager()->getGlobal2DZoom());
  mGlobal2DZoomActionGroup->addAction(global2DZoomAction);

  contextMenu.addSeparator();
  contextMenu.addAction(obliqueAction);
  contextMenu.addAction(ortogonalAction);
  contextMenu.addSeparator();
  contextMenu.addAction(global2DZoomAction);
}

void ViewWrapper2D::checkFromContextMenu(QAction* theAction, QActionGroup* theActionGroup)
{
  //messageManager()->sendInfo("ViewWrapper2D::checkFromContextMenu");

  if(theActionGroup == mOrientationActionGroup)
  {
    //messageManager()->sendInfo("Clicked Oblique!");
    //mOblique = !mOblique;
    ssc::ORIENTATION_TYPE type = string2enum<ssc::ORIENTATION_TYPE>(string_cast(theAction->data().toString()));
    //std::cout << "ortype: " << type << std::endl;
    changeOrientationType(type);
  }
  else if(theActionGroup == mGlobal2DZoomActionGroup)
  {
    messageManager()->sendInfo("Clicked global 2D zoom!");
    viewManager()->setGlobal2DZoom(!viewManager()->getGlobal2DZoom());
  }
}

void ViewWrapper2D::addReps()
{
  // annotation rep
  mOrientationAnnotationRep = ssc::OrientationAnnotationRep::New("annotationRep_"+mView->getName(), "annotationRep_"+mView->getName());
  mView->addRep(mOrientationAnnotationRep);

  // plane type text rep
  mPlaneTypeText = ssc::DisplayTextRep::New("planeTypeRep_"+mView->getName(), "");
  mPlaneTypeText->addText(ssc::Vector3D(0,1,0), "not initialized", ssc::Vector3D(0.98, 0.02, 0.0));
  mView->addRep(mPlaneTypeText);

  //data name text rep
  mDataNameText = ssc::DisplayTextRep::New("dataNameText_"+mView->getName(), "");
  mDataNameText->addText(ssc::Vector3D(0,1,0), "not initialized", ssc::Vector3D(0.02, 0.02, 0.0));
  mView->addRep(mDataNameText);

  // slice proxy
  mSliceProxy = ssc::SliceProxy::New("sliceproxy_("+ mView->getName() +")");

  // slice rep
  mSliceRep = ssc::SliceRepSW::New("SliceRep_"+mView->getName());
  mSliceRep->setSliceProxy(mSliceProxy);
  mView->addRep(mSliceRep);

  // tool rep
  mToolRep2D = ssc::ToolRep2D::New("Tool2D_"+mView->getName());
  mToolRep2D->setSliceProxy(mSliceProxy);
  mToolRep2D->setUseCrosshair(true);
  mView->addRep(mToolRep2D);
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
  renderer->SetDisplayPoint(p_d.begin());
  renderer->DisplayToWorld();
  double* p_wH = renderer->GetWorldPoint();
  ssc::Vector3D p_w = ssc::Vector3D(p_wH)/p_wH[3]; // convert from homogenous to cartesan coords
  return p_w;
}

/**Call when viewport size or zoom has changed.
 * Recompute camera zoom and  reps requiring vpMs.
 */
void ViewWrapper2D::viewportChanged()
{
  //std::cout << "ViewWrapper2D::viewportChanged()" << std::endl;

  if (!mView->getRenderer()->IsActiveCameraCreated())
    return;
//  std::cout << "ViewWrapper2D::viewportChanged() with camera, pt=" << planeToString(mPlaneType) << std::endl;

  double parallelScale = mView->heightMM() / 2.0 / mZoomFactor;
  mView->getRenderer()->GetActiveCamera()->SetParallelScale(parallelScale);
  //std::cout << "ViewWrapper2D::viewportChanged(): zoom:" << mZoomFactor << ", parallelScale:" << parallelScale << ", " << planeToString(mPlaneType) <<  std::endl;

  ssc::DoubleBoundingBox3D BB_vp = getViewport();
  ssc::Transform3D vpMs = get_vpMs();

  mToolRep2D->setViewportData(vpMs, BB_vp);
}

/**Return the viewport in vtk pixels. (viewport space)
 */
ssc::DoubleBoundingBox3D ViewWrapper2D::getViewport() const
{
  QSize size = mView->size();
  ssc::Vector3D p0_d(0,0,0);
  ssc::Vector3D p1_d(size.width(), size.height(), 0);
  ssc::DoubleBoundingBox3D BB_vp(p0_d, p1_d);
  return BB_vp;
}

/**Compute transform from slice space (vtk world/ssc after slicing) to vtk viewport.
 */
ssc::Transform3D ViewWrapper2D::get_vpMs() const
{
  // world == slice
  // display == vp

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
  dominantToolChangedSlot();
  viewportChanged();
}

void ViewWrapper2D::initializePlane(ssc::PLANE_TYPE plane)
{
//  mPlaneType = plane;
  mOrientationAnnotationRep->setPlaneType(plane);
  mPlaneTypeText->setText(0, string_cast(plane));
  mSliceProxy->initializeFromPlane(plane, false, ssc::Vector3D(0,0,1), false, 1, 0.25);
}

ssc::ORIENTATION_TYPE ViewWrapper2D::getOrientationType() const
{
  return mSliceProxy->getComputer().getOrientationType();
}

void ViewWrapper2D::changeOrientationType(ssc::ORIENTATION_TYPE type)
{
  ssc::SliceComputer computer = mSliceProxy->getComputer();
  computer.switchOrientationMode(type);

  ssc::PLANE_TYPE plane = computer.getPlaneType();
  mOrientationAnnotationRep->setPlaneType(plane);
  mPlaneTypeText->setText(0, string_cast(plane));
  mSliceProxy->setComputer(computer);
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

  //update data name text rep
  mDataNameText->setText(0, image->getName());
}

void ViewWrapper2D::removeImage(ssc::ImagePtr image)
{
  mSliceRep->setImage(ssc::ImagePtr());
}

void ViewWrapper2D::dominantToolChangedSlot()
{
  ssc::ToolPtr dominantTool = ToolManager::getInstance()->getDominantTool();
  mSliceProxy->setTool(dominantTool);
}

void ViewWrapper2D::setZoom2D(double zoomFactor)
{
  zoomFactor = ssc::constrainValue(zoomFactor, 0.2, 10.0);

  if(zoomFactor == mZoomFactor)
    return;

  mZoomFactor = zoomFactor;
  emit zoom2DChange(mZoomFactor);

  //std::cout << "VIEWWRAPPER: zoom changed: " + string_cast(mZoomFactor) << std::endl;

  this->viewportChanged();
}

double ViewWrapper2D::getZoom2D()
{
  return mZoomFactor;
}

/**Part of the mouse interactor:
 * Move manual tool tip when mouse pressed
 *
 */
void ViewWrapper2D::mousePressSlot(QMouseEvent* event)
{
  if (event->buttons() & Qt::LeftButton)
  {
    moveAxisPos(qvp2vp(event->pos()));
  }
}

/**Part of the mouse interactor:
 * Interpret mouse wheel as a zoom operation.
 */
void ViewWrapper2D::mouseWheelSlot(QWheelEvent* event)
{
  // scale zoom in log space
  double val = log10(mZoomFactor);
  val += event->delta()/120.0 / 20.0; // 120 is normal scroll resolution, x is zoom resolution
  double newZoom = pow(10.0, val);

  this->setZoom2D(newZoom);
}

/**Convert a position in Qt viewport space (pixels with origin in upper-left corner)
 * to vtk viewport space (pixels with origin in lower-left corner).
 */
ssc::Vector3D ViewWrapper2D::qvp2vp(QPoint pos_qvp)
{
  QSize size = mView->size();
  ssc::Vector3D pos_vp(pos_qvp.x(), size.height()-pos_qvp.y(), 0.0); // convert from left-handed qt space to vtk space display/viewport
  return pos_vp;
}

/**Move the tool pos / axis pos to a new position given
 * by the input click position in vp space.
 */
void ViewWrapper2D::moveAxisPos(ssc::Vector3D click_vp)
{
  ssc::ManualToolPtr tool = ToolManager::getInstance()->getManualTool();

  ssc::Transform3D sMr = mSliceProxy->get_sMr();
  ssc::Transform3D rMpr = *ToolManager::getInstance()->get_rMpr();
  ssc::Transform3D prMt = tool->get_prMt();

  // find tool position in s
  ssc::Vector3D tool_t(0,0,tool->getTooltipOffset());
  ssc::Vector3D tool_s = (sMr*rMpr*prMt).coord(tool_t);

  // find click position in s.
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
}



//------------------------------------------------------------------------------


}
