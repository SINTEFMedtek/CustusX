/*
 * sscSlicePlaneClipper.cpp
 *
 *  Created on: Aug 20, 2010
 *      Author: christiana
 */

#include "sscSlicePlaneClipper.h"

#include <vtkPlane.h>
#include <vtkVolume.h>
#include <vtkAbstractVolumeMapper.h>

#include "sscSliceProxy.h"
#include "sscVolumetricRep.h"

typedef vtkSmartPointer<class vtkPlane> vtkPlanePtr;


namespace ssc
{

SlicePlaneClipperPtr SlicePlaneClipper::New()
{
  return SlicePlaneClipperPtr(new SlicePlaneClipper());
}

SlicePlaneClipper::SlicePlaneClipper() :
  mInvertPlane(false)
{
}

SlicePlaneClipper::~SlicePlaneClipper()
{
  this->clearVolumes();
}

void SlicePlaneClipper::setSlicer(ssc::SliceProxyPtr slicer)
{
  if (mSlicer==slicer)
    return;
  if (mSlicer)
  {
    disconnect(mSlicer.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(changedSlot()));
  }
  mSlicer = slicer;
  connect(mSlicer.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(changedSlot()));
  this->changedSlot();
}

ssc::SliceProxyPtr SlicePlaneClipper::getSlicer()
{
  return mSlicer;
}

void SlicePlaneClipper::clearVolumes()
{
  for (VolumesType::iterator iter=mVolumes.begin(); iter!=mVolumes.end(); ++iter)
  {
    (*iter)->getVtkVolume()->GetMapper()->RemoveAllClippingPlanes();
  }
  mVolumes.clear();
}

void SlicePlaneClipper::addVolume(ssc::VolumetricRepPtr volume)
{
  if (!volume)
    return;
  mVolumes.insert(volume);
  this->changedSlot();
}

void SlicePlaneClipper::removeVolume(ssc::VolumetricRepPtr volume)
{
  if (!volume)
    return;
  volume->getVtkVolume()->GetMapper()->RemoveAllClippingPlanes();
  mVolumes.erase(volume);
  this->changedSlot();
}

SlicePlaneClipper::VolumesType SlicePlaneClipper::getVolumes()
{
  return mVolumes;
}

void SlicePlaneClipper::setInvertPlane(bool on)
{
  mInvertPlane = on;
  changedSlot();
}

bool SlicePlaneClipper::getInvertPlane() const
{
  return mInvertPlane;
}

/** return an untransformed plane normal to use during clipping.
 *  The direction is dependent in invertedPlane()
 */
ssc::Vector3D SlicePlaneClipper::getUnitNormal() const
{
  if (mInvertPlane)
    return ssc::Vector3D(0,0,1);
  else
    return ssc::Vector3D(0,0,-1);
}

void SlicePlaneClipper::changedSlot()
{
  if (!mSlicer)
    return;
  ssc::Transform3D rMs = mSlicer->get_sMr().inv();

  ssc::Vector3D n = rMs.vector(this->getUnitNormal());
  ssc::Vector3D p = rMs.coord(ssc::Vector3D(0,0,0));
  vtkPlanePtr plane = vtkPlanePtr::New();
  plane->SetNormal(n.begin());
  plane->SetOrigin(p.begin());

  for (VolumesType::iterator iter=mVolumes.begin(); iter!=mVolumes.end(); ++iter)
  {
    (*iter)->getVtkVolume()->GetMapper()->RemoveAllClippingPlanes();
    (*iter)->getVtkVolume()->GetMapper()->AddClippingPlane(plane);
  }
}


} // namespace ssc
