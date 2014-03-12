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

#ifndef SSCPLANEMETRICREP_H_
#define SSCPLANEMETRICREP_H_

#include "sscDataMetricRep.h"
#include "sscGraphicalPrimitives.h"
#include "sscPlaneMetric.h"
#include "sscViewportListener.h"

namespace cx
{

typedef boost::shared_ptr<class PlaneMetricRep> PlaneMetricRepPtr;

/** \brief Rep for visualizing a PlaneMetric.
 *
 * \ingroup cx_resource_visualization
 * \ingroup cx_resource_visualization_rep3D
 *
 * \date Jul 27, 2011
 * \author Christian Askeland, SINTEF
 */
class PlaneMetricRep: public DataMetricRep
{
Q_OBJECT
public:
	static PlaneMetricRepPtr New(const QString& uid, const QString& name = ""); ///constructor
	virtual ~PlaneMetricRep() {}

	virtual QString getType() const { return "PlaneMetricRep"; }

protected:
	virtual void addRepActorsToViewRenderer(View *view);
	virtual void removeRepActorsFromViewRenderer(View *view);
    virtual void clear();
	virtual void onModifiedStartRender();

private:
	PlaneMetricRep(const QString& uid, const QString& name = "");
	PlaneMetricRep(); ///< not implemented

	void rescale();
	PlaneMetricPtr getPlaneMetric();
	void drawRectangleForPlane(Vector3D p0_r, Vector3D n_r, double size);

	GraphicalPoint3DPtr mGraphicalPoint;
	GraphicalArrow3DPtr mNormal;
	Rect3DPtr mRect;
	ViewportListenerPtr mViewportListener;
};

}

#endif /* SSCPLANEMETRICREP_H_ */
