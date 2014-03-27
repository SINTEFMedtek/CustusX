// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#ifndef CXSPHEREMETRICREP2D_H
#define CXSPHEREMETRICREP2D_H

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
 * \ingroup cx_resource_visualization
 * \ingroup cx_resource_visualization_rep2D
 * \date 2014-03-27
 * \author christiana
 */
class SphereMetricRep2D: public DataMetricRep
{
Q_OBJECT
public:
	static SphereMetricRep2DPtr New(const QString& uid, const QString& name = ""); ///constructor
	virtual ~SphereMetricRep2D() {}

	virtual QString getType() const { return "SphereMetricRep2D"; }
	void setSliceProxy(SliceProxyPtr slicer);

protected:
	virtual void addRepActorsToViewRenderer(View* view);
	virtual void removeRepActorsFromViewRenderer(View* view);

	virtual void clear();
	virtual void onModifiedStartRender();

private:
	SphereMetricRep2D(const QString& uid, const QString& name = "");
	SphereMetricRep2D(); ///< not implemented

	SphereMetricPtr getSphereMetric();

	SliceProxyPtr mSliceProxy;
	GraphicalDiskPtr mDisk;
};


} // namespace cx


#endif // CXSPHEREMETRICREP2D_H
