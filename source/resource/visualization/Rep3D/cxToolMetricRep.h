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
#ifndef CXTOOLMETRICREP_H
#define CXTOOLMETRICREP_H

#include "sscDataMetricRep.h"
#include "sscGraphicalPrimitives.h"
#include "cxToolMetric.h"
#include "sscViewportListener.h"

namespace cx
{
typedef boost::shared_ptr<class GraphicalAxes3D> GraphicalAxes3DPtr;
}

namespace cx
{

typedef boost::shared_ptr<class ToolMetricRep> ToolMetricRepPtr;

/** Rep for visualizing a ToolMetric.
 *
 * \ingroup sscRep
 * \ingroup sscRep3D
 *
 * \date Aug 30, 2013
 * \author Christian Askeland, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 */
class ToolMetricRep: public DataMetricRep
{
Q_OBJECT
public:
	static ToolMetricRepPtr New(const QString& uid, const QString& name = ""); ///constructor
	virtual ~ToolMetricRep() {}
	virtual QString getType() const { return "ToolMetricRep"; }

protected:
	virtual void clear();
	void addRepActorsToViewRenderer(View *view);
	void removeRepActorsFromViewRenderer(View *view);
	virtual void onModifiedStartRender();

private:
	ToolMetricRep(const QString& uid, const QString& name = "");
	ToolMetricRep(); ///< not implemented
	ToolMetricPtr getToolMetric();
	void rescale();

	GraphicalAxes3DPtr mAxes;

	GraphicalPoint3DPtr mToolTip;
	GraphicalLine3DPtr mToolOffset;
	ViewportListenerPtr mViewportListener;

};

} // namespace cx

#endif // CXTOOLMETRICREP_H
