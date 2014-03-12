// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.


#ifndef SSCDISTANCEMETRICREP_H_
#define SSCDISTANCEMETRICREP_H_

#include "sscDataMetricRep.h"
#include "sscGraphicalPrimitives.h"
#include "sscDistanceMetric.h"
#include "vtkForwardDeclarations.h"

#include "vtkTextActor.h"
typedef vtkSmartPointer<vtkTextActor> vtkTextActorPtr;

namespace cx
{

typedef boost::shared_ptr<class DistanceMetricRep> DistanceMetricRepPtr;

/** \brief Rep for visualizing a DistanceMetric.
 *
 * \ingroup cx_resource_visualization
 * \ingroup cx_resource_visualization_rep3D
 *
 * \date Jul 5, 2011
 * \author Christian Askeland, SINTEF
 */
class DistanceMetricRep: public DataMetricRep
{
Q_OBJECT
public:
	static DistanceMetricRepPtr New(const QString& uid, const QString& name = ""); ///constructor
	virtual ~DistanceMetricRep() {}

	virtual QString getType() const { return "DistanceMetricRep"; }

protected:
    virtual void clear();
	virtual void onModifiedStartRender();

private:
	DistanceMetricRep(const QString& uid, const QString& name = "");
	DistanceMetricRep(); ///< not implemented
    DistanceMetricPtr getDistanceMetric();

	GraphicalLine3DPtr mGraphicalLine;
};

}

#endif /* SSCDISTANCEMETRICREP_H_ */
