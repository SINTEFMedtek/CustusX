/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXPLANEMETRICREP_H_
#define CXPLANEMETRICREP_H_

#include "cxResourceVisualizationExport.h"

#include "cxDataMetricRep.h"
#include "cxGraphicalPrimitives.h"
#include "cxPlaneMetric.h"
#include "cxViewportListener.h"

namespace cx
{

typedef boost::shared_ptr<class PlaneMetricRep> PlaneMetricRepPtr;

/** \brief Rep for visualizing a PlaneMetric.
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep3D
 *
 * \date Jul 27, 2011
 * \author Christian Askeland, SINTEF
 */
class cxResourceVisualization_EXPORT PlaneMetricRep: public DataMetricRep
{
Q_OBJECT
public:
	static PlaneMetricRepPtr New(const QString& uid = ""); ///constructor
	virtual ~PlaneMetricRep() {}

	virtual QString getType() const { return "PlaneMetricRep"; }

protected:
	virtual void addRepActorsToViewRenderer(ViewPtr view);
	virtual void removeRepActorsFromViewRenderer(ViewPtr view);
    virtual void clear();
	virtual void onModifiedStartRender();

private:
	PlaneMetricRep();

	void rescale();
	PlaneMetricPtr getPlaneMetric();
	void drawRectangleForPlane(Vector3D p0_r, Vector3D n_r, double size);

	GraphicalPoint3DPtr mGraphicalPoint;
	GraphicalArrow3DPtr mNormal;
	Rect3DPtr mRect;
	ViewportListenerPtr mViewportListener;
};

}

#endif /* CXPLANEMETRICREP_H_ */
