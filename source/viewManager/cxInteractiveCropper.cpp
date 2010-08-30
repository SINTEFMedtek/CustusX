/*
 * cxInteractiveCropper.cpp
 *
 *  Created on: Aug 24, 2010
 *      Author: christiana
 */
#include "cxInteractiveCropper.h"

#include "sscView.h"

#include <vector>
//#include <QSettings>
//#include <QAction>
//#include <QMenu>
#include <vtkTransform.h>
#include <vtkAbstractVolumeMapper.h>
#include <vtkVolumeMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
//#include "sscView.h"
//#include "sscSliceProxy.h"
//#include "sscSlicerRepSW.h"
//#include "sscTool2DRep.h"
//#include "sscOrientationAnnotationRep.h"
//#include "sscDisplayTextRep.h"
//#include "sscMessageManager.h"
//#include "sscToolManager.h"
//#include "sscSlicePlanes3DRep.h"
//#include "sscSlicePlaneClipper.h"
//#include "cxLandmarkRep.h"
#include "sscTypeConversions.h"
#include "cxRepManager.h"
#include "cxDataManager.h"
#include <vtkBoxWidget2.h>
#include <vtkBoxWidget.h>
//#include <vtkBoxRepresentation.h>
#include "sscBoundingBox3D.h"

namespace cx
{

class CropBoxCallback : public vtkCommand
{
public:
  CropBoxCallback(InteractiveCropper* cropper) : mCropper(cropper) {}
  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    //std::cout << "executing" << std::endl;
    ssc::DoubleBoundingBox3D bb_new = mCropper->getBoxWidgetSize();
    mCropper->setCroppingRegion(bb_new);
  }
  InteractiveCropper* mCropper;
};

class CropBoxEnableCallback : public vtkCommand
{
public:
  CropBoxEnableCallback(bool val, InteractiveCropper* cropper) : mValue(val), mCropper(cropper) {}
  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    //std::cout << "flip" << std::endl;
    ssc::DoubleBoundingBox3D bb_new = mCropper->getBoxWidgetSize();
    mCropper->boxWasShown(mValue);
  }
  bool mValue;
  InteractiveCropper* mCropper;
};


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


InteractiveCropper::InteractiveCropper()
{
  connect(ssc::dataManager(), SIGNAL(activeImageChanged(std::string)), this, SLOT(imageChangedSlot()));
  connect(ssc::dataManager(), SIGNAL(activeImageChanged(std::string)), this, SIGNAL(changed()));

  mBoxWidget = vtkBoxWidgetPtr::New();
  mBoxWidget->RotationEnabledOff();

  double bb_hard[6] = { -1,1,  -1,1,  -1,1 };
  mBoxWidget->PlaceWidget(bb_hard);

  mCropBoxCallback = new CropBoxCallback(this);
  mCropBoxEnableCallback = new CropBoxEnableCallback(true, this);
  mCropBoxDisableCallback = new CropBoxEnableCallback(false, this);
//  mBoxWidget->AddObserver(vtkCommand::InteractionEvent, mCropBoxCallback);

 // mBoxWidget->AddObserver(vtkCommand::StartInteractionEvent, new CropBoxCallback("start"));
 // mBoxWidget->AddObserver(vtkCommand::EnableEvent, new CropBoxCallback("enable"));
 // this->useCropping(true);
}

void InteractiveCropper::setView(ssc::View* view)
{
  mView = view;
  //mBoxWidget->SetInteractor(view->getRenderWindow()->GetInteractor());
  this->updateBoxWidgetInteractor();
}

