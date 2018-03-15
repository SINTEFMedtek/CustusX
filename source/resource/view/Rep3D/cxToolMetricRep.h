/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXTOOLMETRICREP_H
#define CXTOOLMETRICREP_H

#include "cxResourceVisualizationExport.h"

#include "cxDataMetricRep.h"
#include "cxGraphicalPrimitives.h"
#include "cxToolMetric.h"
#include "cxViewportListener.h"

namespace cx
{
typedef boost::shared_ptr<class GraphicalAxes3D> GraphicalAxes3DPtr;
}

namespace cx
{

typedef boost::shared_ptr<class ToolMetricRep> ToolMetricRepPtr;

/** Rep for visualizing a ToolMetric.
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep3D
 *
 * \date Aug 30, 2013
 * \author Christian Askeland, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 */
class cxResourceVisualization_EXPORT ToolMetricRep: public DataMetricRep
{
Q_OBJECT
public:
	static ToolMetricRepPtr New(const QString& uid = ""); ///constructor
	virtual ~ToolMetricRep() {}
	virtual QString getType() const { return "ToolMetricRep"; }

protected:
	virtual void clear();
	void addRepActorsToViewRenderer(ViewPtr view);
	void removeRepActorsFromViewRenderer(ViewPtr view);
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
