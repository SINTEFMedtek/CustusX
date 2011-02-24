/*
 * cxViewWrapper2D.cpp
 *
 *  Created on: Mar 24, 2010
 *      Author: christiana
 */

#include "cxViewWrapper2D.h"
#include <vector>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include "sscUtilHelpers.h"
#include "sscView.h"
#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"
#include "sscTool2DRep.h"
#include "sscOrientationAnnotationRep.h"
#include "sscDisplayTextRep.h"
#include "sscMessageManager.h"
#include "cxRepManager.h"
#include "sscDataManager.h"
#include "cxViewManager.h"
#include "cxToolManager.h"
#include "cxViewGroup.h"
#include "sscDefinitionStrings.h"
#include "sscSlicePlanes3DRep.h"
#include "sscDefinitionStrings.h"
#include "sscSliceComputer.h"
#include "sscGeometricRep2D.h"
#include "sscTexture3DSlicerRep.h"

#ifdef USE_GLX_SHARED_CONTEXT
  #define USE_2D_GPU_RENDER
#endif

namespace cx
{

ViewWrapper2D::ViewWrapper2D(ssc::View* view) :
    mOrientationActionGroup(new QActionGroup(view))
{
//  std::cout << "ViewWrapper2D create" << std::endl;
  mView = view;
  this->connectContextMenu(mView);

  // disable vtk interactor: this wrapper IS an interactor
  mView->getRenderWindow()->GetInteractor()->Disable();
  mView->getRenderer()->GetActiveCamera()->SetParallelProjection(true);
  double clipDepth = 1.0; // 1mm depth, i.e. all 3D props rendered outside this range is not shown.
  mView->getRenderer()->GetActiveCamera()->SetClippingRange(-clipDepth/2.0, clipDepth/2.0);

  addReps();

  setZoom2D(SyncedValue::create(1));
  setOrientationMode(SyncedValue::create(0)); // must set after addreps()

  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot()));
  connect(mView, SIGNAL(resized(QSize)), this, SLOT(viewportChanged()));
  connect(mView, SIGNAL(showSignal(QShowEvent*)), this, SLOT(showSlot()));
  connect(mView, SIGNAL(mousePressSignal(QMouseEvent*)), this, SLOT(mousePressSlot(QMouseEvent*)));
  connect(mView, SIGNAL(mouseMoveSignal(QMouseEvent*)), this, SLOT(mouseMoveSlot(QMouseEvent*)));
  connect(mView, SIGNAL(mouseWheelSignal(QWheelEvent*)), this, SLOT(mouseWheelSlot(QWheelEvent*)));
}

ViewWrapper2D::~ViewWrapper2D()
{
//  std::cout << "ViewWrapper2D delete" << std::endl;
  if (mView)
    mView->removeReps();
}

void ViewWrapper2D::appendToContextMenu(QMenu& contextMenu)
{
  QAction* obliqueAction = new QAction("Oblique", &contextMenu);
  obliqueAction->setCheckable(true);
  obliqueAction->setData(qstring_cast(ssc::otOBLIQUE));
  obliqueAction->setChecked(getOrientationType()==ssc::otOBLIQUE);
  connect(obliqueAction, SIGNAL(triggered()),
          this, SLOT(orientationActionSlot()));

  QAction* ortogonalAction = new QAction("Ortogonal", &contextMenu);
  ortogonalAction->setCheckable(true);
  ortogonalAction->setData(qstring_cast(ssc::otORTHOGONAL));
  ortogonalAction->setChecked(getOrientationType()==ssc::otORTHOGONAL);
  //ortogonalAction->setChecked(true);
  connect(ortogonalAction, SIGNAL(triggered()),
          this, SLOT(orientationActionSlot()));

  //TODO remove actiongroups?
  mOrientationActionGroup->addAction(obliqueAction);
  mOrientationActionGroup->addAction(ortogonalAction);

  QAction* global2DZoomAction = new QAction("Global 2D Zoom", &contextMenu);
  global2DZoomAction->setCheckable(true);
  global2DZoomAction->setChecked(viewManager()->getGlobal2DZoom());
  connect(global2DZoomAction, SIGNAL(triggered()),
          this, SLOT(global2DZoomActionSlot()));

  contextMenu.addSeparator();
  contextMenu.addAction(obliqueAction);
  contextMenu.addAction(ortogonalAction);
  contextMenu.addSeparator();
  contextMenu.addAction(global2DZoomAction);
}

