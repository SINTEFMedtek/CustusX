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

/*
 * cxPlaneMetricRep.h
 *
 *  \date Jul 27, 2011
 *      \author christiana
 */

#ifndef CXPLANEMETRICREP_H_
#define CXPLANEMETRICREP_H_

#include "cxDataMetricRep.h"
#include "sscGraphicalPrimitives.h"
#include "cxPlaneMetric.h"
#include "sscViewportListener.h"

namespace cx
{

typedef boost::shared_ptr<class PlaneMetricRep> PlaneMetricRepPtr;

/**\brief Rep for visualizing a PlaneMetric.
 * \ingroup cxServiceVisualizationRep
 *
 */
class PlaneMetricRep: public DataMetricRep
{
Q_OBJECT
public:
	static PlaneMetricRepPtr New(const QString& uid, const QString& name = ""); ///constructor
	virtual ~PlaneMetricRep() {}

	void setMetric(PlaneMetricPtr point);
	virtual QString getType() const { return "cx::PlaneMetricRep"; }

protected:
	virtual void addRepActorsToViewRenderer(ssc::View* view);
	virtual void removeRepActorsFromViewRenderer(ssc::View* view);

protected slots:
	virtual void changedSlot();

private:
	PlaneMetricRep(const QString& uid, const QString& name = "");
	PlaneMetricRep(); ///< not implemented

//  double getVpnZoom();
	void rescale();

	ssc::GraphicalPoint3DPtr mGraphicalPoint;
	ssc::CaptionText3DPtr mText;
	ssc::GraphicalArrow3DPtr mNormal;
	PlaneMetricPtr mMetric;
	ssc::View* mView;
	ssc::ViewportListenerPtr mViewportListener;
};

}

#endif /* CXPLANEMETRICREP_H_ */
