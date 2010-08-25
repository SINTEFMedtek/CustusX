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
  CropBoxCallback(QString msg) : Msg(msg) {}
  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    vtkBoxWidget* box = reinterpret_cast<vtkBoxWidget*>(caller);
    std::cout << Msg << " box: " << box << std::endl;
    if (!box)
      return;

    ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
    if (!image)
      return;
    double bb_hard[6] = { -0.5,0.5,  -0.5,0.5,  -0.5,0.5 };
    ssc::DoubleBoundingBox3D bb_unit(bb_hard);
    ssc::DoubleBoundingBox3D bb_image = transform(image->get_rMd(), image->boundingBox());
    std::cout << "bb_image_r: " << bb_image << std::endl;

    vtkTransformPtr transform = vtkTransformPtr::New();
    box->GetTransform(transform);
    ssc::Transform3D M(transform->GetMatrix());
    //std::cout << "M: " << M << std::endl;
    ssc::DoubleBoundingBox3D bb_new = ssc::transform(M, bb_unit);
    bb_new = ssc::transform(image->get_rMd().inv(), bb_new);
    std::cout << "bb_image_d: " << image->boundingBox() << std::endl;
    std::cout << "BB_new_d  : " << bb_new << std::endl;
    ssc::VolumetricRepPtr volRep = repManager()->getVolumetricRep(image);

    vtkVolumeMapper* mapper = dynamic_cast<vtkVolumeMapper*>(volRep->getVtkVolume()->GetMapper());
    if (!mapper)
      return;
    mapper->CroppingOn();
//    mapper->SetCroppingRegionFlagsToCross();
    std::cout << "cropping" << std::endl;
    mapper->SetCroppingRegionPlanes(bb_new.begin());
    mapper->Update();
  }

  QString Msg;
};


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


InteractiveCropper::InteractiveCropper()
{
  connect(ssc::dataManager(), SIGNAL(activeImageChanged(std::string)), this, SLOT(imageChangedSlot()));

  mBoxWidget = vtkBoxWidgetPtr::New();
  mBoxWidget->RotationEnabledOff();

  double bb_hard[6] = { -1,1,  -1,1,  -1,1 };
  mBoxWidget->PlaceWidget(bb_hard);

  CropBoxCallback* mCropBoxCallback = new CropBoxCallback("inter");
  mBoxWidget->AddObserver(vtkCommand::InteractionEvent, mCropBoxCallback);
 // mBoxWidget->AddObserver(vtkCommand::StartInteractionEvent, new CropBoxCallback("start"));
 // mBoxWidget->AddObserver(vtkCommand::EnableEvent, new CropBoxCallback("enable"));
  mBoxWidget->RemoveObserver(mCropBoxCallback);
}

void InteractiveCropper::setView(ssc::View* view)
{
  mBoxWidget->SetInteractor(view->getRenderWindow()->GetInteractor());
}

void InteractiveCropper::showBoxWidget(bool on)
{

}

ssc::DoubleBoundingBox3D InteractiveCropper::getBoundingBox() const
{
  return ssc::DoubleBoundingBox3D();
}

void InteractiveCropper::setBoundingBox(const ssc::DoubleBoundingBox3D& bb_r)
{

}
void InteractiveCropper::useCropping(bool on)
{

}
void InteractiveCropper::resetBoundingBox()
{

}

void InteractiveCropper::imageChangedSlot()
{
  startBoxInteraction();
}

void InteractiveCropper::startBoxInteraction()
{
  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
  if (!image)
    return;
  double bb_hard[6] = { -0.5,0.5,  -0.5,0.5,  -0.5,0.5 };
  ssc::DoubleBoundingBox3D bb_unit(bb_hard);
  ssc::DoubleBoundingBox3D bb = transform(image->get_rMd(), image->boundingBox());
  ssc::Transform3D M = ssc::createTransformNormalize(bb_unit, bb);
  std::cout << "BB_image_d " << image->boundingBox() << std::endl;
  std::cout << "BB_image_r " << bb << std::endl;

  vtkTransformPtr transform = vtkTransformPtr::New();
  transform->SetMatrix(M.matrix());
  mBoxWidget->SetTransform(transform);
}

bool InteractiveCropper::getUseCropping() const
{
  return true;
}
bool InteractiveCropper::getShowBoxWidget() const
{
  return true;
}


} // namespace cx
