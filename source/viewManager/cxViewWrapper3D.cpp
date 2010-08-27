/*
 * cxViewWrapper3D.cpp
 *
 *  Created on: Mar 24, 2010
 *      Author: christiana
 */

#include "cxViewWrapper3D.h"
#include <vector>
#include <QSettings>
#include <QAction>
#include <QMenu>
#include <vtkTransform.h>
#include <vtkAbstractVolumeMapper.h>
#include <vtkVolumeMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include "sscView.h"
#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"
#include "sscTool2DRep.h"
#include "sscOrientationAnnotationRep.h"
#include "sscDisplayTextRep.h"
#include "sscMessageManager.h"
#include "sscToolManager.h"
#include "sscSlicePlanes3DRep.h"
#include "sscSlicePlaneClipper.h"
#include "cxLandmarkRep.h"
#include "cxRepManager.h"
#include "cxDataManager.h"

namespace cx
{


ViewWrapper3D::ViewWrapper3D(int startIndex, ssc::View* view)
{
  mView = view;
  this->connectContextMenu(mView);
  std::string index = QString::number(startIndex).toStdString();

  view->getRenderer()->GetActiveCamera()->SetParallelProjection(true);

//  mInteractiveCropper.reset(new InteractiveCropper());
//  mInteractiveCropper->setView(view);

//  mBoxWidget = vtkBoxWidgetPtr::New();
//  mBoxWidget->RotationEnabledOff();
//  mBoxWidget->SetInteractor(view->getRenderWindow()->GetInteractor());
//
////  mBoxRep = vtkBoxRepresentationPtr::New();
////  //boxRep->PlaceWidget(bounds)
////  //view->getRenderer()->AddActor(mBoxRep);
////
////  mBoxWidget = vtkBoxWidget2Ptr::New();
////  mBoxWidget->RotationEnabledOff();
////  mBoxWidget->SetInteractor(view->getRenderWindow()->GetInteractor());
////  mBoxWidget->SetRepresentation(mBoxRep);
//
////  ssc::DoubleBoundingBox3D bb = transform(image->get_rMd(), image->boundingBox());
////  double bb_hard[6] = { -114.775, 114.664,    -250.775, -21.336,      1099.500, 1308.500 };
//  double bb_hard[6] = { -1,1,  -1,1,  -1,1 };
//
//  //double bb_hard[6] = { -0.5,0.5,  -0.5,0.5,  -0.5,0.5 };
////  vtkTransformPtr transform = vtkTransformPtr::New();
////  transform->setMatrix();
//
//  //std::cout << "bb_r: " << bb << std::endl;
////  mBoxRep->PlaceWidget(bb.begin());
//  mBoxWidget->PlaceWidget(bb_hard);
//
//  CropBoxCallback* mCropBoxCallback = new CropBoxCallback("inter");
//  mBoxWidget->AddObserver(vtkCommand::InteractionEvent, mCropBoxCallback);
//  mBoxWidget->AddObserver(vtkCommand::StartInteractionEvent, new CropBoxCallback("start"));
//  mBoxWidget->AddObserver(vtkCommand::EnableEvent, new CropBoxCallback("enable"));
////  mSlicePlaneClipper = ssc::SlicePlaneClipper::New();

//  mBoxWidget = vtkBoxWidgetPtr::New();
//  mBoxWidget->RotationEnabledOff();
//  mBoxWidget->SetInteractor(view->getRenderWindow()->GetInteractor());
//
////  mBoxRep = vtkBoxRepresentationPtr::New();
////  //boxRep->PlaceWidget(bounds)
////  //view->getRenderer()->AddActor(mBoxRep);
////
////  mBoxWidget = vtkBoxWidget2Ptr::New();
////  mBoxWidget->RotationEnabledOff();
////  mBoxWidget->SetInteractor(view->getRenderWindow()->GetInteractor());
////  mBoxWidget->SetRepresentation(mBoxRep);
//
////  ssc::DoubleBoundingBox3D bb = transform(image->get_rMd(), image->boundingBox());
//  double bb_hard[6] = { -114.775, 114.664,    -250.775, -21.336,      1099.500, 1308.500 };
//
//  //std::cout << "bb_r: " << bb << std::endl;
////  mBoxRep->PlaceWidget(bb.begin());
//  mBoxWidget->PlaceWidget(bb_hard);
//
//
////  mSlicePlaneClipper = ssc::SlicePlaneClipper::New();
//>>>>>>> Stashed changes

//  mVolumetricRep = repManager()->getVolumetricRep("VolumetricRep_"+index);
  mLandmarkRep = repManager()->getLandmarkRep("LandmarkRep_"+index);
  mProbeRep = repManager()->getProbeRep("ProbeRep_"+index);
  mGeometricRep = repManager()->getGeometricRep("GeometricRep_"+index);

  // plane type text rep
  mPlaneTypeText = ssc::DisplayTextRep::New("planeTypeRep_"+mView->getName(), "");
  mPlaneTypeText->addText(ssc::Vector3D(0,1,0), "3D", ssc::Vector3D(0.98, 0.02, 0.0));
  mView->addRep(mPlaneTypeText);

  //data name text rep
  mDataNameText = ssc::DisplayTextRep::New("dataNameText_"+mView->getName(), "");
  mDataNameText->addText(ssc::Vector3D(0,1,0), "not initialized", ssc::Vector3D(0.02, 0.02, 0.0));
  mView->addRep(mDataNameText);

  connect(ssc::toolManager(), SIGNAL(initialized()), this, SLOT(toolsAvailableSlot()));
  this->toolsAvailableSlot();
}

//void ViewWrapper3D::startBoxInteraction()
//{
//  if (mImage.empty())
//    return;
//  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
//  if (!image)
//    image = mImage.front();
//  double bb_hard[6] = { -0.5,0.5,  -0.5,0.5,  -0.5,0.5 };
//  //double bb_hard[6] = { -1,1,  -1,1,  -1,1 };
//  ssc::DoubleBoundingBox3D bb_unit(bb_hard);
//  ssc::DoubleBoundingBox3D bb = transform(image->get_rMd(), image->boundingBox());
//  ssc::Transform3D M = ssc::createTransformNormalize(bb_unit, bb);
////  std::cout << "BB_image_d " << image->boundingBox() << std::endl;
////  std::cout << "BB_image_r " << bb << std::endl;
////return;
//  //double bb_hard[6] = { -114.775, 114.664,    -250.775, -21.336,      1099.500, 1308.500 };
//  std::cout << "BB_image_d " << image->boundingBox() << std::endl;
//  std::cout << "BB_image_r " << bb << std::endl;
//
//  vtkTransformPtr transform = vtkTransformPtr::New();
//  transform->SetMatrix(M.matrix());
//  mBoxWidget->SetTransform(transform);
//
//  //std::cout << "bb_r: " << bb << std::endl;
////  mBoxRep->PlaceWidget(bb.begin());
////  mBoxWidget->PlaceWidget(bb_hard);
//}

ViewWrapper3D::~ViewWrapper3D()
{
  if (mView)
    mView->removeReps();
}

void ViewWrapper3D::appendToContextMenu(QMenu& contextMenu)
{
  QAction* slicePlanesAction = new QAction("Show Slice Planes", &contextMenu);
  slicePlanesAction->setCheckable(true);
  //obliqueAction->setData(qstring_cast(ssc::otOBLIQUE));
  slicePlanesAction->setChecked(mSlicePlanes3DRep->getProxy()->getVisible());
  connect(slicePlanesAction, SIGNAL(triggered(bool)),
          this, SLOT(showSlicePlanesActionSlot(bool)));

  QAction* fillSlicePlanesAction = new QAction("Fill Slice Planes", &contextMenu);
  fillSlicePlanesAction->setCheckable(true);
  fillSlicePlanesAction->setEnabled(mSlicePlanes3DRep->getProxy()->getVisible());
  //obliqueAction->setData(qstring_cast(ssc::otOBLIQUE));
  fillSlicePlanesAction->setChecked(mSlicePlanes3DRep->getProxy()->getDrawPlanes());
  connect(fillSlicePlanesAction, SIGNAL(triggered(bool)),
          this, SLOT(fillSlicePlanesActionSlot(bool)));

  contextMenu.addSeparator();
  contextMenu.addAction(slicePlanesAction);
  contextMenu.addAction(fillSlicePlanesAction);

//  for (VolumetricRepMap::iterator iter = mVolumetricReps.begin(); iter!=mVolumetricReps.end(); ++iter)
//  {
//    QAction* clipAction = new QAction("Clip " + qstring_cast(iter->second->getImage()->getName()), &contextMenu);
//    clipAction->setCheckable(true);
//    clipAction->setData(qstring_cast(iter->first));
//    clipAction->setChecked(mSlicePlaneClipper->getVolumes().count(iter->second));
//    connect(clipAction, SIGNAL(triggered()),
//            this, SLOT(clipActionSlot()));
//    contextMenu.addAction(clipAction);
//  }
}

void ViewWrapper3D::clipActionSlot()
{
//  QAction* action = dynamic_cast<QAction*>(sender());
//  if (!action)
//    return;
//  std::string uid = string_cast(action->data().toString());
//
//  ssc::SliceProxyPtr slicer = mSlicePlanes3DRep->getProxy()->getData()[ssc::ptCORONAL].mSliceProxy;
//  mSlicePlaneClipper->setSliceProxy(slicer);
//  if (action->isChecked())
//    mSlicePlaneClipper->addVolume(mVolumetricReps[uid]);
//  else
//    mSlicePlaneClipper->removeVolume(mVolumetricReps[uid]);
}

void ViewWrapper3D::showSlicePlanesActionSlot(bool checked)
{
  mSlicePlanes3DRep->getProxy()->setVisible(checked);
}
void ViewWrapper3D::fillSlicePlanesActionSlot(bool checked)
{
  mSlicePlanes3DRep->getProxy()->setDrawPlanes(checked);
}

void ViewWrapper3D::addImage(ssc::ImagePtr image)
{
  if (!image)
  {
    return;
  }

  if (std::count(mImage.begin(), mImage.end(), image))
  {
    return;
  }

  if (!image->getRefVtkImageData().GetPointer())
  {
    ssc::messageManager()->sendWarning("ViewManager::setImage vtk image missing from current image!");
    return;
  }

  mImage.push_back(image);

  if (!mVolumetricReps.count(image->getUid()))
  {
//    std::string uid("VolumetricRep_" + image->getUid());
//    ssc::VolumetricRepPtr rep = ssc::VolumetricRep::New(uid, uid);
//    mVolumetricReps[image->getUid()] = rep;
////    //Shading
////    if (QSettings().value("shadingOn").toBool())
////      rep->getVtkVolume()->GetProperty()->ShadeOn();
//
//    rep->setImage(image);
    ssc::VolumetricRepPtr rep = repManager()->getVolumetricRep(image);

//    vtkPlanePtr plane = vtkPlanePtr::New();
//    plane->SetNormal(0,0,1);
//    plane->SetOrigin(100,100,100);
//    std::cout << "coronal: <" << n << "> <" << p << ">" << std::endl;
//    std::cout << "BB: " << iter->second->getImage()->boundingBox() << std::endl;
    //std::cout << "adding cutplane for image" << std::endl;

    //rep->getVtkVolume()->GetMapper()->RemoveAllClippingPlanes();
    //rep->getVtkVolume()->GetMapper()->AddClippingPlane(plane);


    mVolumetricReps[image->getUid()] = rep;
    mView->addRep(rep);
    emit imageAdded(image->getUid().c_str());
  }

  mProbeRep->setImage(image);
  mLandmarkRep->setImage(image);

  updateView();

//  ssc::DoubleBoundingBox3D bb = transform(image->get_rMd(), image->boundingBox());
//  double bb_hard[6] = { -114.775, 114.664,    -250.775, -21.336,      1099.500, 1308.500 };
//
//  std::cout << "bb_r: " << bb << std::endl;
////  mBoxRep->PlaceWidget(bb.begin());
//  mBoxRep->PlaceWidget(bb_hard);
//<<<<<<< Updated upstream
////  this->startBoxInteraction();
//=======
//>>>>>>> Stashed changes


  mView->getRenderer()->ResetCamera();
//  if (mView->isVisible())
//    mView->getRenderWindow()->Render();
}

void ViewWrapper3D::updateView()
{
  //update data name text rep
  QStringList text;
  for (unsigned i = 0; i < mImage.size(); ++i)
  {
    text << qstring_cast(mImage[i]->getName());
  }
  mDataNameText->setText(0, string_cast(text.join("\n")));
}

void ViewWrapper3D::removeImage(ssc::ImagePtr image)
{
  if (!image)
    return;
  if (!mVolumetricReps.count(image->getUid()))
    return;
  if (!std::count(mImage.begin(), mImage.end(), image))
    return;
  mImage.erase(std::find(mImage.begin(), mImage.end(), image));

  ssc::messageManager()->sendDebug("Remove image from view group 3d: "+image->getName());
  mView->removeRep(mVolumetricReps[image->getUid()]);
  mVolumetricReps.erase(image->getUid());

  this->updateView();

  emit imageRemoved(qstring_cast(image->getUid()));
}


void ViewWrapper3D::addMesh(ssc::MeshPtr mesh)
{
  if (!mesh)
    return;
  
  mMesh = mesh;
  //mMeshes.push(mesh);

  mGeometricRep->setMesh(mesh);
  
  //emit imageChanged(image->getUid().c_str());
  
  mView->addRep(mGeometricRep);
  mView->getRenderer()->ResetCamera();
  if(mView->isVisible())
    mView->getRenderWindow()->Render();
}

std::vector<ssc::ImagePtr> ViewWrapper3D::getImages() const
{
  return mImage;
}

ssc::MeshPtr ViewWrapper3D::getMesh() const
{
  return mMesh;
}
  
ssc::View* ViewWrapper3D::getView()
{
  return mView;
}

void ViewWrapper3D::dominantToolChangedSlot()
{
  ssc::ToolPtr dominantTool = ssc::toolManager()->getDominantTool();
  mProbeRep->setTool(dominantTool);
  //std::cout << "ViewWrapper3D::dominantToolChangedSlot(): " << dominantTool.get() << std::endl;
}


void ViewWrapper3D::toolsAvailableSlot()
{
 // std::cout <<"void ViewWrapper3D::toolsAvailableSlot() " << std::endl;
  // we want to do this also when nonconfigured and manual tool is present
//  if (!toolManager()->isConfigured())
//    return;

  ssc::ToolManager::ToolMapPtr tools = ssc::toolManager()->getTools();
  ssc::ToolManager::ToolMapPtr::value_type::iterator iter;
  for (iter=tools->begin(); iter!=tools->end(); ++iter)
  {
    if(iter->second->getType() == ssc::Tool::TOOL_REFERENCE)
      continue;

    std::string uid = iter->second->getUid()+"_rep3d_"+this->mView->getUid();
    if (!mToolReps.count(uid))
    {
      mToolReps[uid] = ssc::ToolRep3D::New(uid);
      repManager()->addToolRep3D(mToolReps[uid]);
    }
    ssc::ToolRep3DPtr toolRep = mToolReps[uid];
//    std::cout << "setting 3D tool rep for " << iter->second->getName() << std::endl;
    toolRep->setTool(iter->second);
    toolRep->setOffsetPointVisibleAtZeroOffset(true);
    mView->addRep(toolRep);
    ssc::messageManager()->sendDebug("ToolRep3D for tool "+iter->second->getName()+" added to view "+mView->getName()+".");
  }

//
//  ToolRep3DMap* toolRep3DMap = repManager()->getToolRep3DReps();
//  ToolRep3DMap::iterator repIt = toolRep3DMap->begin();
//  ssc::ToolManager::ToolMapPtr tools = toolManager()->getTools();
//  ssc::ToolManager::ToolMap::iterator toolIt = tools->begin();
//
//  while((toolIt != tools->end()) && (repIt != toolRep3DMap->end()))
//  {
//    if(toolIt->second->getType() != ssc::Tool::TOOL_REFERENCE)
//    {
//      std::cout << "setting tool rep " << toolIt->second->getName() << std::endl;
//      repIt->second->setTool(toolIt->second);
//      mView->addRep(repIt->second);
//      repIt++;
//    }
//    toolIt++;
//  }
}


void ViewWrapper3D::setRegistrationMode(ssc::REGISTRATION_STATUS mode)
{
  if (mode==ssc::rsNOT_REGISTRATED)
  {
    mView->removeRep(mLandmarkRep);
    mView->removeRep(mProbeRep);
    
    disconnect(ssc::toolManager(), SIGNAL(dominantToolChanged(const std::string&)), this, SLOT(dominantToolChangedSlot()));
  }
  if (mode==ssc::rsIMAGE_REGISTRATED)
  {
    mView->addRep(mLandmarkRep);
    mView->addRep(mProbeRep);

    connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const std::string&)), this, SLOT(dominantToolChangedSlot()));
    this->dominantToolChangedSlot();
  }
  if (mode==ssc::rsPATIENT_REGISTRATED)
  {
    mView->addRep(mLandmarkRep);
  }
}