void InteractiveCropper::updateBoxWidgetInteractor()
{
  if (!mView)
    return;


  if (this->getUseCropping())
  {
    //std::cout << "IInteractiveCropper::updateBoxWidgetInteractor(on)" << std::endl;

  //  mBoxWidget->AddObserver(vtkCommand::InteractionEvent, mCropBoxCallback);
    mBoxWidget->SetInteractor(mView->getRenderWindow()->GetInteractor());
    mBoxWidget->AddObserver(vtkCommand::InteractionEvent, mCropBoxCallback);
    mBoxWidget->AddObserver(vtkCommand::EnableEvent, mCropBoxEnableCallback);
    mBoxWidget->AddObserver(vtkCommand::DisableEvent, mCropBoxDisableCallback);
  }
  else
  {
   // std::cout << "IInteractiveCropper::updateBoxWidgetInteractor(off)" << std::endl;
    mBoxWidget->RemoveObserver(vtkCommand::InteractionEvent);
    mBoxWidget->RemoveObserver(vtkCommand::EnableEvent);
    mBoxWidget->RemoveObserver(vtkCommand::DisableEvent);
   // mBoxWidget->RemoveObserver(vtkCommand::InteractionEvent);
    mBoxWidget->SetInteractor(NULL);
  }
  //std::cout << "Enabled  " << mBoxWidget->GetEnabled() << std::endl;
}

//
//  if (mView)
//  {
//    if (on)
//    {
//    //  mBoxWidget->AddObserver(vtkCommand::InteractionEvent, mCropBoxCallback);
//      mBoxWidget->SetInteractor(mView->getRenderWindow()->GetInteractor());
//    }
//    else
//    {
//     // mBoxWidget->RemoveObserver(vtkCommand::InteractionEvent);
//      mBoxWidget->SetInteractor(NULL);
//    }
//  }


void InteractiveCropper::showBoxWidget(bool on)
{
  //std::cout << "InteractiveCropper::showBoxWidget(" << on << ")" << std::endl;
  //std::cout << "Enabled  " << mBoxWidget->GetEnabled() << std::endl;
//
//  if (on)
//  {
//    mBoxWidget->On();
//  }
//  else
//  {
//    mBoxWidget->Off();
//  }
  mBoxWidget->SetEnabled(on);

  //std::cout << "Enabled  " << mBoxWidget->GetEnabled() << std::endl;
  emit changed();
}

ssc::DoubleBoundingBox3D InteractiveCropper::getBoundingBox()
{
  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
  if (!image)
    return ssc::DoubleBoundingBox3D();

  vtkVolumeMapperPtr mapper = this->getMapper();
  if (!mapper)
    return ssc::DoubleBoundingBox3D();

  ssc::DoubleBoundingBox3D box_d;
  mapper->GetCroppingRegionPlanes(box_d.begin());
  ssc::DoubleBoundingBox3D bb_r = ssc::transform(image->get_rMd(), box_d);

  return bb_r;
}

void InteractiveCropper::setBoundingBox(const ssc::DoubleBoundingBox3D& bb_r)
{
  this->setCroppingRegion(bb_r);
  this->setBoxWidgetSize(bb_r);
//  emit changed();
}

vtkVolumeMapperPtr InteractiveCropper::getMapper()
{
  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
  if (!image)
    return vtkVolumeMapperPtr();
  ssc::VolumetricRepPtr volRep = repManager()->getVolumetricRep(image);

  vtkVolumeMapperPtr mapper = dynamic_cast<vtkVolumeMapper*>(volRep->getVtkVolume()->GetMapper());
  //mapper->Register();
  return mapper;
//  if (!mapper)
//    return;
}

void InteractiveCropper::useCropping(bool on)
{
  if (this->getUseCropping()==on)
    return;

  //return;
  vtkVolumeMapperPtr mapper = this->getMapper();
  if (!mapper)
    return;
  mapper->SetCropping(on);

  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
  if (!image)
    return;

  image->setCropping(on);

  //ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
//  if (!ssc::dataManager()->getActiveImage())
//    return;
//  ssc::DoubleBoundingBox3D bb_r = transform(image->get_rMd(), image->boundingBox());

  //ssc::DoubleBoundingBox3D bb_r = this->getMaxBoundingBox();
  ssc::DoubleBoundingBox3D bb_r = image->getDoubleCroppingBox();

  //this->setBoxWidgetSize(bb_r);
  this->setCroppingRegion(bb_r);
  this->setBoxWidgetSize(bb_r);
  this->updateBoxWidgetInteractor();
//
//  if (mView)
//  {
//    if (on)
//    {
//    //  mBoxWidget->AddObserver(vtkCommand::InteractionEvent, mCropBoxCallback);
//      mBoxWidget->SetInteractor(mView->getRenderWindow()->GetInteractor());
//    }
//    else
//    {
//     // mBoxWidget->RemoveObserver(vtkCommand::InteractionEvent);
//      mBoxWidget->SetInteractor(NULL);
//    }
//  }

//  mBoxWidget->SetEnabled(on);
  //std::cout << "box enabled: " << mBoxWidget->GetEnabled() << std::endl;

//  mBoxWidget->SetEnabled(on);
  //std::cout << "InteractiveCropper::useCropping(" << on << ") test: " << this->getUseCropping() << std::endl;
  //mBoxWidget->SetEnabled(on);

  emit changed();
}

