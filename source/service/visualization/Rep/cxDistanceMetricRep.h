/*
 * cxDistanceMetricRep.h
 *
 *  Created on: Jul 5, 2011
 *      Author: christiana
 */

#ifndef CXDISTANCEMETRICREP_H_
#define CXDISTANCEMETRICREP_H_

#include <sscRepImpl.h>
#include "sscGraphicalPrimitives.h"
#include "cxDistanceMetric.h"
#include "vtkForwardDeclarations.h"

#include "vtkTextActor.h"
typedef vtkSmartPointer<vtkTextActor> vtkTextActorPtr;

namespace cx
{

typedef boost::shared_ptr<class DistanceMetricRep> DistanceMetricRepPtr;

/**Rep for visualizing a DistanceMetric.
 *
 */
class DistanceMetricRep : public ssc::RepImpl
{
	Q_OBJECT
public:
  static DistanceMetricRepPtr New(const QString& uid, const QString& name=""); ///constructor
  virtual ~DistanceMetricRep() {}

	void setDistanceMetric(DistanceMetricPtr point);
	virtual QString getType() const { return "cx::DistanceMetricRep"; }
  void setShowLabel(bool on);

protected:
  virtual void addRepActorsToViewRenderer(ssc::View* view);
  virtual void removeRepActorsFromViewRenderer(ssc::View* view);

protected slots:
	void changedSlot();

private:
	DistanceMetricRep(const QString& uid, const QString& name="");
	DistanceMetricRep(); ///< not implemented

  ssc::GraphicalLine3DPtr mGraphicalLine;
  ssc::FollowerText3DPtr mText;
  DistanceMetricPtr mMetric;
	ssc::View* mView;
	ssc::Vector3D mColor;
  bool mShowLabel;
};


}

#endif /* CXDISTANCEMETRICREP_H_ */
