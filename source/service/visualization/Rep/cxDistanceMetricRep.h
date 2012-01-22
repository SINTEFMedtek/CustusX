/*
 * cxDistanceMetricRep.h
 *
 *  Created on: Jul 5, 2011
 *      Author: christiana
 */

#ifndef CXDISTANCEMETRICREP_H_
#define CXDISTANCEMETRICREP_H_

#include "cxDataMetricRep.h"
#include "sscGraphicalPrimitives.h"
#include "cxDistanceMetric.h"
#include "vtkForwardDeclarations.h"

#include "vtkTextActor.h"
typedef vtkSmartPointer<vtkTextActor> vtkTextActorPtr;

namespace cx
{

typedef boost::shared_ptr<class DistanceMetricRep> DistanceMetricRepPtr;

/**\brief Rep for visualizing a DistanceMetric.
 * \ingroup cxServiceVisualizationRep
 *
 */
class DistanceMetricRep : public DataMetricRep
{
	Q_OBJECT
public:
  static DistanceMetricRepPtr New(const QString& uid, const QString& name=""); ///constructor
  virtual ~DistanceMetricRep() {}

	void setDistanceMetric(DistanceMetricPtr point);
	virtual QString getType() const { return "cx::DistanceMetricRep"; }

protected:
  virtual void addRepActorsToViewRenderer(ssc::View* view);
  virtual void removeRepActorsFromViewRenderer(ssc::View* view);

protected slots:
	virtual void changedSlot();

private:
	DistanceMetricRep(const QString& uid, const QString& name="");
	DistanceMetricRep(); ///< not implemented

  ssc::GraphicalLine3DPtr mGraphicalLine;
  ssc::CaptionText3DPtr mText;
  DistanceMetricPtr mMetric;
	ssc::View* mView;
};

}

#endif /* CXDISTANCEMETRICREP_H_ */
