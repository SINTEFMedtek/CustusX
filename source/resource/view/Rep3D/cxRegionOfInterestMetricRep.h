/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXREGIONOFINTERESTMETRICREP_H
#define CXREGIONOFINTERESTMETRICREP_H

#include "cxResourceVisualizationExport.h"

#include "cxDataMetricRep.h"
#include "cxGraphicalPrimitives.h"
#include "cxRegionOfInterestMetric.h"
#include "vtkForwardDeclarations.h"

#include "vtkTextActor.h"
typedef vtkSmartPointer<vtkTextActor> vtkTextActorPtr;

namespace cx
{

typedef boost::shared_ptr<class RegionOfInterestMetricRep> RegionOfInterestMetricRepPtr;
typedef boost::shared_ptr<class GraphicalBox> GraphicalBoxPtr;

/** \brief Rep for visualizing a RegionOfInterestMetric.
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep3D
 *
 */
class cxResourceVisualization_EXPORT RegionOfInterestMetricRep: public DataMetricRep
{
Q_OBJECT
public:
	static RegionOfInterestMetricRepPtr New(const QString& uid=""); ///constructor
	virtual ~RegionOfInterestMetricRep() {}

	virtual QString getType() const { return "RegionOfInterestMetricRep"; }

protected:
	virtual void clear();
	virtual void onModifiedStartRender();

private:
	RegionOfInterestMetricRep();
	RegionOfInterestMetricPtr getRegionOfInterestMetric();

	GraphicalBoxPtr mGraphicalBox;
};

}

#endif // CXREGIONOFINTERESTMETRICREP_H
