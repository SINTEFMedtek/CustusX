/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXPOINTMETRICREP2D_H_
#define CXPOINTMETRICREP2D_H_

#include "cxResourceVisualizationExport.h"

#include "cxDataMetricRep.h"
#include "cxGraphicalPrimitives.h"
#include "cxPointMetric.h"
#include "cxViewportListener.h"

namespace cx
{

typedef boost::shared_ptr<class GraphicalDisk> GraphicalDiskPtr;
typedef boost::shared_ptr<class PointMetricRep2D> PointMetricRep2DPtr;

/**Rep for visualizing a PointMetric in 2D views.
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep2D
 *
 * \date Jun 14, 2012
 * \author Sigmund Augdal, SonoWand AS
 */
class cxResourceVisualization_EXPORT PointMetricRep2D: public DataMetricRep
{
Q_OBJECT
public:
	static PointMetricRep2DPtr New(const QString& uid=""); ///constructor
	virtual ~PointMetricRep2D() {}

	virtual QString getType() const { return "PointMetricRep2D"; }
	void setSliceProxy(SliceProxyPtr slicer);
	void setDynamicSize(bool on);

protected:
	virtual void addRepActorsToViewRenderer(ViewPtr view);
	virtual void removeRepActorsFromViewRenderer(ViewPtr view);

    virtual void clear();
//    virtual void rescale();
	virtual void onModifiedStartRender();

private:
	PointMetricRep2D(const QString& uid, const QString& name = "");
	PointMetricRep2D(); ///< not implemented

	double findSphereRadius();

	ViewportListenerPtr mViewportListener;
	SliceProxyPtr mSliceProxy;

	GraphicalDiskPtr mDisk;
};



}

#endif /* CXPOINTMETRICREP2D_H_ */
