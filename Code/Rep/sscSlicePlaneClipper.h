/*
 * sscSlicePlaneClipper.h
 *
 *  Created on: Aug 20, 2010
 *      Author: christiana
 */
#ifndef SSCSLICEPLANECLIPPER_H_
#define SSCSLICEPLANECLIPPER_H_

#include <set>
#include <boost/shared_ptr.hpp>
#include <QObject>
#include "sscForwardDeclarations.h"

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
  void setSliceProxy(ssc::SliceProxyPtr slicer);
  void clearVolumes();
  void addVolume(ssc::VolumetricRepPtr volume);
  void removeVolume(ssc::VolumetricRepPtr volume);
  VolumesType getVolumes();

private slots:
  void changedSlot();
private:
  ssc::SliceProxyPtr mSlicer;
  VolumesType mVolumes;
};

} // namespace ssc

#endif /* SSCSLICEPLANECLIPPER_H_ */
