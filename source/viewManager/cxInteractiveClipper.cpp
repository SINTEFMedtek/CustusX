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
#include "sscSliceComputer.h"

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
  connect(this              , SIGNAL(changed()),                       this, SLOT(changedSlot()));
  connect(ssc::dataManager(), SIGNAL(activeImageChanged(std::string)), this, SIGNAL(changed()));

  this->changedSlot();
}

void InteractiveClipper::setSlicePlane(ssc::PLANE_TYPE plane)
{
  if (mSlicePlaneClipper->getSlicer() && mSlicePlaneClipper->getSlicer()->getComputer().getPlaneType()==plane)
    return;

  if (mSlicePlanesProxy->getData().count(plane))
  {
//    std::cout << "InteractiveClipper::setSlicePlane(" << plane << ")" << std::endl;
    mSlicePlaneClipper->setSlicer(mSlicePlanesProxy->getData()[plane].mSliceProxy);
    emit changed();
  }
}

void InteractiveClipper::saveClipPlaneToVolume()
{
  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
  if (!image)
    return;

  image->addClipPlane(mSlicePlaneClipper->getClipPlaneCopy());
}
void InteractiveClipper::clearClipPlanesInVolume()
{
  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
  if (!image)
    return;
  image->clearClipPlanes();
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
    mSlicePlaneClipper->clearVolumes();
    mSlicePlaneClipper->addVolume(repManager()->getVolumetricRep(image));

    ssc::PLANE_TYPE currentPlane = this->getPlaneType();

    std::vector<ssc::PLANE_TYPE> planes = this->getAvailableSlicePlanes();

    if (!std::count(planes.begin(), planes.end(), currentPlane))    //if (this->getPlaneType()==ssc::ptCOUNT)
    {
      if (planes.empty()) // no slices: remove clipping
      {
    	  currentPlane = ssc::ptCOUNT;
      }
      currentPlane = planes.front();
//      std::cout << "init slicer(" << currentPlane << ")" << std::endl;
    }

    // reset plane anyway. It might be the same planeType but a different sliceProxy.
    mSlicePlaneClipper->setSlicer(mSlicePlanesProxy->getData()[currentPlane].mSliceProxy);
  }
  else
  {
    mSlicePlaneClipper->clearVolumes();
  }
}

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
