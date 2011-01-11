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
#include "sscImage.h"
#include "sscTransform3D.h"
#include "sscVolumetricRep.h"

namespace cx
{

class CropBoxCallback : public vtkCommand
{
public:
  CropBoxCallback() {}
  static CropBoxCallback* New() {return new CropBoxCallback;}
  void SetCropper(InteractiveCropper* cropper) {mCropper = cropper;}
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
  CropBoxEnableCallback() {}
  static CropBoxEnableCallback* New() {return new CropBoxEnableCallback;}
  void SetCropper(bool val, InteractiveCropper* cropper) {mValue = val; mCropper = cropper;}
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
  connect(ssc::dataManager(), SIGNAL(activeImageChanged(QString)), this, SLOT(imageChangedSlot()));
  connect(ssc::dataManager(), SIGNAL(activeImageChanged(QString)), this, SIGNAL(changed()));
}

void InteractiveCropper::initialize()
{
  if (mBoxWidget) // already initialized
    return;

  mBoxWidget = vtkBoxWidgetPtr::New();
  mBoxWidget->RotationEnabledOff();

  double bb_hard[6] = { -1,1,  -1,1,  -1,1 };
  mBoxWidget->PlaceWidget(bb_hard);

  mCropBoxCallback = CropBoxCallbackPtr::New();
  mCropBoxCallback->SetCropper(this);
  mCropBoxEnableCallback = CropBoxEnableCallbackPtr::New();
  mCropBoxEnableCallback->SetCropper(true, this);
  mCropBoxDisableCallback = CropBoxEnableCallbackPtr::New();
  mCropBoxDisableCallback->SetCropper(false, this);

  mBoxWidget->SetInteractor(mView->getRenderWindow()->GetInteractor());

  mBoxWidget->SetEnabled(false);
}

void InteractiveCropper::setView(ssc::View* view)
{
  mView = view;
  this->updateBoxWidgetInteractor();
}

void InteractiveCropper::updateBoxWidgetInteractor()
{
  if (!mView)
    return;

  this->initialize();

  if (this->getUseCropping())
  {
    mBoxWidget->SetInteractor(mView->getRenderWindow()->GetInteractor());
    mBoxWidget->AddObserver(vtkCommand::InteractionEvent, mCropBoxCallback);
    mBoxWidget->AddObserver(vtkCommand::EnableEvent, mCropBoxEnableCallback);
    mBoxWidget->AddObserver(vtkCommand::DisableEvent, mCropBoxDisableCallback);
  }
  else
  {
    mBoxWidget->RemoveObserver(vtkCommand::InteractionEvent);
    mBoxWidget->RemoveObserver(vtkCommand::EnableEvent);
    mBoxWidget->RemoveObserver(vtkCommand::DisableEvent);
  }
}

void InteractiveCropper::showBoxWidget(bool on)
{
  if (!mImage)
    return;
  if (!mBoxWidget)
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
  emit changed();
}

/** get current cropping box in ref coords
 */
ssc::DoubleBoundingBox3D InteractiveCropper::getBoundingBox()
{
  if (!mImage || !mBoxWidget)
    return ssc::DoubleBoundingBox3D();
  return mImage->getCroppingBox();
}

void InteractiveCropper::setBoundingBox(const ssc::DoubleBoundingBox3D& bb_d)
{
  this->setCroppingRegion(bb_d);
  this->setBoxWidgetSize(bb_d);
}

vtkVolumeMapperPtr InteractiveCropper::getMapper()
{
  if (!mImage)
    return vtkVolumeMapperPtr();
  ssc::VolumetricRepPtr volRep = repManager()->getVolumetricRep(mImage);

  vtkVolumeMapperPtr mapper = dynamic_cast<vtkVolumeMapper*>(volRep->getVtkVolume()->GetMapper());
  return mapper;
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

  ssc::DoubleBoundingBox3D bb_d = this->getBoundingBox();
  this->setBoxWidgetSize(bb_d);
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
  if (!mBoxWidget)
    return false;
  return mBoxWidget->GetEnabled();
}

/** Set the box widget bounding box to the input box (given in data space)
 */
void InteractiveCropper::setBoxWidgetSize(const ssc::DoubleBoundingBox3D& bb_d)
{
  if (!mImage || !mBoxWidget)
    return;

  double bb_hard[6] = { -0.5,0.5,  -0.5,0.5,  -0.5,0.5 };
  ssc::DoubleBoundingBox3D bb_unit(bb_hard);
  ssc::Transform3D M = ssc::createTransformNormalize(bb_unit, bb_d);
  ssc::Transform3D rMd = mImage->get_rMd();
  M = rMd * M;

  vtkTransformPtr transform = vtkTransformPtr::New();
  transform->SetMatrix(M.matrix());
  mBoxWidget->SetTransform(transform);
}

/** return the bow widget current size in data space
 */
ssc::DoubleBoundingBox3D InteractiveCropper::getBoxWidgetSize()
{
  if (!mImage || !mBoxWidget)
  {
    return ssc::DoubleBoundingBox3D();
  }

  double bb_hard[6] = { -0.5,0.5,  -0.5,0.5,  -0.5,0.5 };
  ssc::DoubleBoundingBox3D bb_unit(bb_hard);

  vtkTransformPtr transform = vtkTransformPtr::New();
  mBoxWidget->GetTransform(transform);
  ssc::Transform3D M(transform->GetMatrix());

  ssc::Transform3D rMd = mImage->get_rMd();
  M = rMd.inv() * M;

  ssc::DoubleBoundingBox3D bb_new_r = ssc::transform(M, bb_unit);

  return bb_new_r;
}

void InteractiveCropper::setCroppingRegion(ssc::DoubleBoundingBox3D bb_d)
{
  if (!mImage)
    return;
  mImage->setCroppingBox(bb_d);
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
  return mImage->boundingBox();
}


} // namespace cx
