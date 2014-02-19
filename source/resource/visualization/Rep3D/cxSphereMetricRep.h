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

#ifndef CXSPHEREMETRICREP_H
#define CXSPHEREMETRICREP_H

#include "sscDataMetricRep.h"
#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"

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
 * \ingroup sscRep
 * \ingroup sscRep3D
 *
 * \date 2014-02-11
 * \author Christian Askeland, SINTEF
 */
class SphereMetricRep: public DataMetricRep
{
Q_OBJECT
public:
	static SphereMetricRepPtr New(const QString& uid, const QString& name = ""); ///constructor
	virtual ~SphereMetricRep() {}

	virtual QString getType() const { return "SphereMetricRep"; }

protected:
	virtual void clear();
	virtual void onModifiedStartRender();

private:
	SphereMetricRep(const QString& uid, const QString& name = "");
	SphereMetricRep(); ///< not implemented
	SphereMetricPtr getSphereMetric();

	GraphicalPoint3DPtr mGraphicalPoint;
};

}

#endif // CXSPHEREMETRICREP_H
