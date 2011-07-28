/*
 * cxPlaneMetricRep.h
 *
 *  Created on: Jul 27, 2011
 *      Author: christiana
 */

#ifndef CXPLANEMETRICREP_H_
#define CXPLANEMETRICREP_H_

#include <sscRepImpl.h>
#include "sscGraphicalPrimitives.h"
#include "cxPlaneMetric.h"
#include "sscViewportListener.h"

namespace cx
{

typedef boost::shared_ptr<class PlaneMetricRep> PlaneMetricRepPtr;

/**Rep for visualizing a PlaneMetric.
 *
 */
class PlaneMetricRep : public ssc::RepImpl
{
  Q_OBJECT
public:
  static PlaneMetricRepPtr New(const QString& uid, const QString& name=""); ///constructor
  virtual ~PlaneMetricRep() {}

  void setMetric(PlaneMetricPtr point);
  void setSphereRadius(double radius);
  virtual QString getType() const { return "cx::PlaneMetricRep"; }

protected:
  virtual void addRepActorsToViewRenderer(ssc::View* view);
  virtual void removeRepActorsFromViewRenderer(ssc::View* view);

protected slots:
  void changedSlot();

private:
  PlaneMetricRep(const QString& uid, const QString& name="");
  PlaneMetricRep(); ///< not implemented

  double getVpnZoom();
  void scaleText();

  ssc::GraphicalPoint3DPtr mGraphicalPoint;
  ssc::GraphicalArrow3DPtr mNormal;
  PlaneMetricPtr mMetric;
  ssc::View* mView;
  double mSphereRadius;
  ssc::Vector3D mColor;
  ssc::ViewportListenerPtr mViewportListener;
};

}

#endif /* CXPLANEMETRICREP_H_ */
