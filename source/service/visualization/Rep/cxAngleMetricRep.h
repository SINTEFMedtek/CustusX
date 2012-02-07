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
 * cxAngleMetricRep.h
 *
 *  \date Jul 27, 2011
 *      \author christiana
 */

#ifndef CXANGLEMETRICREP_H_
#define CXANGLEMETRICREP_H_

#include "cxDataMetricRep.h"
#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"

typedef vtkSmartPointer<class vtkTextActor> vtkTextActorPtr;

namespace ssc
{
typedef boost::shared_ptr<class CaptionText3D> CaptionText3DPtr;
typedef boost::shared_ptr<class GraphicalArc3D> GraphicalArc3DPtr;
}

namespace cx
{

typedef boost::shared_ptr<class AngleMetricRep> AngleMetricRepPtr;
typedef boost::shared_ptr<class AngleMetric> AngleMetricPtr;

/**Rep for visualizing a DistanceMetric.
 * \ingroup cxServiceVisualizationRep
 *
 */
class AngleMetricRep: public DataMetricRep
{
Q_OBJECT
public:
	static AngleMetricRepPtr New(const QString& uid, const QString& name = ""); ///constructor
	virtual ~AngleMetricRep() {}

	void setMetric(AngleMetricPtr point);
	virtual QString getType() const { return "cx::AngleMetricRep"; }

protected:
	virtual void addRepActorsToViewRenderer(ssc::View* view);
	virtual void removeRepActorsFromViewRenderer(ssc::View* view);

protected slots:
	virtual void changedSlot();

private:
	AngleMetricRep(const QString& uid, const QString& name = "");
	AngleMetricRep(); ///< not implemented

	ssc::GraphicalLine3DPtr mLine0;
	ssc::GraphicalLine3DPtr mLine1;
	ssc::GraphicalArc3DPtr mArc;
	ssc::CaptionText3DPtr mText;
	AngleMetricPtr mMetric;
	ssc::View* mView;
};

}

#endif /* CXANGLEMETRICREP_H_ */
