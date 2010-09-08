/*
 * cxInteractiveCropper.cpp
 *
 *  Created on: Aug 24, 2010
 *      Author: christiana
 */
#include "cxInteractiveCropper.h"

#include "sscView.h"

#include <vector>
#include <vtkTransform.h>
#include <vtkAbstractVolumeMapper.h>
#include <vtkVolumeMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include "sscTypeConversions.h"
#include "cxRepManager.h"
#include "sscDataManager.h"
#include <vtkBoxWidget2.h>
#include <vtkBoxWidget.h>
#include "sscBoundingBox3D.h"

namespace cx
{

class CropBoxCallback : public vtkCommand
{
public:
  CropBoxCallback(InteractiveCropper* cropper) : mCropper(cropper) {}
  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
 //   std::cout << "executing" << std::endl;
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

void InteractiveCropper::showBoxWidget(bool on)
{
  if (!mImage)
    return;
  if (this->getShowBoxWidget()==on)
    return;

  if (!mImage->getCropping())
  {
    mBoxWidget->SetEnabled(false);
    emit changed();
    return;
  }

  mBoxWidget->SetEnabled(on);

  //std::cout << "Enabled  " << mBoxWidget->GetEnabled() << std::endl;
  emit changed();
}

/** get current cropping box in ref coords
 */
ssc::DoubleBoundingBox3D InteractiveCropper::getBoundingBox()
{
  if (!mImage)
    return ssc::DoubleBoundingBox3D();
  return transform(mImage->get_rMd(), mImage->getCroppingBox());
}

void InteractiveCropper::setBoundingBox(const ssc::DoubleBoundingBox3D& bb_r)
{
 // std::cout << "setBB" << std::endl;
  this->setCroppingRegion(bb_r);
  this->setBoxWidgetSize(bb_r);
//  emit changed();
}

vtkVolumeMapperPtr InteractiveCropper::getMapper()
{
  if (!mImage)
    return vtkVolumeMapperPtr();
  ssc::VolumetricRepPtr volRep = repManager()->getVolumetricRep(mImage);

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

  if (!mImage)
    return;

  mImage->setCropping(on);
}

void InteractiveCropper::imageCropChangedSlot()
{
  if (!mImage)
    return;

  ssc::DoubleBoundingBox3D bb_r = this->getBoundingBox();

  //std::cout << "InteractiveCropper::imageCropChangedSlot" << std::endl;
  this->setBoxWidgetSize(bb_r);
  this->updateBoxWidgetInteractor();

  if (!mImage->getCropping())
    this->showBoxWidget(false);

  emit changed();
}

void InteractiveCropper::resetBoundingBox()
{
  emit changed();
}

void InteractiveCropper::imageChangedSlot()
{
  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();

  if (mImage)
  {
    disconnect(mImage.get(), SIGNAL(cropBoxChanged()), this, SLOT(imageCropChangedSlot()));
  }

  mImage = image;

  if (mImage)
  {
    connect(mImage.get(), SIGNAL(cropBoxChanged()), this, SLOT(imageCropChangedSlot()));
  }

  this->imageCropChangedSlot();
  emit changed();
}

bool InteractiveCropper::getUseCropping()
{
  if (!mImage)
    return false;
  return mImage->getCropping();
}

bool InteractiveCropper::getShowBoxWidget() const
{
  return mBoxWidget->GetEnabled();
}

/** Set the box widget bounding box to the input box (given in ref space)
 */
void InteractiveCropper::setBoxWidgetSize(const ssc::DoubleBoundingBox3D& bb_r)
{
  double bb_hard[6] = { -0.5,0.5,  -0.5,0.5,  -0.5,0.5 };
  ssc::DoubleBoundingBox3D bb_unit(bb_hard);
  ssc::Transform3D M = ssc::createTransformNormalize(bb_unit, bb_r);

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
  if (!mImage)
    return;
  mImage->setCroppingBox(transform(mImage->get_rMd().inv(), bb_r));
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
  if (!mImage)
    return ssc::DoubleBoundingBox3D();
  ssc::DoubleBoundingBox3D bb_r = transform(mImage->get_rMd(), mImage->boundingBox());
  return bb_r;
}


} // namespace cx