/** Slot for the orientation action.
 *  Set the orientation mode.
 */
void ViewWrapper2D::orientationActionSlot()
{
  QAction* theAction = static_cast<QAction*>(sender());
  if(!theAction)
    return;

  ssc::ORIENTATION_TYPE type = string2enum<ssc::ORIENTATION_TYPE>(theAction->data().toString());
  mOrientationMode->set(type);
}

/** Slot for the global zoom action
 *  Set the global zoom flag in the view manager.
 */
void ViewWrapper2D::global2DZoomActionSlot()
{
  QAction* theAction = static_cast<QAction*>(sender());
  if(!theAction)
    return;

  viewManager()->setGlobal2DZoom(!viewManager()->getGlobal2DZoom());
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
#ifdef USE_2D_GPU_RENDER
//  this->resetMultiSlicer(); ignore until addimage

//  mMultiSliceRep = ssc::Texture3DSlicerRep::New("MultiSliceRep_"+mView->getName());
//  mMultiSliceRep->setShaderFile("/home/christiana/christiana/workspace/CustusX3/CustusX3/externals/ssc/Sandbox/Texture3DOverlay.frag");
//  mMultiSliceRep->setSliceProxy(mSliceProxy);
//  mView->addRep(mMultiSliceRep);
#else
  mSliceRep = ssc::SliceRepSW::New("SliceRep_"+mView->getName());
  mSliceRep->setSliceProxy(mSliceProxy);
  mView->addRep(mSliceRep);
#endif

  // tool rep
  mToolRep2D = ssc::ToolRep2D::New("Tool2D_"+mView->getName());
  mToolRep2D->setSliceProxy(mSliceProxy);
  mToolRep2D->setUseCrosshair(true);
  mView->addRep(mToolRep2D);
}

/**Hack: gpu slicer recreate and fill with images every time,
 * due to internal instabilities.
 *
 */
void ViewWrapper2D::resetMultiSlicer()
{
  if (mMultiSliceRep)
    mView->removeRep(mMultiSliceRep);
  mMultiSliceRep = ssc::Texture3DSlicerRep::New("MultiSliceRep_"+mView->getName());
  mMultiSliceRep->setShaderFile("/home/christiana/christiana/workspace/CustusX3/CustusX3/externals/ssc/Sandbox/Texture3DOverlay.frag");
  mMultiSliceRep->setSliceProxy(mSliceProxy);
  mView->addRep(mMultiSliceRep);
  if (mViewGroup)
    mMultiSliceRep->setImages(mViewGroup->getImages());
  this->viewportChanged();
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
  if (!mView->getRenderer()->IsActiveCameraCreated())
    return;

  mView->setZoomFactor(mZoom2D->get().toDouble());

  double viewHeight = mView->heightMM() / getZoomFactor2D();
//  double parallelScale = mView->heightMM() / 2.0 / getZoomFactor2D();
  mView->getRenderer()->GetActiveCamera()->SetParallelScale(viewHeight/2);

  mSliceProxy->setToolViewportHeight(viewHeight);
//std::cout << "mView->heightMM() " << mView->heightMM() << ", " << mView->height() << "," << mView->physicalDpiY() << std::endl;
//
//int width() const { return metric(PdmWidth); }
//int height() const { return metric(PdmHeight); }
//int widthMM() const { return metric(PdmWidthMM); }
//int heightMM() const { return metric(PdmHeightMM); }
//int logicalDpiX() const { return metric(PdmDpiX); }
//int logicalDpiY() const { return metric(PdmDpiY); }
//int physicalDpiX() const { return metric(PdmPhysicalDpiX); }
//int physicalDpiY() const { return metric(PdmPhysicalDpiY); }

//  vtkCameraPtr camera = mView->getRenderer()->GetActiveCamera();
//  std::cout << ssc::Vector3D(camera->GetFocalPoint()) << std::endl;
//  std::cout << camera->GetClippingRange()[0] << ", " << camera->GetClippingRange()[1] << std::endl;
//  std::cout << camera->GetDistance() << std::endl;

  //std::cout << "ViewWrapper2D::viewportChanged(): zoom:" << mZoomFactor << ", parallelScale:" << parallelScale << ", " << planeToString(mPlaneType) <<  std::endl;

  ssc::DoubleBoundingBox3D BB_vp = getViewport();
  ssc::Transform3D vpMs = get_vpMs();
  ssc::PLANE_TYPE plane = mSliceProxy->getComputer().getPlaneType();

  mToolRep2D->setViewportData(vpMs, BB_vp);
  if (mSlicePlanes3DMarker)
  {
    mSlicePlanes3DMarker->getProxy()->setViewportData(plane, mSliceProxy, transform(vpMs.inv(), BB_vp));
//    std::cout << "-- " << string_cast(plane) << "-----------------------------------------------------" << std::endl;
//    std::cout << "BB_vp" << " -- " << BB_vp << std::endl;
//    std::cout << "vpMs" << " --\n " << vpMs << std::endl;
//    std::cout << "BB_s" << " -- " << transform(vpMs.inv(), BB_vp) << std::endl;
//    std::cout << "-----------------------------------------------------" << std::endl;
  }
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

//  std::cout << " -------------------------------------------- "  << std::endl;
//  std::cout << "p0: " << p0_d << " --- " << p0_w << std::endl;
//  std::cout << "p1: " << p1_d << " --- " << p1_w << std::endl;

  p0_w[2] = 0;
  p1_w[2] = 1;

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
  mOrientationAnnotationRep->setPlaneType(plane);
  mPlaneTypeText->setText(0, qstring_cast(plane));
  double viewHeight = mView->heightMM() / this->getZoomFactor2D();
  mSliceProxy->initializeFromPlane(plane, false, ssc::Vector3D(0,0,1), true, viewHeight, 0.25);

  // do this to force sync global and local type - must think on how we want this to work
  this->changeOrientationType(getOrientationType());
}

