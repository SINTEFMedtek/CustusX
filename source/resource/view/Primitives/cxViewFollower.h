/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
