/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXSPHEREMETRICREP_H
#define CXSPHEREMETRICREP_H

#include "cxResourceVisualizationExport.h"

#include "cxDataMetricRep.h"
#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"

typedef vtkSmartPointer<class vtkTextActor> vtkTextActorPtr;

namespace cx
{
typedef boost::shared_ptr<class CaptionText3D> CaptionText3DPtr;
typedef boost::shared_ptr<class GraphicalArc3D> GraphicalArc3DPtr;
}

namespace cx
{

typedef boost::shared_ptr<class SphereMetricRep> SphereMetricRepPtr;
typedef boost::shared_ptr<class SphereMetric> SphereMetricPtr;

/**Rep for visualizing a SphereMetric.
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep3D
 *
 * \date 2014-02-11
 * \author Christian Askeland, SINTEF
 */
class cxResourceVisualization_EXPORT SphereMetricRep: public DataMetricRep
{
Q_OBJECT
public:
	static SphereMetricRepPtr New(const QString& uid = ""); ///constructor
	virtual ~SphereMetricRep() {}

	virtual QString getType() const { return "SphereMetricRep"; }

protected:
	virtual void clear();
	virtual void onModifiedStartRender();

private:
	SphereMetricRep();
	SphereMetricPtr getSphereMetric();

	GraphicalPoint3DPtr mGraphicalPoint;
};

}

#endif // CXSPHEREMETRICREP_H
