#ifndef SSCSLICEPLANECLIPPER_H_
#define SSCSLICEPLANECLIPPER_H_

#include <set>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <vtkSmartPointer.h>
#include <QObject>
#include "sscForwardDeclarations.h"
#include "sscVector3D.h"
#include "vtkForwardDeclarations.h"

namespace ssc
{
typedef boost::shared_ptr<class SlicePlaneClipper> SlicePlaneClipperPtr;

/**
 * \class SlicePlaneClipper
 *
 * \brief SlicePlaneClipper uses a SlicePlanesProxy to clip several volumes.
 * \date Aug 20, 2010
 * \author christiana
 */
class SlicePlaneClipper : public QObject
{
  Q_OBJECT
public:
  typedef std::set<ssc::VolumetricBaseRepPtr> VolumesType;

  static SlicePlaneClipperPtr New();
  ~SlicePlaneClipper();
  void setSlicer(ssc::SliceProxyPtr slicer);
  void clearVolumes();
  void addVolume(ssc::VolumetricBaseRepPtr volume);
  void removeVolume(ssc::VolumetricBaseRepPtr volume);
  VolumesType getVolumes();
  ssc::SliceProxyPtr getSlicer();
  void setInvertPlane(bool on);
  bool getInvertPlane() const;
  vtkPlanePtr getClipPlaneCopy();

  void saveClipPlaneToVolume(); ///< save the current clip to image
  void clearClipPlanesInVolume(); ///< clear all saved clips in the image.

private slots:
  void changedSlot();
  void volumeRepChangedSlot();
private:
  void addClipPlane(ssc::VolumetricBaseRepPtr volume, vtkPlanePtr clipPlane);
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
