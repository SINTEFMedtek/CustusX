// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#ifndef CXPOINTMETRICREP2D_H_
#define CXPOINTMETRICREP2D_H_

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
 * \ingroup cx_resource_visualization
 * \ingroup cx_resource_visualization_rep2D
 *
 * \date Jun 14, 2012
 * \author Sigmund Augdal, SonoWand AS
 */
class PointMetricRep2D: public DataMetricRep
{
Q_OBJECT
public:
	static PointMetricRep2DPtr New(const QString& uid, const QString& name = ""); ///constructor
	virtual ~PointMetricRep2D() {}

	virtual QString getType() const { return "PointMetricRep2D"; }
	void setSliceProxy(SliceProxyPtr slicer);
	void setDynamicSize(bool on);

protected:
	virtual void addRepActorsToViewRenderer(View* view);
	virtual void removeRepActorsFromViewRenderer(View* view);

    virtual void clear();
    virtual void rescale();
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
