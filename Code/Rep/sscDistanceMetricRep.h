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

namespace ssc
{

typedef boost::shared_ptr<class DistanceMetricRep> DistanceMetricRepPtr;

/**\brief Rep for visualizing a DistanceMetric.
 *
 * \ingroup sscRep
 * \ingroup sscRep3D
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

	void setDistanceMetric(DistanceMetricPtr point);
	virtual QString getType() const { return "ssc::DistanceMetricRep"; }

protected:
	virtual void addRepActorsToViewRenderer(ssc::View *view);
	virtual void removeRepActorsFromViewRenderer(ssc::View *view);

protected slots:
	virtual void changedSlot();

private:
	DistanceMetricRep(const QString& uid, const QString& name = "");
	DistanceMetricRep(); ///< not implemented

	ssc::GraphicalLine3DPtr mGraphicalLine;
	ssc::CaptionText3DPtr mText;
	DistanceMetricPtr mMetric;
	ssc::View *mView;
};

}

#endif /* SSCDISTANCEMETRICREP_H_ */
