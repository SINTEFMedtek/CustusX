/*
 * cxInteractiveClipper.cpp
 *
 *  Created on: Aug 24, 2010
 *      Author: christiana
 */
#include "cxInteractiveClipper.h"

//#include <QGridLayout>
//#include <QWidget>
//#include <QTimer>
//#include <QSettings>
//#include <QTime>
//#include <QAction>
//#include <vtkRenderWindow.h>
//#include <vtkImageData.h>
#include "sscVolumetricRep.h"
#include "sscMessageManager.h"
#include "cxRepManager.h"
#include "cxDataManager.h"

#include "sscSlicePlaneClipper.h"
#include "sscSlicePlanes3DRep.h"
#include "sscSliceProxy.h"


namespace cx
{

InteractiveClipper::InteractiveClipper(ssc::SlicePlanesProxyPtr slicePlanesProxy) :
    mSlicePlanesProxy(slicePlanesProxy),
    mUseClipper(false)
{
  mSlicePlaneClipper = ssc::SlicePlaneClipper::New();
//  connect(ssc::dataManager(), SIGNAL(activeImageChanged(std::string)),      this, SLOT(changedSlot()));
  connect(this              , SIGNAL(changed()),                       this, SLOT(changedSlot()));
  connect(ssc::dataManager(), SIGNAL(activeImageChanged(std::string)), this, SIGNAL(changed()));
  //connect(viewManager(), SIGNAL(activeLayoutChanged()), this, SIGNAL(changed()));

  //  ssc::SliceProxyPtr slicer = mSlicePlanes3DRep->getProxy()->getData()[ssc::ptCORONAL].mSliceProxy;
  //  mSlicePlaneClipper->setSliceProxy(slicer);
  //  if (action->isChecked())
  //    mSlicePlaneClipper->addVolume(mVolumetricReps[uid]);
  //  else
  //    mSlicePlaneClipper->removeVolume(mVolumetricReps[uid]);
  this->changedSlot();
}

void InteractiveClipper::setSlicePlane(ssc::PLANE_TYPE plane)
{
  if (mSlicePlaneClipper->getSlicer() && mSlicePlaneClipper->getSlicer()->getComputer().getPlaneType()==plane)
    return;

  if (mSlicePlanesProxy->getData().count(plane))
  {
    std::cout << "InteractiveClipper::setSlicePlane(" << plane << ")" << std::endl;
    mSlicePlaneClipper->setSlicer(mSlicePlanesProxy->getData()[plane].mSliceProxy);
    emit changed();
  }
}

void InteractiveClipper::saveClipPlaneToVolume()
{
  // not implemented
}
void InteractiveClipper::clearClipPlanesInVolume()
{
  // not implemented
}

ssc::PLANE_TYPE InteractiveClipper::getSlicePlane()
{
  if (!mSlicePlaneClipper->getSlicer())
    return ssc::ptCOUNT;
  return mSlicePlaneClipper->getSlicer()->getComputer().getPlaneType();
}

bool InteractiveClipper::getUseClipper() const
{
  return mUseClipper;
}
bool InteractiveClipper::getInvertPlane() const
{
  return mSlicePlaneClipper->getInvertPlane();
}
void InteractiveClipper::useClipper(bool on)
{
  mUseClipper = on;
  emit changed();
}
void InteractiveClipper::invertPlane(bool on)
{
  mSlicePlaneClipper->setInvertPlane(on);
  emit changed();
}

ssc::PLANE_TYPE InteractiveClipper::getPlaneType()
{
  if (!mSlicePlaneClipper->getSlicer())
    return ssc::ptCOUNT;
  return mSlicePlaneClipper->getSlicer()->getComputer().getPlaneType();
}

void InteractiveClipper::changedSlot()
{
//  std::cout << "InteractiveClipper::changedSlot()" << std::endl;

  if (mUseClipper)
  {
    ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
//    std::cout << "image: " << image.get() << std::endl;
    mSlicePlaneClipper->clearVolumes();
    mSlicePlaneClipper->addVolume(repManager()->getVolumetricRep(image));

    std::vector<ssc::PLANE_TYPE> planes = this->getAvailableSlicePlanes();
    if (!std::count(planes.begin(), planes.end(), this->getPlaneType()))
    //if (this->getPlaneType()==ssc::ptCOUNT)
    {
      std::vector<ssc::PLANE_TYPE> planes = this->getAvailableSlicePlanes();
      if (planes.empty())
        return;
      ssc::PLANE_TYPE plane = planes.front();
      std::cout << "init slicer(" << plane << ")" << std::endl;
      mSlicePlaneClipper->setSlicer(mSlicePlanesProxy->getData()[plane].mSliceProxy);
    }
  }
  else
  {
    mSlicePlaneClipper->clearVolumes();
  }
}

//void InteractiveClipper::activeLayoutChangedSlot()
//{
//
//}
//void InteractiveClipper::activeImageChangedSlot()
//{
//
//}

std::vector<ssc::PLANE_TYPE> InteractiveClipper::getAvailableSlicePlanes() const
{
//  std::cout << "InteractiveClipper::getAvailableSlicePlanes() " << mSlicePlanesProxy->getData().size() << std::endl;

  std::vector<ssc::PLANE_TYPE> retval;
  ssc::SlicePlanesProxy::DataMap data = mSlicePlanesProxy->getData();
  for (ssc::SlicePlanesProxy::DataMap::iterator iter=data.begin(); iter!=data.end(); ++iter)
  {
    retval.push_back(iter->first);
  }
  return retval;
}


} // namespace cx
