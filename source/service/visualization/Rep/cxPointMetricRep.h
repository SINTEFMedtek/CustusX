/*
 * cxPointMetricRep.h
 *
 *  Created on: Jul 5, 2011
 *      Author: christiana
 */

#ifndef CXPOINTMETRICREP_H_
#define CXPOINTMETRICREP_H_

#include <sscRepImpl.h>
#include "sscGraphicalPrimitives.h"
#include "cxPointMetric.h"
#include "sscViewportListener.h"

namespace cx
{

typedef boost::shared_ptr<class PointMetricRep> PointMetricRepPtr;

/**Rep for visualizing a PointMetric.
 *
 */
class PointMetricRep : public ssc::RepImpl
{
	Q_OBJECT
public:
  static PointMetricRepPtr New(const QString& uid, const QString& name=""); ///constructor
  virtual ~PointMetricRep() {}

	void setPointMetric(PointMetricPtr point);
	void setSphereRadius(double radius);
	virtual QString getType() const { return "cx::PointMetricRep"; }

protected:
  virtual void addRepActorsToViewRenderer(ssc::View* view);
  virtual void removeRepActorsFromViewRenderer(ssc::View* view);

protected slots:
	void changedSlot();

private:
  PointMetricRep(const QString& uid, const QString& name="");
  PointMetricRep(); ///< not implemented

  void scaleText();
  ssc::GraphicalPoint3DPtr mGraphicalPoint;
  PointMetricPtr mPointMetric;
	ssc::View* mView;
	double mSphereRadius;
  ssc::ViewportListenerPtr mViewportListener;
};

}

#endif /* CXPOINTMETRICREP_H_ */
