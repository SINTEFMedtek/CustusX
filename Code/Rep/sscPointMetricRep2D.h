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

#ifndef SSCPOINTMETRICREP2D_H_
#define SSCPOINTMETRICREP2D_H_

#include "sscDataMetricRep.h"
#include "sscGraphicalPrimitives.h"
#include "sscPointMetric.h"
#include "sscViewportListener.h"

namespace ssc
{

typedef boost::shared_ptr<class PointMetricRep2D> PointMetricRep2DPtr;

/**Rep for visualizing a PointMetric in 2D views.
 *
 * \ingroup sscRep
 * \ingroup sscRep2D
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

	void setPointMetric(PointMetricPtr point);
	PointMetricPtr getPointMetric();
	virtual QString getType() const { return "ssc::PointMetricRep2D"; }
	void setSliceProxy(ssc::SliceProxyPtr slicer);
	/**
	 * Set the width of the outline in fractions of the full size
	 */
	void setOutlineWidth(double width);
	/**
	 * Set the color of the outline
	 */
	void setOutlineColor(double red, double green, double blue);
	void setFillVisibility(bool on);
	void setDynamicSize(bool on);

protected:
	virtual void addRepActorsToViewRenderer(ssc::View* view);
	virtual void removeRepActorsFromViewRenderer(ssc::View* view);

	virtual void rescale();

protected slots:
	virtual void changedSlot();

private:
	PointMetricRep2D(const QString& uid, const QString& name = "");
	PointMetricRep2D(); ///< not implemented

	ssc::CaptionText3DPtr mText;
	PointMetricPtr mMetric;
	ssc::View* mView;
	ssc::ViewportListenerPtr mViewportListener;
	ssc::SliceProxyPtr mSliceProxy;
	vtkActorPtr mCircleActor;
	vtkSectorSourcePtr mCircleSource;
	vtkActorPtr mOutlineActor;
	vtkSectorSourcePtr mOutlineSource;
	double mOutlineWidth;
	ssc::Vector3D mOutlineColor;
	bool mFillVisible;
};



}

#endif /* SSCPOINTMETRICREP2D_H_ */
