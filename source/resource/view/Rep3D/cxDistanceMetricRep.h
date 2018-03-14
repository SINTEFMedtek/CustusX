/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/



#ifndef CXDISTANCEMETRICREP_H_
#define CXDISTANCEMETRICREP_H_

#include "cxResourceVisualizationExport.h"

#include "cxDataMetricRep.h"
#include "cxGraphicalPrimitives.h"
#include "cxDistanceMetric.h"
#include "vtkForwardDeclarations.h"

#include "vtkTextActor.h"
typedef vtkSmartPointer<vtkTextActor> vtkTextActorPtr;

namespace cx
{

typedef boost::shared_ptr<class DistanceMetricRep> DistanceMetricRepPtr;

/** \brief Rep for visualizing a DistanceMetric.
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep3D
 *
 * \date Jul 5, 2011
 * \author Christian Askeland, SINTEF
 */
class cxResourceVisualization_EXPORT DistanceMetricRep: public DataMetricRep
{
Q_OBJECT
public:
	static DistanceMetricRepPtr New(const QString& uid=""); ///constructor
	virtual ~DistanceMetricRep() {}

	virtual QString getType() const { return "DistanceMetricRep"; }

protected:
    virtual void clear();
	virtual void onModifiedStartRender();

private:
	DistanceMetricRep();
    DistanceMetricPtr getDistanceMetric();

	GraphicalLine3DPtr mGraphicalLine;
};

}

#endif /* CXDISTANCEMETRICREP_H_ */