/** get the orientation type directly from the slice proxy
 */
ssc::ORIENTATION_TYPE ViewWrapper2D::getOrientationType() const
{
  return mSliceProxy->getComputer().getOrientationType();
}

/** Slot called when the synced orientation has changed.
 *  Update the slice proxy orientation.
 */
void ViewWrapper2D::orientationModeChanged()
{
//  changeOrientationType(static_cast<ssc::ORIENTATION_TYPE>(mOrientationMode->get().toInt()));
//std::cout << "mOrientationModeChanbgedslot" << std::endl;

  ssc::ORIENTATION_TYPE type = static_cast<ssc::ORIENTATION_TYPE>(mOrientationMode->get().toInt());

  if(type == this->getOrientationType())
    return;
  if (!mSliceProxy)
    return;

  ssc::SliceComputer computer = mSliceProxy->getComputer();
  computer.switchOrientationMode(type);

  ssc::PLANE_TYPE plane = computer.getPlaneType();
  mOrientationAnnotationRep->setPlaneType(plane);
  mPlaneTypeText->setText(0, qstring_cast(plane));
  mSliceProxy->setComputer(computer);
}

/** Set the synced orientation mode.
 */
void ViewWrapper2D::changeOrientationType(ssc::ORIENTATION_TYPE type)
{
  mOrientationMode->set(type);
}

ssc::View* ViewWrapper2D::getView()
{
  return mView;
}

/**
 */
void ViewWrapper2D::imageAdded(ssc::ImagePtr image)
{
  this->updateView();

  // removed this side effect: unwanted when loading a patient, might also be unwanted to change scene when adding/removing via gui?
  //Navigation().centerToView(mViewGroup->getImages());
}

void ViewWrapper2D::updateView()
{
  std::vector<ssc::ImagePtr> images = mViewGroup->getImages();
  ssc::ImagePtr image;
  //std::cout << "ViewWrapper2D::updateView() " << images.size() << std::endl;
  if (!images.empty())
    image = images.back(); // always show last in vector

  QString text;

  if (image)
  {
    ssc::Vector3D c = image->get_rMd().coord(image->boundingBox().center());
    mSliceProxy->setDefaultCenter(c);
    text = image->getName();
  }

  // slice rep
#ifdef USE_2D_GPU_RENDER
  this->resetMultiSlicer();
//  mMultiSliceRep->setImages(images);
#else
  mSliceRep->setImage(image);
#endif

  //update data name text rep
  mDataNameText->setText(0, text);
}

