/*
 * sscSlicePlaneClipper.h
 *
 *  Created on: Aug 20, 2010
 *      Author: christiana
 */
#ifndef SSCSLICEPLANECLIPPER_H_
#define SSCSLICEPLANECLIPPER_H_

#include <set>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <vtkSmartPointer.h>
#include <QObject>
#include "sscForwardDeclarations.h"
#include "sscVector3D.h"

typedef vtkSmartPointer<class vtkPlane> vtkPlanePtr;
typedef vtkSmartPointer<class vtkVolumeMapper> vtkVolumeMapperPtr;
typedef vtkSmartPointer<class vtkVolume> vtkVolumePtr;


namespace ssc
{

typedef boost::shared_ptr<class SlicePlaneClipper> SlicePlaneClipperPtr;

/**SlicePlaneClipper uses a SlicePlanesProxy to clip several volumes.
 */
class SlicePlaneClipper : public QObject
{
  Q_OBJECT
public:
  typedef std::set<ssc::VolumetricRepPtr> VolumesType;

  static SlicePlaneClipperPtr New();
  ~SlicePlaneClipper();
  void setSlicer(ssc::SliceProxyPtr slicer);
  void clearVolumes();
  void addVolume(ssc::VolumetricRepPtr volume);
  void removeVolume(ssc::VolumetricRepPtr volume);
  VolumesType getVolumes();
  ssc::SliceProxyPtr getSlicer();
  void setInvertPlane(bool on);
  bool getInvertPlane() const;
  vtkPlanePtr getClipPlaneCopy();

  void saveClipPlaneToVolume(); ///< save the current clip to image
  void clearClipPlanesInVolume(); ///< clear all saved clips in the image.

private slots:
  void changedSlot();
private:
  void addClipPlane(ssc::VolumetricRepPtr volume, vtkPlanePtr clipPlane);
  SlicePlaneClipper();
  void updateClipPlane();
  ssc::Vector3D getUnitNormal() const;
  ssc::SliceProxyPtr mSlicer;
  VolumesType mVolumes;
  bool mInvertPlane;

  vtkPlanePtr mClipPlane;
//  std::vector<vtkPlanePtr> mFixedPlanes;
};

/**Helper class that uses the stored clip planes in a Image to clip it in a mapper.
 *
 * Used internally by VolumetricRep - No need to use this elsewhere!
 */
class ImageMapperMonitor : public QObject
{
  Q_OBJECT
public:
  ImageMapperMonitor(vtkVolumePtr volume, ImagePtr image);
  ~ImageMapperMonitor();

private slots:
  void clipPlanesChangedSlot();
  void cropBoxChangedSlot();
private:
  //ssc::VolumetricRepPtr mVolume;
  vtkVolumePtr mVolume;
  ImagePtr mImage;
  std::vector<vtkPlanePtr> mPlanes;
  vtkVolumeMapperPtr getMapper();

  void clearClipPlanes();
  void fillClipPlanes();
};

} // namespace ssc

#endif /* SSCSLICEPLANECLIPPER_H_ */