//void ViewWrapper3D::slicePlanesChangedSlot()
//{
//  if (!mSlicePlanes3DRep->getProxy()->getData().count(ssc::ptCORONAL))
//    return;
////  ssc::Transform3D sMr = mSlicePlanes3DRep->getProxy()->getData()[ssc::ptCORONAL].mSliceProxy->get_sMr();
//
//  SlicePlaneClipperPtr clipper = SlicePlaneClipper::New();
//  clipper->setProxy(mSlicePlanes3DRep->getProxy()->getData()[ssc::ptCORONAL].mSliceProxy);
//  clipper->addVolumes();
//
//  for (std::map<std::string, ssc::VolumetricRepPtr>::iterator iter=mVolumetricReps.begin(); iter!=mVolumetricReps.end(); ++iter)
//  {
//    clipper->addVolume(iter->second);
////    ssc::Transform3D rMs = sMr.inv();
////
////    ssc::Vector3D n = rMs.vector(ssc::Vector3D(0,0,-1));
////    ssc::Vector3D p = rMs.coord(ssc::Vector3D(0,0,0));
////    vtkPlanePtr plane = vtkPlanePtr::New();
////    plane->SetNormal(n.begin());
////    plane->SetOrigin(p.begin());
//////    std::cout << "coronal: <" << n << "> <" << p << ">" << std::endl;
//////    std::cout << "BB: " << iter->second->getImage()->boundingBox() << std::endl;
//////
////    iter->second->getVtkVolume()->GetMapper()->RemoveAllClippingPlanes();
////    iter->second->getVtkVolume()->GetMapper()->AddClippingPlane(plane);
//  }
//}

void ViewWrapper3D::setSlicePlanesProxy(ssc::SlicePlanesProxyPtr proxy)
{
  mSlicePlanes3DRep = ssc::SlicePlanes3DRep::New("uid");
  mSlicePlanes3DRep->setProxy(proxy);
  mView->addRep(mSlicePlanes3DRep);
  //connect(mSlicePlanes3DRep->getProxy().get(), SIGNAL(changed()), this, SLOT(slicePlanesChangedSlot()));
}


//------------------------------------------------------------------------------


}