void ViewWrapper2D::imageRemoved(const QString& uid)
{
  updateView();
}

void ViewWrapper2D::meshAdded(ssc::MeshPtr mesh)
{
//  std::map<QString, ssc::GeometricRep2DPtr> mGeometricRep;

  if (!mesh)
    return;
  if (mGeometricRep.count(mesh->getUid()))
    return;

  ssc::GeometricRep2DPtr rep = ssc::GeometricRep2D::New(mesh->getUid()+"_rep2D");
  rep->setSliceProxy(mSliceProxy);
  rep->setMesh(mesh);
  mView->addRep(rep);
  mGeometricRep[mesh->getUid()] = rep;
  //std::cout << "added mesh " << mesh->getName() << std::endl;
//
//  std::vector<ssc::MeshPtr> images = mViewGroup->getMeshes();
//
//  // slice rep
//  mSliceRep = ssc::SliceRepSW::New("SliceRep_"+mView->getName());
//  mSliceRep->setSliceProxy(mSliceProxy);
//  mView->addRep(mSliceRep);
}

void ViewWrapper2D::meshRemoved(const QString& uid)
{
  if (!mGeometricRep.count(uid))
    return;

  mView->removeRep(mGeometricRep[uid]);
  mGeometricRep.erase(uid);
 // std::cout << "removed mesh " << uid << std::endl;
}


void ViewWrapper2D::dominantToolChangedSlot()
{
  ssc::ToolPtr dominantTool = ssc::toolManager()->getDominantTool();
  mSliceProxy->setTool(dominantTool);
}

void ViewWrapper2D::setOrientationMode(SyncedValuePtr value)
{
  if (mOrientationMode)
    disconnect(mOrientationMode.get(), SIGNAL(changed()), this, SLOT(orientationModeChanged()));
  mOrientationMode = value;
  if (mOrientationMode)
    connect(mOrientationMode.get(), SIGNAL(changed()), this, SLOT(orientationModeChanged()));

  orientationModeChanged();
}

void ViewWrapper2D::setZoom2D(SyncedValuePtr value)
{
  if (mZoom2D)
    disconnect(mZoom2D.get(), SIGNAL(changed()), this, SLOT(viewportChanged()));
  mZoom2D = value;
  if (mZoom2D)
    connect(mZoom2D.get(), SIGNAL(changed()), this, SLOT(viewportChanged()));

  viewportChanged();
}

void ViewWrapper2D::setZoomFactor2D(double zoomFactor)
{
  zoomFactor = ssc::constrainValue(zoomFactor, 0.2, 10.0);
  mZoom2D->set(zoomFactor);
  viewportChanged();
}

double ViewWrapper2D::getZoomFactor2D() const
{
  return mZoom2D->get().toDouble();
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
 * Move manual tool tip when mouse pressed
 *
 */
void ViewWrapper2D::mouseMoveSlot(QMouseEvent* event)
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
  double val = log10(getZoomFactor2D());
  val += event->delta()/120.0 / 20.0; // 120 is normal scroll resolution, x is zoom resolution
  double newZoom = pow(10.0, val);

  this->setZoomFactor2D(newZoom);

  Navigation().centerToTooltip(); // side effect: center on tool
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
  ssc::Transform3D rMpr = *ssc::toolManager()->get_rMpr();
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

void ViewWrapper2D::setSlicePlanesProxy(ssc::SlicePlanesProxyPtr proxy)
{
  mSlicePlanes3DMarker = ssc::SlicePlanes3DMarkerIn2DRep::New("uid");
  ssc::PLANE_TYPE plane = mSliceProxy->getComputer().getPlaneType();
  mSlicePlanes3DMarker->setProxy(plane, proxy);

  ssc::DoubleBoundingBox3D BB_vp = getViewport();
  ssc::Transform3D vpMs = get_vpMs();
  mSlicePlanes3DMarker->getProxy()->setViewportData(plane, mSliceProxy, transform(vpMs.inv(), BB_vp));

  mView->addRep(mSlicePlanes3DMarker);
}

//------------------------------------------------------------------------------
}
