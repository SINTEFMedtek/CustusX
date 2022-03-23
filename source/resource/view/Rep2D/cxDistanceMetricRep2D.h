/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXDISTANCEMETRICREP2D_H_
#define CXDISTANCEMETRICREP2D_H_

#include "cxResourceVisualizationExport.h"

#include "cxDataMetricRep.h"
#include "cxDistanceMetric.h"
#include "cxViewportListener.h"

namespace cx
{

typedef boost::shared_ptr<class DistanceMetricRep2D> DistanceMetricRep2DPtr;

/**Rep for visualizing a PointMetric in 2D views.
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep2D
 *
 * \date Feb 19, 2022
 * \author Ole Vegard Solberg, SINTEF
 */
class cxResourceVisualization_EXPORT DistanceMetricRep2D: public DataMetricRep
{
	Q_OBJECT
public:
	static DistanceMetricRep2DPtr New(const QString& uid=""); ///constructor
	virtual ~DistanceMetricRep2D() {}

	virtual QString getType() const { return "DistanceMetricRep2D"; }
	void setSliceProxy(SliceProxyPtr slicer);
	void setDynamicSize(bool on);

protected:
	virtual void addRepActorsToViewRenderer(ViewPtr view);
	virtual void removeRepActorsFromViewRenderer(ViewPtr view);

	virtual void clear();
	virtual void onModifiedStartRender();

private:
	DistanceMetricRep2D(const QString& uid, const QString& name = "");
	DistanceMetricRep2D(); ///< not implemented

	ViewportListenerPtr mViewportListener;
	SliceProxyPtr mSliceProxy;

	LineSegmentPtr mLine;
};



}

#endif /* CXDISTANCEMETRICREP2D_H_ */
