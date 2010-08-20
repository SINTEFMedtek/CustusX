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

SlicePlaneClipper::~SlicePlaneClipper()
{
  this->clearVolumes();
}

void SlicePlaneClipper::setSliceProxy(ssc::SliceProxyPtr slicer)
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
  mVolumes.insert(volume);
  this->changedSlot();
}

void SlicePlaneClipper::removeVolume(ssc::VolumetricRepPtr volume)
{
  volume->getVtkVolume()->GetMapper()->RemoveAllClippingPlanes();
  mVolumes.erase(volume);
  this->changedSlot();
}

SlicePlaneClipper::VolumesType SlicePlaneClipper::getVolumes()
{
  return mVolumes;
}

void SlicePlaneClipper::changedSlot()
{
  ssc::Transform3D rMs = mSlicer->get_sMr().inv();
  ssc::Vector3D n = rMs.vector(ssc::Vector3D(0,0,-1));
  ssc::Vector3D p = rMs.coord(ssc::Vector3D(0,0,0));
  vtkPlanePtr plane = vtkPlanePtr::New();
  plane->SetNormal(n.begin());
  plane->SetOrigin(p.begin());

  for (VolumesType::iterator iter=mVolumes.begin(); iter!=mVolumes.end(); ++iter)
  {
//    std::cout << "coronal: <" << n << "> <" << p << ">" << std::endl;
//    std::cout << "BB: " << iter->second->getImage()->boundingBox() << std::endl;
//
    (*iter)->getVtkVolume()->GetMapper()->RemoveAllClippingPlanes();
    (*iter)->getVtkVolume()->GetMapper()->AddClippingPlane(plane);
  }
}


} // namespace ssc