void InteractiveCropper::resetBoundingBox()
{
  emit changed();
}

void InteractiveCropper::imageChangedSlot()
{
  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
  if (!image)
    return;

  this->useCropping(image->getCropping());

//  if (!ssc::dataManager()->getActiveImage())
//    return;
//
//  ssc::DoubleBoundingBox3D bb_r = getMaxBoundingBox();
//  this->setBoxWidgetSize(bb_r);
}

bool InteractiveCropper::getUseCropping()
{
  vtkVolumeMapperPtr mapper = this->getMapper();
  if (!mapper)
    return false;
  //std::cout << "mapper->GetCropping() " << mapper->GetCropping() << std::endl;
  return mapper->GetCropping();
}

bool InteractiveCropper::getShowBoxWidget() const
{
  return mBoxWidget->GetEnabled();
}

/** Set the box widget bounding box to the input box (given in ref space)
 */
void InteractiveCropper::setBoxWidgetSize(const ssc::DoubleBoundingBox3D& bb_r)
{
 // std::cout << "InteractiveCropper::setBoxWidgetSize(" << bb_r << ")" << std::endl;
  double bb_hard[6] = { -0.5,0.5,  -0.5,0.5,  -0.5,0.5 };
  ssc::DoubleBoundingBox3D bb_unit(bb_hard);
  ssc::Transform3D M = ssc::createTransformNormalize(bb_unit, bb_r);
  //std::cout << "BB_image_d " << image->boundingBox() << std::endl;
  //std::cout << "BB_image_r " << bb << std::endl;

  vtkTransformPtr transform = vtkTransformPtr::New();
  transform->SetMatrix(M.matrix());
  mBoxWidget->SetTransform(transform);
}

/** return the bow widget current size in ref space
 */
ssc::DoubleBoundingBox3D InteractiveCropper::getBoxWidgetSize()
{
  double bb_hard[6] = { -0.5,0.5,  -0.5,0.5,  -0.5,0.5 };
  ssc::DoubleBoundingBox3D bb_unit(bb_hard);

  vtkTransformPtr transform = vtkTransformPtr::New();
  mBoxWidget->GetTransform(transform);
  ssc::Transform3D M(transform->GetMatrix());

  ssc::DoubleBoundingBox3D bb_new_r = ssc::transform(M, bb_unit);
  return bb_new_r;
}

void InteractiveCropper::setCroppingRegion(ssc::DoubleBoundingBox3D bb_r)
{
  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
  if (!image)
    return;

  image->setCroppingBox(bb_r);

  ssc::DoubleBoundingBox3D bb_d = ssc::transform(image->get_rMd().inv(), bb_r);
  //std::cout << "bb_image_d: " << image->boundingBox() << std::endl;
  //std::cout << "BB_new_d  : " << bb_new << std::endl;
  ssc::VolumetricRepPtr volRep = repManager()->getVolumetricRep(image);

  vtkVolumeMapperPtr mapper = this->getMapper();
  if (!mapper)
    return;
//  mapper->CroppingOn();
//    mapper->SetCroppingRegionFlagsToCross();
  //std::cout << "InteractiveCropper::setCroppingRegion()" << std::endl;
  mapper->SetCroppingRegionPlanes(bb_d.begin());
  mapper->Update();

  emit changed();
}

void InteractiveCropper::boxWasShown(bool val)
{
  emit changed();
}

/** return the largest useful bounding box for the current selection
 *
 */
ssc::DoubleBoundingBox3D InteractiveCropper::getMaxBoundingBox()
{
  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
  if (!image)
    return ssc::DoubleBoundingBox3D();
  ssc::DoubleBoundingBox3D bb_r = transform(image->get_rMd(), image->boundingBox());
  return bb_r;
}


} // namespace cx
