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

namespace ssc
{

typedef boost::shared_ptr<class PlaneMetricRep> PlaneMetricRepPtr;

/**\brief Rep for visualizing a PlaneMetric.
 *
 * \ingroup sscRep
 * \ingroup sscRep3D
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

	void setMetric(PlaneMetricPtr point);
	virtual QString getType() const { return "ssc::PlaneMetricRep"; }

protected:
	virtual void addRepActorsToViewRenderer(ssc::View *view);
	virtual void removeRepActorsFromViewRenderer(ssc::View *view);

protected slots:
	virtual void changedSlot();

private:
	PlaneMetricRep(const QString& uid, const QString& name = "");
	PlaneMetricRep(); ///< not implemented

	void rescale();

	ssc::GraphicalPoint3DPtr mGraphicalPoint;
	ssc::CaptionText3DPtr mText;
	ssc::GraphicalArrow3DPtr mNormal;
	ssc::Rect3DPtr mRect;
	PlaneMetricPtr mMetric;
	ssc::View *mView;
	ssc::ViewportListenerPtr mViewportListener;
};

}

#endif /* SSCPLANEMETRICREP_H_ */
