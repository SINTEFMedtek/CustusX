/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXDONUTMETRICREP_H
#define CXDONUTMETRICREP_H

#include "cxResourceVisualizationExport.h"

#include "cxDataMetricRep.h"
#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"


namespace cx
{
typedef boost::shared_ptr<class GraphicalTorus3D> GraphicalTorus3DPtr;
typedef boost::shared_ptr<class DonutMetricRep> DonutMetricRepPtr;
typedef boost::shared_ptr<class DonutMetric> DonutMetricPtr;
typedef boost::shared_ptr<class GraphicalDisk> GraphicalDiskPtr;

/**Rep for visualizing a DonutMetric.
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep3D
 *
 * \date 2014-02-11
 * \author Christian Askeland, SINTEF
 */
class cxResourceVisualization_EXPORT DonutMetricRep: public DataMetricRep
{
Q_OBJECT
public:
	static DonutMetricRepPtr New(const QString& uid = ""); ///constructor
	virtual ~DonutMetricRep() {}

	virtual QString getType() const { return "DonutMetricRep"; }

protected:
	virtual void clear();
	virtual void onModifiedStartRender();

private:
	DonutMetricRep();
	DonutMetricPtr getDonutMetric();
	void updateTorus();
	void updateDisc();

	GraphicalTorus3DPtr mTorus;
	GraphicalDiskPtr mDisk;
};

}

#endif // CXDONUTMETRICREP_H
