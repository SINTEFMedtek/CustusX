/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXGUIDEREP2D_H_
#define CXGUIDEREP2D_H_

#include "cxResourceVisualizationExport.h"

#include "cxDataMetricRep.h"
#include "cxGraphicalPrimitives.h"
#include "cxPointMetric.h"
#include "cxViewportListener.h"

namespace cx
{

typedef boost::shared_ptr<class GuideRep2D> GuideRep2DPtr;

/**Rep for visualizing a PointMetric in 2D views.
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep2D
 *
 * \date Jun 14, 2012
 * \author Sigmund Augdal, SonoWand AS
 */
class cxResourceVisualization_EXPORT GuideRep2D: public DataMetricRep
{
Q_OBJECT
public:
	static GuideRep2DPtr New(PatientModelServicePtr dataManager, const QString& uid="");
	virtual ~GuideRep2D() {}

	virtual QString getType() const { return "GuideRep2D"; }
	void setSliceProxy(SliceProxyPtr slicer);
	/**
	 * Set the width of the outline in fractions of the full size
	 */
	void setOutlineWidth(double width);

	void setRequestedAccuracy(double accuracy);

protected:
    virtual void clear();
	virtual void onModifiedStartRender();

private:
	GuideRep2D(PatientModelServicePtr dataManager);
	GuideRep2D(); ///< not implemented

	PatientModelServicePtr mDataManager;
	SliceProxyPtr mSliceProxy;
	vtkActorPtr mCircleActor;
	vtkSectorSourcePtr mCircleSource;
	double mOutlineWidth;
	double mRequestedAccuracy;
};

}

#endif /* CXGUIDEREP2D_H_ */
