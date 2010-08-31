/*
 * sscSlicePlaneClipper.cpp
 *
 *  Created on: Aug 20, 2010
 *      Author: christiana
 */

#include "sscSlicePlaneClipper.h"

#include <vector>
#include <vtkPlane.h>
#include <vtkVolume.h>
#include <vtkAbstractVolumeMapper.h>
#include <vtkPlaneCollection.h>
#include <vtkVolumeMapper.h>

#include "sscSliceProxy.h"
#include "sscVolumetricRep.h"
#include "sscImage.h"
#include "sscDataManager.h"

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
  if (mSlicer)
  {
    connect(mSlicer.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(changedSlot()));
  }

  this->updateClipPlane();
  for (VolumesType::iterator iter=mVolumes.begin(); iter!=mVolumes.end(); ++iter)
  {
//    // debug:
//    vtkAbstractVolumeMapper* mapper = (*iter)->getVtkVolume()->GetMapper();
//    std::cout << "mapper: count: " << mapper->GetReferenceCount() << std::endl;
//    std::cout << "mClipPlane: " << mClipPlane.GetPointer() << std::endl;
//    std::cout << "GetClippingPlanes ptr: " << (*iter)->getVtkVolume()->GetMapper()->GetClippingPlanes() << std::endl;
//    mapper->Print(std::cout);
//    std::cout << "GetClippingPlanes: " << (*iter)->getVtkVolume()->GetMapper()->GetClippingPlanes()->GetNumberOfItems() << "-" << (*iter)->getVtkVolume()->GetMapper()->GetClippingPlanes()->GetReferenceCount() << std::endl;

    this->addClipPlane(*iter, mClipPlane);
  }
  this->changedSlot();
}

void SlicePlaneClipper::addClipPlane(ssc::VolumetricRepPtr volume, vtkPlanePtr clipPlane)
{
  if (!clipPlane)
    return;
  vtkAbstractVolumeMapper* mapper = volume->getVtkVolume()->GetMapper();
  if (mapper->GetClippingPlanes() && mapper->GetClippingPlanes()->IsItemPresent(clipPlane))
    return;
  mapper->AddClippingPlane(clipPlane);
}

ssc::SliceProxyPtr SlicePlaneClipper::getSlicer()
{
  return mSlicer;
}

void SlicePlaneClipper::clearVolumes()
{
//  std::cout << "SlicePlaneClipper::clearVolumes()" << std::endl;

  for (VolumesType::iterator iter=mVolumes.begin(); iter!=mVolumes.end(); ++iter)
  {
    (*iter)->getVtkVolume()->GetMapper()->RemoveClippingPlane(mClipPlane);
  }
  mVolumes.clear();
}

void SlicePlaneClipper::addVolume(ssc::VolumetricRepPtr volume)
{
  if (!volume)
    return;
  mVolumes.insert(volume);

//  if (mClipPlane)
//  {
    this->addClipPlane(volume, mClipPlane);
//
//    if (!volume->getVtkVolume()->GetMapper()->GetClippingPlanes()->IsItemPresent(mClipPlane))
//      volume->getVtkVolume()->GetMapper()->AddClippingPlane(mClipPlane);
 // }

  this->changedSlot();
}

void SlicePlaneClipper::removeVolume(ssc::VolumetricRepPtr volume)
{
  if (!volume)
    return;
  volume->getVtkVolume()->GetMapper()->RemoveClippingPlane(mClipPlane);
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

/** return a vtkPlane representing the current clip plane.
 */
vtkPlanePtr SlicePlaneClipper::getClipPlaneCopy()
{
  vtkPlanePtr retval = vtkPlanePtr::New();
  retval->SetNormal(mClipPlane->GetNormal());
  retval->SetOrigin(mClipPlane->GetOrigin());
  return retval;
}

void SlicePlaneClipper::updateClipPlane()
{
  if (!mSlicer)
    return;
  if (!mClipPlane)
    mClipPlane = vtkPlanePtr::New();

  ssc::Transform3D rMs = mSlicer->get_sMr().inv();

  ssc::Vector3D n = rMs.vector(this->getUnitNormal());
  ssc::Vector3D p = rMs.coord(ssc::Vector3D(0,0,0));
  mClipPlane->SetNormal(n.begin());
  mClipPlane->SetOrigin(p.begin());
}

void SlicePlaneClipper::changedSlot()
{
  if (!mSlicer)
    return;

  this->updateClipPlane();
  //std::cout << "SlicePlaneClipper::changedSlot()" << std::endl;
}



///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------



ImageMapperMonitor::ImageMapperMonitor(vtkVolumePtr volume, ImagePtr image) : mVolume(volume), mImage(image)
{
  if (!mImage)
    return;

  //std::cout << "ImageMapperMonitor::ImageMapperMonitor()" << std::endl;
  connect(mImage.get(), SIGNAL(clipPlanesChanged()), this, SLOT(clipPlanesChangedSlot()));
  connect(mImage.get(), SIGNAL(cropBoxChanged()), this, SLOT(cropBoxChangedSlot()));
  this->fillClipPlanes();
  this->cropBoxChangedSlot();
}

ImageMapperMonitor::~ImageMapperMonitor()
{
  this->clearClipPlanes();
}

void ImageMapperMonitor::clipPlanesChangedSlot()
{
  //std::cout << "ImageMapperMonitor::clipPlanesChangedSlot()" << std::endl;
  this->clearClipPlanes();
  this->fillClipPlanes();
}

void ImageMapperMonitor::clearClipPlanes()
{
  if (!mImage)
    return;

  for (unsigned i=0; i<mPlanes.size(); ++i)
  {
    mVolume->GetMapper()->RemoveClippingPlane(mPlanes[i]);
  }
  mPlanes.clear();
}

void ImageMapperMonitor::fillClipPlanes()
{
  if (!mImage)
    return;

  mPlanes = mImage->getClipPlanes();
  for (unsigned i=0; i<mPlanes.size(); ++i)
  {
    mVolume->GetMapper()->AddClippingPlane(mPlanes[i]);
  }
}

vtkVolumeMapperPtr ImageMapperMonitor::getMapper()
{
  vtkVolumeMapperPtr mapper = dynamic_cast<vtkVolumeMapper*>(mVolume->GetMapper());
  //mapper->Register();
  return mapper;
//  if (!mapper)
//    return;
}

void ImageMapperMonitor::cropBoxChangedSlot()
{
  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
  if (!image)
    return;

  vtkVolumeMapperPtr mapper = this->getMapper();
  if (!mapper)
    return;
  mapper->SetCropping(image->getCropping());

  ssc::DoubleBoundingBox3D bb_d = image->getCroppingBox();

  //ssc::DoubleBoundingBox3D bb_d = ssc::transform(image->get_rMd().inv(), bb_r);
  mapper->SetCroppingRegionPlanes(bb_d.begin());
  mapper->Update();

//  emit changed();
}



} // namespace ssc
