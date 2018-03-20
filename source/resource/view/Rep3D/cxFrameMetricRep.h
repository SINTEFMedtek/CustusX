/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXFRAMEMETRICREP_H
#define CXFRAMEMETRICREP_H

#include "cxResourceVisualizationExport.h"

#include "cxDataMetricRep.h"
#include "cxGraphicalPrimitives.h"
#include "cxFrameMetric.h"
#include "cxViewportListener.h"

namespace cx
{
typedef boost::shared_ptr<class GraphicalAxes3D> GraphicalAxes3DPtr;
}

namespace cx
{

typedef boost::shared_ptr<class FrameMetricRep> FrameMetricRepPtr;

/** Rep for visualizing a FrameMetric.
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep3D
 *
 * \date Aug 26, 2013
 * \author Christian Askeland, SINTEF
 */
class cxResourceVisualization_EXPORT FrameMetricRep: public DataMetricRep
{
Q_OBJECT
public:
	static FrameMetricRepPtr New(const QString& uid=""); ///constructor
	virtual ~FrameMetricRep() {}
	virtual QString getType() const { return "FrameMetricRep"; }

protected:
    virtual void clear();
	virtual void onModifiedStartRender();

private:
	FrameMetricRep(); ///< not implemented
    FrameMetricPtr getFrameMetric();

    GraphicalAxes3DPtr mAxes;
};

} // namespace cx

#endif // CXFRAMEMETRICREP_H
