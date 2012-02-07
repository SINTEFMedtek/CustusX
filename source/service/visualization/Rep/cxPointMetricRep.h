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
 * cxPointMetricRep.h
 *
 *  \date Jul 5, 2011
 *      \author christiana
 */

#ifndef CXPOINTMETRICREP_H_
#define CXPOINTMETRICREP_H_

#include "cxDataMetricRep.h"
#include "sscGraphicalPrimitives.h"
#include "cxPointMetric.h"
#include "sscViewportListener.h"

namespace cx
{

typedef boost::shared_ptr<class PointMetricRep> PointMetricRepPtr;

/**Rep for visualizing a PointMetric.
 * \ingroup cxServiceVisualizationRep
 *
 */
class PointMetricRep: public DataMetricRep
{
Q_OBJECT
public:
	static PointMetricRepPtr New(const QString& uid, const QString& name = ""); ///constructor
	virtual ~PointMetricRep() {}

	void setPointMetric(PointMetricPtr point);
	virtual QString getType() const { return "cx::PointMetricRep"; }

protected:
	virtual void addRepActorsToViewRenderer(ssc::View* view);
	virtual void removeRepActorsFromViewRenderer(ssc::View* view);

	virtual void rescale();

protected slots:
	virtual void changedSlot();

private:
	PointMetricRep(const QString& uid, const QString& name = "");
	PointMetricRep(); ///< not implemented

	ssc::GraphicalPoint3DPtr mGraphicalPoint;
	ssc::CaptionText3DPtr mText;
	PointMetricPtr mMetric;
	ssc::View* mView;
	ssc::ViewportListenerPtr mViewportListener;
};

}

#endif /* CXPOINTMETRICREP_H_ */
