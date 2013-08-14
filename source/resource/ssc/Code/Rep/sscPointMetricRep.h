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

#ifndef SSCPOINTMETRICREP_H_
#define SSCPOINTMETRICREP_H_

#include "sscDataMetricRep.h"
#include "sscGraphicalPrimitives.h"
#include "sscPointMetric.h"
#include "sscViewportListener.h"

namespace ssc
{

typedef boost::shared_ptr<class PointMetricRep> PointMetricRepPtr;

/**Rep for visualizing a PointMetric.
 *
 * \ingroup sscRep
 * \ingroup sscRep3D
 *
 * \date Jul 5, 2011
 * \author Christian Askeland, SINTEF
 */
class PointMetricRep: public DataMetricRep
{
Q_OBJECT
public:
	static PointMetricRepPtr New(const QString& uid, const QString& name = ""); ///constructor
	virtual ~PointMetricRep() {}

	void setPointMetric(PointMetricPtr point);
	PointMetricPtr getPointMetric();
	virtual QString getType() const { return "ssc::PointMetricRep"; }

protected:
	virtual void addRepActorsToViewRenderer(ssc::View *view);
	virtual void removeRepActorsFromViewRenderer(ssc::View *view);

	virtual void rescale();

protected slots:
	virtual void changedSlot();

private:
	PointMetricRep(const QString& uid, const QString& name = "");
	PointMetricRep(); ///< not implemented

	ssc::GraphicalPoint3DPtr mGraphicalPoint;
	ssc::CaptionText3DPtr mText;
	PointMetricPtr mMetric;
	ssc::View *mView;
	ssc::ViewportListenerPtr mViewportListener;
};

}

#endif /* SSCPOINTMETRICREP_H_ */
