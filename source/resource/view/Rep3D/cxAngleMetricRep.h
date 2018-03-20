/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXANGLEMETRICREP_H_
#define CXANGLEMETRICREP_H_

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

typedef boost::shared_ptr<class AngleMetricRep> AngleMetricRepPtr;
typedef boost::shared_ptr<class AngleMetric> AngleMetricPtr;

/** Rep for visualizing a AngleMetric.
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep3D
 *
 * \date Jul 27, 2011
 * \author Christian Askeland, SINTEF
 */
class cxResourceVisualization_EXPORT AngleMetricRep: public DataMetricRep
{
Q_OBJECT
public:
	static AngleMetricRepPtr New(const QString& uid = ""); ///constructor
	virtual ~AngleMetricRep() {}

	virtual QString getType() const { return "AngleMetricRep"; }

protected:
    virtual void clear();
	virtual void onModifiedStartRender();

private:
	AngleMetricRep(const QString& uid, const QString& name = "");
	AngleMetricRep(); ///< not implemented
    AngleMetricPtr getAngleMetric();

	GraphicalLine3DPtr mLine0;
	GraphicalLine3DPtr mLine1;
	GraphicalArc3DPtr mArc;
};

}

#endif /* CXANGLEMETRICREP_H_ */
