/*
 * cxInteractiveClipper.h
 *
 *  Created on: Aug 24, 2010
 *      Author: christiana
 */

#ifndef CXINTERACTIVECLIPPER_H_
#define CXINTERACTIVECLIPPER_H_

#include <vector>
#include <QObject>
#include "sscDefinitions.h"
#include "cxForwardDeclarations.h"

namespace cx
{

/**Helper class for clipping the active volume using a specific slice plane.
 * The visible slice planes are the only ones allowed for clipping.
 */
class InteractiveClipper : public QObject
{
  Q_OBJECT
public:
  InteractiveClipper(ssc::SlicePlanesProxyPtr slicePlanesProxy);

  void setSlicePlane(ssc::PLANE_TYPE plane);
  void saveClipPlaneToVolume(); ///< save the current clip to image
  void clearClipPlanesInVolume(); ///< clear all saved clips in the image.
  ssc::PLANE_TYPE getSlicePlane();
  bool getUseClipper() const;
  bool getInvertPlane() const;
  std::vector<ssc::PLANE_TYPE> getAvailableSlicePlanes() const;
signals:
  void changed();
public slots:
  void useClipper(bool on);
  void invertPlane(bool on);
private slots:
//  void activeLayoutChangedSlot();
//  void activeImageChangedSlot();
  void changedSlot();

private:
  ssc::PLANE_TYPE getPlaneType();
  ssc::SlicePlaneClipperPtr mSlicePlaneClipper;
  ssc::SlicePlanesProxyPtr mSlicePlanesProxy;
  bool mUseClipper;
};
typedef boost::shared_ptr<InteractiveClipper> InteractiveClipperPtr;

} // namespace cx

#endif /* CXINTERACTIVECLIPPER_H_ */
