/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXVIEWFOLLOWER_H
#define CXVIEWFOLLOWER_H

#include "cxResourceVisualizationExport.h"

#include "boost/scoped_ptr.hpp"
#include <QObject>
#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include "cxBoundingBox3D.h"
#include "cxSliceAutoViewportCalculator.h"

namespace cx
{

class SliceAutoViewportCalculator;
typedef boost::shared_ptr<class RegionOfInterestMetric> RegionOfInterestMetricPtr;
typedef boost::shared_ptr<class ViewFollower> ViewFollowerPtr;



/**
 * Ensure the tool is inside a given viewport, by moving the global center.
 *
 * \ingroup cx_resource_view
 * \date 2014-01-14
 * \author christiana
 */
class cxResourceVisualization_EXPORT ViewFollower : public QObject
{
	Q_OBJECT
public:
	static ViewFollowerPtr create(PatientModelServicePtr dataManager);
	void setSliceProxy(SliceProxyPtr sliceProxy);
	void setView(DoubleBoundingBox3D bb_s);
	void setAutoZoomROI(QString uid);

	SliceAutoViewportCalculator::ReturnType calculate();
	Vector3D findCenter_r_fromShift_s(Vector3D shift_s);

	~ViewFollower();

signals:
	void newZoom(double);

private:
	explicit ViewFollower(PatientModelServicePtr dataManager);

	SliceProxyPtr mSliceProxy;
	DoubleBoundingBox3D mBB_s;
	PatientModelServicePtr mDataManager;
	QString mRoi;

	boost::scoped_ptr<SliceAutoViewportCalculator> mCalculator;

	Vector3D findVirtualTooltip_s();
	DoubleBoundingBox3D getROI_BB_s();
};


} // namespace cx

#endif // CXVIEWFOLLOWER_H
