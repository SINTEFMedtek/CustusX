/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXPOINTMETRICREP_H_
#define CXPOINTMETRICREP_H_

#include "cxResourceVisualizationExport.h"

#include "cxDataMetricRep.h"
#include "cxGraphicalPrimitives.h"
#include "cxPointMetric.h"
#include "cxViewportListener.h"

namespace cx
{

typedef boost::shared_ptr<class PointMetricRep> PointMetricRepPtr;

/** Rep for visualizing a PointMetric.
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep3D
 *
 * \date Jul 5, 2011
 * \author Christian Askeland, SINTEF
 */
class cxResourceVisualization_EXPORT PointMetricRep: public DataMetricRep
{
Q_OBJECT
public:
	static PointMetricRepPtr New(const QString& uid=""); ///constructor
	virtual ~PointMetricRep() {}

	void setPointMetric(PointMetricPtr point);
	PointMetricPtr getPointMetric();
	virtual QString getType() const { return "PointMetricRep"; }

protected:
	virtual void addRepActorsToViewRenderer(ViewPtr view);
	virtual void removeRepActorsFromViewRenderer(ViewPtr view);

    virtual void clear();
    virtual void rescale();
	virtual void onModifiedStartRender();

private:
	PointMetricRep();

	GraphicalPoint3DPtr mGraphicalPoint;
	ViewportListenerPtr mViewportListener;
};

}

#endif /* CXPOINTMETRICREP_H_ */
