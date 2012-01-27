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

/*
 * cxDistanceMetricRep.h
 *
 *  \date Jul 5, 2011
 *      \author christiana
 */

#ifndef CXDISTANCEMETRICREP_H_
#define CXDISTANCEMETRICREP_H_

#include "cxDataMetricRep.h"
#include "sscGraphicalPrimitives.h"
#include "cxDistanceMetric.h"
#include "vtkForwardDeclarations.h"

#include "vtkTextActor.h"
typedef vtkSmartPointer<vtkTextActor> vtkTextActorPtr;

namespace cx
{

typedef boost::shared_ptr<class DistanceMetricRep> DistanceMetricRepPtr;

/**\brief Rep for visualizing a DistanceMetric.
 * \ingroup cxServiceVisualizationRep
 *
 */
class DistanceMetricRep: public DataMetricRep
{
Q_OBJECT
public:
	static DistanceMetricRepPtr New(const QString& uid, const QString& name = ""); ///constructor
	virtual ~DistanceMetricRep() {}

	void setDistanceMetric(DistanceMetricPtr point);
	virtual QString getType() const { return "cx::DistanceMetricRep"; }

protected:
	virtual void addRepActorsToViewRenderer(ssc::View* view);
	virtual void removeRepActorsFromViewRenderer(ssc::View* view);

protected slots:
	virtual void changedSlot();

private:
	DistanceMetricRep(const QString& uid, const QString& name = "");
	DistanceMetricRep(); ///< not implemented

	ssc::GraphicalLine3DPtr mGraphicalLine;
	ssc::CaptionText3DPtr mText;
	DistanceMetricPtr mMetric;
	ssc::View* mView;
};

}

#endif /* CXDISTANCEMETRICREP_H_ */
