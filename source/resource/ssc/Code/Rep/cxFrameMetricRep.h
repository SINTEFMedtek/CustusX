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
#ifndef CXFRAMEMETRICREP_H
#define CXFRAMEMETRICREP_H

#include "sscDataMetricRep.h"
#include "sscGraphicalPrimitives.h"
#include "cxFrameMetric.h"
#include "sscViewportListener.h"

namespace ssc
{
typedef boost::shared_ptr<class GraphicalAxes3D> GraphicalAxes3DPtr;
}

namespace cx
{

typedef boost::shared_ptr<class FrameMetricRep> FrameMetricRepPtr;

/** Rep for visualizing a FrameMetric.
 *
 * \ingroup sscRep
 * \ingroup sscRep3D
 *
 * \date Aug 26, 2013
 * \author Christian Askeland, SINTEF
 */
class FrameMetricRep: public ssc::DataMetricRep
{
Q_OBJECT
public:
	static FrameMetricRepPtr New(const QString& uid, const QString& name = ""); ///constructor
	virtual ~FrameMetricRep() {}
	virtual QString getType() const { return "ssc::FrameMetricRep"; }

protected:
    virtual void clear();

protected slots:
	virtual void changedSlot();

private:
	FrameMetricRep(const QString& uid, const QString& name = "");
	FrameMetricRep(); ///< not implemented
    FrameMetricPtr getFrameMetric();

    ssc::GraphicalAxes3DPtr mAxes;
};

} // namespace cx

#endif // CXFRAMEMETRICREP_H
