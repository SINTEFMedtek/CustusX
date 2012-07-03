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

#ifndef SSCGUIDEREP2D_H_
#define SSCGUIDEREP2D_H_

#include "sscDataMetricRep.h"
#include "sscGraphicalPrimitives.h"
#include "sscPointMetric.h"
#include "sscViewportListener.h"

namespace ssc
{

typedef boost::shared_ptr<class GuideRep2D> GuideRep2DPtr;

/**Rep for visualizing a PointMetric in 2D views.
 *
 * \ingroup sscRep
 * \ingroup sscRep2D
 *
 * \date Jun 14, 2012
 * \author Sigmund Augdal, SonoWand AS
 */
class GuideRep2D: public DataMetricRep
{
Q_OBJECT
public:
	static GuideRep2DPtr New(const QString& uid, const QString& name = ""); ///constructor
	virtual ~GuideRep2D() {}

	void setPointMetric(PointMetricPtr point);
	PointMetricPtr getPointMetric();
	virtual QString getType() const { return "ssc::GuideRep2D"; }
	void setSliceProxy(ssc::SliceProxyPtr slicer);
	/**
	 * Set the width of the outline in fractions of the full size
	 */
	void setOutlineWidth(double width);

	void setRequestedAccuracy(double accuracy);

protected:
	virtual void addRepActorsToViewRenderer(ssc::View* view);
	virtual void removeRepActorsFromViewRenderer(ssc::View* view);

protected slots:
	virtual void changedSlot();

private:
	GuideRep2D(const QString& uid, const QString& name = "");
	GuideRep2D(); ///< not implemented

	PointMetricPtr mMetric;
	ssc::View* mView;
	ssc::SliceProxyPtr mSliceProxy;
	vtkActorPtr mCircleActor;
	vtkSectorSourcePtr mCircleSource;
	double mOutlineWidth;
	double mRequestedAccuracy;
};

}

#endif /* SSCGUIDEREP2D_H_ */
