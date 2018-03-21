/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXSPHEREMETRICREP2D_H
#define CXSPHEREMETRICREP2D_H

#include "cxResourceVisualizationExport.h"

#include "cxDataMetricRep.h"
#include "cxGraphicalPrimitives.h"
#include "cxPointMetric.h"
#include "cxViewportListener.h"

namespace cx
{

typedef boost::shared_ptr<class GraphicalDisk> GraphicalDiskPtr;
typedef boost::shared_ptr<class SphereMetricRep2D> SphereMetricRep2DPtr;
typedef boost::shared_ptr<class SphereMetric> SphereMetricPtr;

/**
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep2D
 * \date 2014-03-27
 * \author christiana
 */
class cxResourceVisualization_EXPORT SphereMetricRep2D: public DataMetricRep
{
Q_OBJECT
public:
	static SphereMetricRep2DPtr New(const QString& uid=""); ///constructor
	virtual ~SphereMetricRep2D() {}

	virtual QString getType() const { return "SphereMetricRep2D"; }
	void setSliceProxy(SliceProxyPtr slicer);

protected:
	virtual void addRepActorsToViewRenderer(ViewPtr view);
	virtual void removeRepActorsFromViewRenderer(ViewPtr view);

	virtual void clear();
	virtual void onModifiedStartRender();

private:
	SphereMetricRep2D();

	SphereMetricPtr getSphereMetric();

	SliceProxyPtr mSliceProxy;
	GraphicalDiskPtr mDisk;
};


} // namespace cx


#endif // CXSPHEREMETRICREP2D_H
