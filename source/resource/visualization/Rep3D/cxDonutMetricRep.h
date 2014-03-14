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

#ifndef CXDONUTMETRICREP_H
#define CXDONUTMETRICREP_H

#include "cxDataMetricRep.h"
#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"
class QColor;

typedef vtkSmartPointer<class vtkTextActor> vtkTextActorPtr;

namespace cx
{
typedef boost::shared_ptr<class GraphicalTorus3D> GraphicalTorus3DPtr;
typedef boost::shared_ptr<class DonutMetricRep> DonutMetricRepPtr;
typedef boost::shared_ptr<class DonutMetric> DonutMetricPtr;
typedef boost::shared_ptr<class GraphicalDisk> GraphicalDiskPtr;

/**Rep for visualizing a DonutMetric.
 *
 * \ingroup cx_resource_visualization
 * \ingroup cx_resource_visualization_rep3D
 *
 * \date 2014-02-11
 * \author Christian Askeland, SINTEF
 */
class DonutMetricRep: public DataMetricRep
{
Q_OBJECT
public:
	static DonutMetricRepPtr New(const QString& uid, const QString& name = ""); ///constructor
	virtual ~DonutMetricRep() {}

	virtual QString getType() const { return "DonutMetricRep"; }

protected:
	virtual void clear();
	virtual void onModifiedStartRender();

private:
	DonutMetricRep(const QString& uid, const QString& name = "");
	DonutMetricRep(); ///< not implemented
	DonutMetricPtr getDonutMetric();
	void updateTorus();
	void updateDisc();

	GraphicalTorus3DPtr mTorus;
	GraphicalDiskPtr mDisk;
};

}

#endif // CXDONUTMETRICREP_H
